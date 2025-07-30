//
// SPDX-License-Identifier: Apache-2.0
//
// PicoGK ("peacock") is a compact software kernel for computational geometry,
// specifically for use in Computational Engineering Models (CEM).
//
// For more information, please visit https://picogk.org
//
// PicoGK is developed and maintained by LEAP 71 - © 2023-2025 by LEAP 71
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

#include "PicoGKTrace.h"

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
        
        m_pBuffer = std::make_unique<char[]>(nSize);
        
        memcpy( m_pBuffer.get(),
                pSource,
                nSize);
    }
    
    virtual ~GpuTextureRgba()
    {
        PKTRACE(GpuTextureRgba_Destructor);
        
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
    
    int nWidth() const
    {
        return m_nWidth;
    }
    
    int nHeight() const
    {
        return m_nHeight;
    }
    
    bool bNeedsUpload() const
    {
        return m_pBuffer != nullptr;
    }
    
    void TransferToGpu()
    {
        PKTRACE(GpuTextureRgba_TransferToGpu);
        assert(bNeedsUpload());
        
        glGenTextures(1, &m_nGlHandle);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glBindTexture(GL_TEXTURE_2D, m_nGlHandle);
        
        {
            PKTRACE(GpuTextureRgba_TransferToGpu_glTexImage2D);
            
            glTexImage2D(   GL_TEXTURE_2D,
                            0,
                            GL_SRGB8_ALPHA8,
                            m_nWidth, m_nHeight,
                            0,
                            GL_RGBA,
                            GL_UNSIGNED_BYTE,
                            m_pBuffer.get());
            
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        
        {
            PKTRACE(GpuTextureRgba_TransferToGpu_DeleteBuffer);
            m_pBuffer.reset();
        }
    }
        
protected:
    int                         m_nWidth    = 0;
    int                         m_nHeight   = 0;
    std::unique_ptr<char[]>     m_pBuffer   = nullptr;
    GLuint                      m_nGlHandle = INVALID_GL_ID;
    
    static constexpr GLuint INVALID_GL_ID = 0;
};

class GpuTextureList
{
public:
    uint64_t hAdd(  int nWidth,
                    int nHeight,
                    const char* pData)
    {
        PKTRACE(GpuTextureList_hAdd);
        std::lock_guard lk(m_mtx);
        m_hCurrent++;
        
        m_oNew.emplace(m_hCurrent, std::make_unique<GpuTextureRgba>(nWidth, nHeight, pData));
        return m_hCurrent;
    }
    
    void MarkForDestruction(uint64_t hObject)
    {
        PKTRACE(GpuTextureList_MarkForDestruction);
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
        PKTRACE(GpuTextureList_ManageTextureState);
        std::lock_guard lk(m_mtx);
        
        while (!m_oNew.empty())
        {
            PKTRACE(GpuTextureList_ManageTextureState_Loop);
            auto node = m_oNew.extract(m_oNew.begin());
            node.mapped()->TransferToGpu();
            m_oActive.insert(std::move(node));
        }
        
        m_oCleanUp.clear();
    }
    
    void CleanupAllTextures()
    {
        PKTRACE(GpuTextureList_CleanupAllTextures);
        std::lock_guard lk(m_mtx);
        m_oNew.clear();
        m_oActive.clear();
        m_oCleanUp.clear();
    }
    
    void ShowAllTextures() const
    {
        PKTRACE(GpuTextureList_ShowAllTextures);
        std::lock_guard lk(m_mtx);
        
        for (const auto& node : m_oActive)
        {
            PKTRACE(GpuTextureList_ShowAllTextures_Loop);

            if (node.second->nHeight() == 0)
                continue;

            float fAspect = (float)node.second->nWidth() / (float)node.second->nHeight();
            ImVec2 vecAvailable = ImGui::GetContentRegionAvail();
            float fAvailableAspect = vecAvailable.x / vecAvailable.y;

            ImVec2 vecSize;
            if (fAspect > fAvailableAspect)
            {
                vecSize.x = vecAvailable.x;
                vecSize.y = vecAvailable.x / fAspect;
            }
            else
            {
                vecSize.y = vecAvailable.y;
                vecSize.x = vecAvailable.y * fAspect;
            }

            ImTextureID texID = (ImTextureID)(intptr_t)node.second->nGlHandle();
            ImGui::Image(texID, vecSize);

            ImGui::Dummy(ImVec2(0, 10)); // spacing
        }
    }
    
    mutable std::mutex m_mtx;
    uint64_t                                                        m_hCurrent = 0;
    std::unordered_map<uint64_t, std::unique_ptr<GpuTextureRgba>>   m_oNew;
    std::unordered_map<uint64_t, std::unique_ptr<GpuTextureRgba>>   m_oActive;
    std::unordered_map<uint64_t, std::unique_ptr<GpuTextureRgba>>   m_oCleanUp;
};

}

#endif // #ifndef PICOGKGLTEXTURE_H_

