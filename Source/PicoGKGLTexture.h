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
#include <unordered_map>
#include <memory>
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
        Refresh(pSource);
    }
    
    void Refresh(const char* pSource)
    {
        m_pBuffer = std::make_unique<char[]>(nMemSize());
        
        memcpy( m_pBuffer.get(),
                pSource,
                nMemSize());
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
    
    int nMemSize() const
    {
        return m_nWidth * m_nHeight * 4;
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
        
        if (m_nGlHandle == INVALID_GL_ID)
        {
            PKTRACE(GpuTextureRgba_TransferToGpu_CreateNew);
            // First time, create a new texture
            
            glGenTextures(1, &m_nGlHandle);
            glBindTexture(GL_TEXTURE_2D, m_nGlHandle);
            
            glTexImage2D(   GL_TEXTURE_2D,
                            0,
                            GL_SRGB8_ALPHA8,
                            m_nWidth, m_nHeight,
                            0,
                            GL_RGBA,
                            GL_UNSIGNED_BYTE,
                            nullptr); // allocate but don't upload yet

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glBindTexture(GL_TEXTURE_2D, m_nGlHandle);
        
        {
            PKTRACE(GpuTextureRgba_TransferToGpu_CopyToGpuMem);
            
            PKTRACE(GpuTextureRgba_TransferToGpu_glTexImage2D);
            
            glTexSubImage2D( GL_TEXTURE_2D,
                             0,
                             0, 0,
                             m_nWidth, m_nHeight,
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
    
    void Refresh(   uint64_t hObject,
                    const char* pData)
    {
        PKTRACE(GpuTextureList_Refresh);
        std::lock_guard lk(m_mtx);
        
        // First check if this is still in the "new" pipeline
        if (auto node = m_oNew.extract(hObject))
        {
            node.mapped()->Refresh(pData);
            m_oRefresh.insert(std::move(node));
            return;
        }
        
        // Is it in the active pipeline?
        if (auto node = m_oActive.extract(hObject))
        {
            node.mapped()->Refresh(pData);
            m_oRefresh.insert(std::move(node));
            return;
        }
        
        // Maybe refresh is called too quickly and it
        // hasn't been refreshed yet
        
        auto it = m_oRefresh.find(hObject);
        
        if (it == m_oRefresh.end())
            throw std::out_of_range("Invalid Texture Handle " + std::to_string(hObject));
        
        // Refresh in place
        it->second->Refresh(pData);
    }
    
    /// Called with the current GL context active to activate and cleanup all textures
    void ManageTextureState()
    {
        PKTRACE(GpuTextureList_ManageTextureState);
        std::lock_guard lk(m_mtx);
        
        while (!m_oNew.empty())
        {
            auto node = m_oNew.extract(m_oNew.begin());
            node.mapped()->TransferToGpu();
            m_oActive.insert(std::move(node));
        }
        
        while (!m_oRefresh.empty())
        {
            auto node = m_oRefresh.extract(m_oRefresh.begin());
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
        m_oRefresh.clear();
        m_oCleanUp.clear();
    }
    
    void ShowAllTextures() const
    {
        PKTRACE(GpuTextureList_ShowAllTextures);
        std::lock_guard lk(m_mtx);
        
        for (const auto& node : m_oActive)
        {
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
    
    class UseTexture
    {
    public:
        UseTexture( const GpuTextureList& oList,
                    uint64_t hObject)
        : m_lock(oList.m_mtx)
        {
            // Mutex lock locks list for lifetime of this object
            PKTRACE(GpuTextureList_UseTexture_Constructor);
            
            auto it = oList.m_oActive.find(hObject);
            if (it == oList.m_oActive.end())
            {
                m_nGlTexHandle  = 0;
                m_nWidth        = 0;
                m_nHeight       = 0;
                return;
            }
            
            m_nGlTexHandle  = it->second.get()->nGlHandle();
            m_nWidth        = it->second.get()->nWidth();
            m_nHeight       = it->second.get()->nHeight();
        }
        
        ~UseTexture()
        {
            // mutex lock automatically unlocks when going out of scope
        }
        
        bool bFound() const
        {
            return (m_nGlTexHandle != 0);
        }
        
        GLuint nGlTexHandle() const
        {
            return m_nGlTexHandle;
        }
        
        int32_t nWidth() const
        {
            return m_nWidth;
        }
        
        int32_t nHeight() const
        {
            return m_nHeight;
        }
        
    protected:
        std::lock_guard<std::mutex> m_lock;
        GLuint                      m_nGlTexHandle;
        int32_t                     m_nWidth;
        int32_t                     m_nHeight;
        
    };
    
protected:
    mutable std::mutex m_mtx;
    uint64_t                                                        m_hCurrent = 0;
    std::unordered_map<uint64_t, std::unique_ptr<GpuTextureRgba>>   m_oNew;
    std::unordered_map<uint64_t, std::unique_ptr<GpuTextureRgba>>   m_oActive;
    std::unordered_map<uint64_t, std::unique_ptr<GpuTextureRgba>>   m_oRefresh;
    std::unordered_map<uint64_t, std::unique_ptr<GpuTextureRgba>>   m_oCleanUp;
};

}

#endif // #ifndef PICOGKGLTEXTURE_H_

