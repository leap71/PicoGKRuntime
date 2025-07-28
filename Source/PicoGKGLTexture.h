//
// SPDX-License-Identifier: Apache-2.0
//
// PicoGK ("peacock") is a compact software kernel for computational geometry,
// specifically for use in Computational Engineering Models (CEM).
//
// For more information, please visit https://picogk.org
//
// PicoGK is developed and maintained by LEAP 71 - © 2023-2024 by LEAP 71
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

#ifndef PICOGKGLTEXTURE_H_
#define PICOGKGLTEXTURE_H_

#include "PicoGKTypes.h"
#include "PicoGKApiTypes.h"
#include <string>
#include <mutex>
#include <map>

namespace PicoGK
{

bool bLoadDdsTexture(   const char* pBuffer,
                        int32_t nSize,
                        GLenum eTarget);


class GpuTextureRgba
{
public:
    GpuTextureRgba( int nWidth,
                    int nHeight,
                    const char* pSource)
    :   PKINIT(nWidth),
        PKINIT(nHeight)
    {
        int nSize = nWidth * nHeight * 4;
        
        m_pBuffer = new char[nSize];
        
        memcpy( m_pBuffer,
                pSource,
                nSize);
    }
    
    virtual ~GpuTextureRgba()
    {
        if (m_pBuffer)
            delete [] m_pBuffer;
        
        if (m_nGlHandle != INVALID_GL_ID)
        {
            glDeleteTextures(1, &m_nGlHandle);
            m_nGlHandle = INVALID_GL_ID;
        }
    }
    
    GLuint nGlHandle() const
    {
        assert(!bNeedsUpload());
        
        return m_nGlHandle;
    }
    
    bool bNeedsUpload() const
    {
        return m_pBuffer != nullptr;
    }
    
    void TransferToGpu()
    {
        assert(bNeedsUpload());
        
        glGenTextures(1, &m_nGlHandle);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glBindTexture(GL_TEXTURE_2D, m_nGlHandle);
        
        glTexImage2D(   GL_TEXTURE_2D,
                        0,
                        GL_SRGB8_ALPHA8,
                        m_nWidth, m_nHeight,
                        0,
                        GL_RGBA,
                        GL_UNSIGNED_BYTE,
                        m_pBuffer);
        
        glGenerateMipmap(GL_TEXTURE_2D);
        
        delete [] m_pBuffer;
        m_pBuffer = nullptr;
    }
        
protected:
    int     m_nWidth;
    int     m_nHeight;
    char*   m_pBuffer;
    GLuint  m_nGlHandle = INVALID_GL_ID;
    
    static constexpr GLuint INVALID_GL_ID = 0;
};

class GpuTextureList
{
public:
    
    uint64_t hAdd(  int nWidth,
                    int nHeight,
                    const char* pData)
    {
        std::lock_guard lk(m_mtx);
        m_hCurrent++;
        
        m_oNew.emplace(m_hCurrent, GpuTextureRgba(nWidth, nHeight, pData));
        return m_hCurrent;
    }
    
    void MarkForDestruction(uint64_t hObject)
    {
        std::lock_guard lk(m_mtx);
        
        // First check if this is still in the "new" pipeline
        if (auto node = m_oNew.extract(hObject))
        {
            m_oCleanUp.insert(std::move(node));
            return;
        }
        
        // It must be in the active pipeline
        if (auto node = m_oActive.extract(hObject))
        {
            m_oCleanUp.insert(std::move(node));
            return;
        }
        
        throw std::out_of_range("Invalid Texture Handle " + std::to_string(hObject));
    }
    
    /// Called with the current GL context active to activate and cleanup all textures
    void ManageTextureState()
    {
        std::lock_guard lk(m_mtx);
        
        for (auto it = m_oNew.begin(); it != m_oNew.end();)
        {
            auto node = m_oNew.extract(it++);
            node.mapped().TransferToGpu();
            m_oActive.insert(std::move(node));
        }
        
        m_oCleanUp.clear();
    }
    
    void CleanupAllTextures()
    {
        std::lock_guard lk(m_mtx);
        m_oNew.clear();
        m_oActive.clear();
        m_oCleanUp.clear();
    }
    
    mutable std::mutex m_mtx;
    uint64_t                                        m_hCurrent = 0;
    std::unordered_map<uint64_t, GpuTextureRgba>    m_oNew;
    std::unordered_map<uint64_t, GpuTextureRgba>    m_oActive;
    std::unordered_map<uint64_t, GpuTextureRgba>    m_oCleanUp;
};

}

#endif // #ifndef PICOGKGLTEXTURE_H_

