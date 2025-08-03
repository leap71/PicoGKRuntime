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

#include "gl/gl.h"
#include "PicoGKGLViewerShaders.h"
#include "PicoGKGLTexture.h"
#include <iostream>

namespace PicoGK
{

ShaderProgMeshPoly::ShaderProgMeshPoly()
: GlShaderProgram(c_strVertShader, c_strFragShader)
{
    std::cerr << strListUniforms()   << "\n";
    std::cerr << strListAttributes() << "\n";
    
    m_nUmat4MVP       = nUniformLoc("mat4MVP");
    m_nUmat4OtoW      = nUniformLoc("mat4OtoW");
    m_nUvec3Eye       = nUniformLoc("vec3Eye");
    m_nUtexSpec       = nUniformLoc("texSpec");
    m_nUfMetallic     = nUniformLoc("fMetallic");
    m_nUvec4Color     = nUniformLoc("vec4Color");
    m_nUfRoughness    = nUniformLoc("fRoughness");
    m_nUtexDiff       = nUniformLoc("texDiff");

    m_nAvec3Pos       = nAttribLoc("vec3Pos");
}

void ShaderProgMeshPoly::CreateBufferInstance(  const std::vector<Vector3> vVertices,
                                                std::unique_ptr<GlVertexBuffer<Vector3>>* prResult) const
{
    *prResult = std::make_unique<GlVertexBuffer<Vector3>>(m_nAvec3Pos, vVertices);
}

void ShaderProgMeshPoly::CreateBufferInstance(  const std::vector<Vector3> vVertices,
                                                const std::vector<Triangle> vTriangles,
                                                std::unique_ptr<GlElementBuffer<Vector3>>* prResult) const
{
    *prResult = std::make_unique<GlElementBuffer<Vector3>>(m_nAvec3Pos, vVertices, vTriangles);
}

void ShaderProgMeshPoly::Use(   const Matrix4x4& matMVP,
                                const Vector3& vecEye) const
{
    GlShaderProgram::Use();
    
    glUniformMatrix4fv( m_nUmat4MVP,  1,  GL_FALSE, (GLfloat*) &matMVP);
    glUniform3fv(       m_nUvec3Eye,  1,  (GLfloat*) &vecEye);
    
    glUniform1i(m_nUtexSpec,    m_hTexSpecular);
    glUniform1i(m_nUtexDiff,    m_hTexDiffuse);
}

void ShaderProgMeshPoly::SetValues( const Matrix4x4& mat,
                                    const ColorFloat& clr,
                                    float fMetallic,
                                    float fRoughness) const
{
    glUniformMatrix4fv( m_nUmat4OtoW,
                        1,
                        GL_FALSE,
                        (GLfloat*) &mat);
        
    glUniform4f(    m_nUvec4Color,
                    clr.R,
                    clr.G,
                    clr.B,
                    clr.A);
    
    glUniform1f(m_nUfMetallic, fMetallic);
    glUniform1f(m_nUfMetallic, fRoughness);
}

void ShaderProgMeshPoly::SetLightingTextures(   const char* pDiffuseTextureDDS,
                                                uint32_t nDiffuseBufferSize,
                                                const char* pSpecularTextureDDS,
                                                uint32_t nSpecularBufferSize)
{
    /// TODO clean this up and encapsulate the cubemapped texture properly
    
    
    glGenTextures(1, &m_hTexDiffuse);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_hTexDiffuse);
    
    if (!bLoadDdsTexture(pDiffuseTextureDDS, nDiffuseBufferSize, GL_TEXTURE_CUBE_MAP))
        throw ShaderProgramException("Failed to load diffuse texture dds");

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    // Texture 1 is the specular cube map for lighting

    glGenTextures(1, &m_hTexSpecular);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_hTexSpecular);

    if (!bLoadDdsTexture(pSpecularTextureDDS, nSpecularBufferSize, GL_TEXTURE_CUBE_MAP))
        throw ShaderProgramException("Failed to load specular texture dds");

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}
    
const std::string ShaderProgMeshPoly::c_strVertShader =
R"VS(
#version 330 core
in  vec3 vec3Pos;
out vec3 vec3World;
uniform mat4 mat4OtoW;
uniform mat4 mat4MVP;
void main()
{
    vec3World   = (mat4OtoW * vec4(vec3Pos, 1)).xyz;
    gl_Position = mat4MVP * vec4(vec3World, 1);
}
)VS";

const std::string ShaderProgMeshPoly::c_strFragShader = R"FS(
#version 330 core
#extension GL_ARB_shader_texture_lod : enable

#define GAMMA   vec3(0.45, 0.45, 0.45)

in highp vec3   vec3World;

uniform vec3    vec3Eye;

uniform vec4    vec4Color;
uniform float   fMetallic;
uniform float   fRoughness;

uniform samplerCube texDiff;
uniform samplerCube texSpec;

layout(location = 0) out vec4 vec4Fragment;

void main()
{
    vec3 vec3Color = vec3(vec4Color.r, vec4Color.g, vec4Color.b);
    vec3 vec3N     = normalize(cross(dFdx(vec3World), dFdy(vec3World)));
    vec3 vec3View  = normalize(vec3World - vec3Eye);
    vec3 vec3Ref   = normalize(reflect(vec3View, vec3N));
    
    float fVdotN   = clamp(dot(-vec3View, vec3N), 0, 1.0);
    float fFresnel = fMetallic + (1.0 - fMetallic) * pow(1.0 - fVdotN, 5.0) * (1.0 - fRoughness * 0.9);
    
    vec3 vec3Diff  = textureLod(texDiff, vec3N, 0).xyz * vec3Color;
    vec3 vec3Spec  = textureLod(texSpec, vec3Ref, fRoughness * 6.0).xyz;
    
    vec3 vec3NonM  = vec3Diff + vec3Spec * fFresnel;
    vec3 vec3Metal = vec3Color * vec3Spec;
    float fMix     = smoothstep(0.25, 0.45, fMetallic);
    vec4Fragment   = vec4(pow(mix(vec3NonM, vec3Metal, fMix), GAMMA), vec4Color.a);
}
)FS";

} // namespace PicoGK


