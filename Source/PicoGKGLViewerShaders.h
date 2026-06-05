//
// SPDX-License-Identifier: Apache-2.0
//
// PicoGK ("peacock") is a compact software kernel for computational geometry,
// specifically for use in Computational Engineering Models (CEM).
//
// For more information, please visit https://picogk.org
//
// PicoGK is developed and maintained by LEAP 71 - © 2023-2026 by LEAP 71
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

#ifndef PICOGKGLVIEWERSHADERS_H_
#define PICOGKGLVIEWERSHADERS_H_

#include "PicoGKGLShader.h"
#include "PicoGKTypes.h"
#include "PicoGKGLObjects.h"
#include <memory>

namespace PicoGK
{

class ShaderProgMeshPoly: protected GlShaderProgram
{
public:
    ShaderProgMeshPoly();
    
    ShaderProgMeshPoly(const std::string& strFragmentShader);
    
    void SetLightingTextures(   const char* pDiffuseTextureDDS,
                                uint32_t nDiffuseBufferSize,
                                const char* pSpecularTextureDDS,
                                uint32_t nSpecularBufferSize);
    
    void CreateBufferInstance(  const std::vector<Vector3>& vVertices,
                                std::unique_ptr<GlVertexBuffer<Vector3>>* prResult) const;
    
    void CreateBufferInstance(  const std::vector<Vector3>& vVertices,
                                const std::vector<Triangle>& vTriangles,
                                std::unique_ptr<GlElementBuffer<Vector3>>* prResult) const;
    
    void Use(   const Matrix4x4&    matVP,
                const Vector3&      vecEye) const;
    
    void SetValues( const Matrix4x4&    matOtoW,
                    const ColorFloat&   clr,
                    float               fMetallic       = 0.5f,
                    float               fRoughness      = 0.5f,
                    bool                bWarnOverhang   = false,
                    float               fWarning        = 0.50f,            // 50% = 45º (0%   is vertical)
                    float               fError          = 0.61f) const;     // 61% = 55º (100% is horizontal)
    
private:
    void Init();
    
    static const std::string c_strVertShader;
    static const std::string c_strFragShader;
    
    GLuint  m_hTexSpecular              = 0;
    GLuint  m_hTexDiffuse               = 0;
    
    GLint m_nUmat4VP                    =-1;
    GLint m_nUmat4OtoW                  =-1;
    GLint m_nUvec3Eye                   =-1;
    GLint m_nUtexSpec                   =-1;
    GLint m_nUfMetallic                 =-1;
    GLint m_nUvec4Color                 =-1;
    GLint m_nUfRoughness                =-1;
    GLint m_nUtexDiff                   =-1;
    GLint m_nUbWarnOverhang             =-1;
    GLint m_nUfOverhangWarningDotDown   =-1;
    GLint m_nUfOverhangErrorDotDown     =-1;

    GLint m_nAvec3Pos                   = -1;
};

class ShaderProgMeshPolyOit : public ShaderProgMeshPoly
{
public:
    ShaderProgMeshPolyOit();
    
private:
    static const std::string c_strFragShaderOit;
};

class ShaderProgOitComposite : protected GlShaderProgram
{
public:
    ShaderProgOitComposite();
    
    ~ShaderProgOitComposite();
    
    void Use(   GLuint nAccum,
                GLuint nReveal);
    
private:
    static const std::string c_strVertShader;
    static const std::string c_strFragShader;
    
    GLint   m_nUtexAccum        =-1;
    GLint   m_nUtexReveal       =-1;
    
    GLuint m_nVAO = 0;
    GLuint m_nVBO = 0;
};

class ShaderProgQuad: protected GlShaderProgram
{
public:
    ShaderProgQuad();
    
    virtual ~ShaderProgQuad()
    {
        if (m_nVBO)
            glDeleteBuffers(1, &m_nVBO);
            
        if (m_nVAO)
            glDeleteVertexArrays(1, &m_nVAO);
    }
    
    void Use()
    {
        glBindVertexArray(m_nVAO);
        GlShaderProgram::Use();
    }
    
    void DrawQuad(  const Matrix4x4& matMVP,
                    bool bRenderSolid,
                    ColorFloat clrSolid,
                    float fAlpha,
                    GLuint hTexture,
                    bool bFlipX,
                    bool bFlipY,
                    bool bDoubleSided) const;
    
    ShaderProgQuad(const ShaderProgQuad&)               = delete;
    ShaderProgQuad& operator=(const ShaderProgQuad&)    = delete;
    ShaderProgQuad(ShaderProgQuad&&)                    = delete;
    ShaderProgQuad& operator=(ShaderProgQuad&&)         = delete;
    
private:
    static const std::string c_strVertShader;
    static const std::string c_strFragShader;
    
    GLint m_nUmat4MVP           = -1;
    GLint m_nUbFlipX            = -1;
    GLint m_nUbFlipY            = -1;
    GLint m_nUbRenderSolid      = -1;
    GLint m_nUvec4SolidColor    = -1;
    GLint m_nUtexTexture        = -1;
    GLint m_nUfAlpha            = -1;

    GLint m_nAvec3Pos           = -1;
    GLint m_nAvec2InUV          = -1;
    
private:
    // GL buffer objects to store one quad
    // that is re-used
    
    GLuint m_nVAO = 0;
    GLuint m_nVBO = 0;
};
    
} // namespace PicoGK

#endif

