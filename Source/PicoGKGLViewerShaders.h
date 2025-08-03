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

#ifndef PICOGKGLVIEWERSHADERS_H_
#define PICOGKGLVIEWERSHADERS_H_

#include "PicoGKGLShader.h"
#include "PicoGKTypes.h"
#include "PicoGKGLObjects.h"

namespace PicoGK
{

class ShaderProgMeshPoly: public GlShaderProgram
{
public:
    ShaderProgMeshPoly();
    
    void SetLightingTextures(   const char* pDiffuseTextureDDS,
                                uint32_t nDiffuseBufferSize,
                                const char* pSpecularTextureDDS,
                                uint32_t nSpecularBufferSize);
    
    void CreateBufferInstance(  const std::vector<Vector3> vVertices,
                                std::unique_ptr<GlVertexBuffer<Vector3>>* prResult) const;
    
    void CreateBufferInstance(  const std::vector<Vector3> vVertices,
                                const std::vector<Triangle> vTriangles,
                                std::unique_ptr<GlElementBuffer<Vector3>>* prResult) const;
    
    void Use(   const Matrix4x4&    matMVP,
                const Vector3&      vecEye) const;
    
    void SetValues( const Matrix4x4& mat,
                    const ColorFloat& clr,
                    float fMetallic     = 0.5f,
                    float fRoughness    = 0.5f) const;
    
    GLint nAttribPosition() const {return m_nAvec3Pos;}
    
private:
    static const std::string c_strVertShader;
    static const std::string c_strFragShader;
    
    GLuint  m_hTexSpecular  = 0;
    GLuint  m_hTexDiffuse   = 0;
    
    GLint m_nUmat4MVP       =-1;
    GLint m_nUmat4OtoW      =-1;
    GLint m_nUvec3Eye       =-1;
    GLint m_nUtexSpec       =-1;
    GLint m_nUfMetallic     =-1;
    GLint m_nUvec4Color     =-1;
    GLint m_nUfRoughness    =-1;
    GLint m_nUtexDiff       =-1;

    GLint m_nAvec3Pos       = -1;
};


    
} // namespace PicoGK

#endif

