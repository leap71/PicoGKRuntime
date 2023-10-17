//
// SPDX-License-Identifier: Apache-2.0
//
// PicoGK ("peacock") is a compact software kernel for computational geometry,
// specifically for use in Computational Engineering Models (CEM).
//
// For more information, please visit https://picogk.org
//
// PicoGK is developed and maintained by LEAP 71 - © 2023 by LEAP 71
// https://leap71.com
//
// Computational Engineering will profoundly change our physical world in the
// years ahead. Thank you for being part of the journey.
//
// We have developed this library to be used widely, for both commercial and
// non-commercial projects alike. Therefore, have released it under a permissive
// open-source license.
//
// The foundation of PicoGK is a thin layer on top of the powerful open-source
// OpenVDB project, which in turn uses many other Free and Open Source Software
// libraries. We are grateful to be able to stand on the shoulders of giants.
//
// LEAP 71 licenses this file to you under the Apache License, Version 2.0
// (the "License"); you may not use this file except in compliance with the
// License. You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, THE SOFTWARE IS
// PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED.
//
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "gl/gl.h"
#include "PicoGKGLTexture.h"

#include <memory>
#include <fstream>
#include <vector>

#pragma pack(1)

namespace
{

struct SDDSPixelFormat
{
    uint32_t nSize;
    uint32_t nFlags;
    uint32_t nFourCC;
    uint32_t nRGBBitCount;
    uint32_t nRBitMask;
    uint32_t nGBitMask;
    uint32_t nBBitMask;
    uint32_t nABitMask;
};

struct SDDSHeader
{
    uint32_t nSize;
    uint32_t nFlags;
    uint32_t nHeight;
    uint32_t nWidth;
    uint32_t nPitchOrLinearSize;
    uint32_t nDepth;
    uint32_t nMipMapCount;
    uint32_t nReserved1[11];
    SDDSPixelFormat sPixelFormat;
    uint32_t nCaps1;
    uint32_t nCaps2;
    uint32_t nReserved2[3];
};

struct SDDSHeader10
{
    uint32_t nFormat;
    uint32_t nResourceDimension;
    uint32_t nMiscFlags;
    uint32_t nArraySize;
    uint32_t nReserved;
};

#pragma pack()

#define DDSFLAGS_CUBEMAP        0x00000200
#define DDSFLAGS_VOLUME            0x00200000

#define DDSFOURCC_DX10            0x30315844

static const GLenum g_aeCubeSides[] =
{
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

static void FlipUpsideDown( void* pBuffer,
                            uint32_t nLineStride,
                            uint32_t nHeight)
{
    uint8_t* pbyTop        = (uint8_t*)pBuffer;
    uint8_t* pbyBottom    = pbyTop + nLineStride * (nHeight - 1);

    for (uint32_t nY = 0; nY < (nHeight >> 1); nY++)
    {
        for (uint32_t nX = 0; nX < nLineStride; nX++)
        {
            uint8_t byTemp    = *pbyTop;
            *pbyTop            = *pbyBottom;
            *pbyBottom        = byTemp;

            pbyTop++;
            pbyBottom++;
        }

        pbyBottom -= nLineStride * 2;
    }
}

}    //namespace

namespace PicoGK
{

bool bLoadDdsTexture(   const char* pBuffer,
                        int32_t nSize,
                        GLenum eTarget)
{
    if (nSize < (4 + sizeof(SDDSHeader))) // Check if the buffer is too small to contain DDS header
    {
        return false;
    }

    const char* achMagic = pBuffer;
    if (strncmp(achMagic, "DDS ", 4) != 0)
    {
        return false;
    }

    const SDDSHeader* psHeader = reinterpret_cast<const SDDSHeader*>(pBuffer + 4);

    if ((eTarget == GL_TEXTURE_CUBE_MAP) != !!(psHeader->nCaps2 & DDSFLAGS_CUBEMAP) || psHeader->nCaps2 & DDSFLAGS_VOLUME)
    {
        return false;
    }

    uint32_t nTotalHeaderSize = 4 + sizeof(SDDSHeader);

    if (psHeader->sPixelFormat.nFourCC == DDSFOURCC_DX10)
    {
        if (nSize < (nTotalHeaderSize + sizeof(SDDSHeader10)))
        {
            return false;
        }
        nTotalHeaderSize += sizeof(SDDSHeader10);
    }

    uint32_t nComponents = 4;
    uint32_t nBytesPerComponent = 1;
    GLenum eInternalFormat = GL_RGBA8;
    GLenum eBufferFormat = GL_BGRA;
    GLenum eBufferType = GL_UNSIGNED_BYTE;

    if (psHeader->sPixelFormat.nRGBBitCount == 64)
    {
        eInternalFormat        = GL_RGBA16F;
        eBufferFormat         = GL_RGBA;
        eBufferType         = GL_HALF_FLOAT;
        nBytesPerComponent     = 2;
    }
    else if (psHeader->sPixelFormat.nRGBBitCount == 48)
    {
        nComponents         = 3;
        nBytesPerComponent     = 2;
        eInternalFormat     = GL_RGB16F;
        eBufferFormat         = GL_RGB;
        eBufferType         = GL_HALF_FLOAT;
    }
    else if (psHeader->sPixelFormat.nRGBBitCount == 24)
    {
        nComponents     = 3;
        eBufferFormat     = GL_BGR;
    }
    else if (psHeader->sPixelFormat.nRGBBitCount == 8)
    {
        nComponents     = 1;
        eBufferFormat     = GL_RED;
    }
    else if (psHeader->sPixelFormat.nRGBBitCount != 32)
    {
        return false;
    }
    
    if (psHeader->nMipMapCount < 1)
    {
        return false;
    }
    
    uint32_t nMaxReadSize = psHeader->nWidth * psHeader->nHeight * nComponents * nBytesPerComponent;

    std::vector<uint8_t> oBuffer(nMaxReadSize);

    size_t nBufferOffset = nTotalHeaderSize; // Start offset within the buffer

    for (uint32_t nSide = 0; nSide < uint32_t(eTarget == GL_TEXTURE_CUBE_MAP ? 6 : 1); nSide++)
    {
        uint32_t nWidth     = psHeader->nWidth;
        uint32_t nHeight    = psHeader->nHeight;

        GLenum eTexImageTarget = (eTarget == GL_TEXTURE_CUBE_MAP) ? g_aeCubeSides[nSide] : eTarget;

        for (uint32_t nMipLevel = 0; nMipLevel < psHeader->nMipMapCount; nMipLevel++)
        {
            uint32_t nMipSize = nWidth * nHeight * nComponents * nBytesPerComponent;

            if (nBufferOffset + nMipSize > nSize)
            {
                return false; // Not enough data in the buffer
            }

            std::memcpy(oBuffer.data(), pBuffer + nBufferOffset, nMipSize);
            nBufferOffset += nMipSize;

            FlipUpsideDown(oBuffer.data(), nWidth * nComponents * nBytesPerComponent, nHeight);

            glTexImage2D(
                eTexImageTarget,
                nMipLevel,
                eInternalFormat,
                nWidth,
                nHeight,
                0,
                eBufferFormat,
                eBufferType,
                oBuffer.data()
            );

            nWidth >>= 1;
            if (!nWidth)
            {
                nWidth = 1;
            }
            nHeight >>= 1;
            if (!nHeight)
            {
                nHeight = 1;
            }
        }
    }

    return true;
}

bool bLoadDdsTexture(   const std::string& strFileName,
                        GLenum eTarget)
{
    std::ifstream oIn(strFileName, std::ios_base::in | std::ios_base::binary);
    
    if (!oIn)
    {
        return false;
    }
    
    // Determine size
    oIn.seekg(0, std::ios::end);

    // Get the position, which will be the size of the file
    std::streampos nSize = oIn.tellg();
    // Seek back to the beginning
    oIn.seekg(0, std::ios::beg);
    
    std::vector<char> oBuffer(nSize);
    
    oIn.read(oBuffer.data(), nSize);
    
    return bLoadDdsTexture(oBuffer.data(), (int32_t) nSize, eTarget);
}


} // namespace PicoGK
