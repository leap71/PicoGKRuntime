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

#ifndef PICOGKGLOBJECTS_H_
#define PICOGKGLOBJECTS_H_

#include <cstddef>
#include <vector>
#include "PicoGKTypes.h"

namespace PicoGK
{

template<typename T>
struct GlVertexAttribTraits;

template<>
struct GlVertexAttribTraits<Vector3>
{
    static constexpr GLint nComponents = 3;
    static constexpr GLenum eType = GL_FLOAT;
    static constexpr GLboolean bNormalized = GL_FALSE;
    static constexpr size_t nOffset = 0;
};

template<>
struct GlVertexAttribTraits<Vector4>
{
    static constexpr GLint nComponents = 4;
    static constexpr GLenum eType = GL_FLOAT;
    static constexpr GLboolean bNormalized = GL_FALSE;
    static constexpr size_t nOffset = 0;
};

template<>
struct GlVertexAttribTraits<Vector2>
{
    static constexpr GLint nComponents = 2;
    static constexpr GLenum eType = GL_FLOAT;
    static constexpr GLboolean bNormalized = GL_FALSE;
    static constexpr size_t nOffset = 0;
};

template<typename T>
class GlBind
{
public:
    explicit GlBind(const T& oToBind)
    : m_oToBind(oToBind)
    {
        m_oToBind.Bind();
    }
    
    ~GlBind()
    {
        m_oToBind.UnBind();
    }
    
protected:
    const T& m_oToBind;
};

template<typename T>
class GlVertexBuffer
{
public:
    GlVertexBuffer(     const std::vector<T>& vVertices,
                        GLenum eUsage = GL_STATIC_DRAW)
    {
        if (vVertices.size() == 0)
            return;
        
        m_nVertexCount = static_cast<int32_t>(vVertices.size());
        
        glGenVertexArrays(1, &m_nVAO);
        
        glGenBuffers(1, &m_nVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(T) * vVertices.size(), vVertices.data(), eUsage);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    ~GlVertexBuffer()
    {
        if (m_nVBO)
            glDeleteBuffers(1, &m_nVBO);
        if (m_nVAO)
            glDeleteVertexArrays(1, &m_nVAO);
    }
    
    bool bIsEmpty() const
    {
        return m_nVertexCount < 1;
    }
    
    int32_t nVertexCount() const
    {
        return m_nVertexCount;
    }
    
    void BindToShaderAttrib(GLuint nAttribLocation) const
    {
       glEnableVertexAttribArray(nAttribLocation);
        
        glVertexAttribPointer(  nAttribLocation,
                                GlVertexAttribTraits<T>::nComponents,
                                GlVertexAttribTraits<T>::eType,
                                GlVertexAttribTraits<T>::bNormalized,
                                static_cast<GLsizei>(sizeof(T)),
                                nullptr );
    }
    
    GlVertexBuffer(const GlVertexBuffer&)               = delete;
    GlVertexBuffer& operator=(const GlVertexBuffer&)    = delete;
    
protected:
    int32_t m_nVertexCount  = 0;
    GLuint m_nVAO           = 0;
    GLuint m_nVBO           = 0;
    
    template<typename> friend class GlBind;
    
    void Bind() const
    {
        if (m_nVertexCount == 0)
            return;
        
        glBindVertexArray(m_nVAO);
    }
    
    void UnBind() const
    {
        if (m_nVertexCount == 0)
            return;
        
        glBindVertexArray(0);
    }
};
    
} // namespace PicoGK

#endif

