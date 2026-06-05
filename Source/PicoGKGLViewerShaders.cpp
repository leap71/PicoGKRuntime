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

#include "gl/glad.h"
#include "PicoGKGLViewerShaders.h"
#include "PicoGKGLTexture.h"
#include <iostream>
#include <numbers>
#include <cmath>

namespace PicoGK
{

//
//
// class ShaderProgMeshPoly
//
//

ShaderProgMeshPoly::ShaderProgMeshPoly()
: GlShaderProgram(c_strVertShader, c_strFragShader)
{
    Init();
}

ShaderProgMeshPoly::ShaderProgMeshPoly(const std::string& strFragmentShader)
: GlShaderProgram(c_strVertShader, strFragmentShader)
{
    Init();
}

void ShaderProgMeshPoly::Init()
{
    m_nUmat4VP                      = nUniformLoc("mat4VP");
    m_nUmat4OtoW                    = nUniformLoc("mat4OtoW");
    m_nUvec3Eye                     = nUniformLoc("vec3Eye");
    m_nUtexSpec                     = nUniformLoc("texSpec");
    m_nUfMetallic                   = nUniformLoc("fMetallic");
    m_nUvec4Color                   = nUniformLoc("vec4Color");
    m_nUfRoughness                  = nUniformLoc("fRoughness");
    m_nUtexDiff                     = nUniformLoc("texDiff");
    m_nUbWarnOverhang               = nUniformLoc("bWarnOverhang");
    m_nUfOverhangWarningDotDown     = nUniformLoc("fOverhangWarningDotDown");
    m_nUfOverhangErrorDotDown       = nUniformLoc("fOverhangErrorDotDown");

    m_nAvec3Pos                     = nAttribLoc("vec3Pos");
}

void ShaderProgMeshPoly::CreateBufferInstance(  const std::vector<Vector3>& vVertices,
                                                std::unique_ptr<GlVertexBuffer<Vector3>>* prResult) const
{
    *prResult = std::make_unique<GlVertexBuffer<Vector3>>(m_nAvec3Pos, vVertices);
}

void ShaderProgMeshPoly::CreateBufferInstance(  const std::vector<Vector3>& vVertices,
                                                const std::vector<Triangle>& vTriangles,
                                                std::unique_ptr<GlElementBuffer<Vector3>>* prResult) const
{
    *prResult = std::make_unique<GlElementBuffer<Vector3>>(m_nAvec3Pos, vVertices, vTriangles);
}

void ShaderProgMeshPoly::Use(   const Matrix4x4&    matVP,
                                const Vector3&      vecEye) const
{
    GlShaderProgram::Use();
    
    glUniform1i(m_nUtexDiff, 0);
    glUniform1i(m_nUtexSpec, 1);
    
    glUniformMatrix4fv( m_nUmat4VP,     1,  GL_FALSE, (GLfloat*) &matVP);
    glUniform3fv(       m_nUvec3Eye,    1,  (GLfloat*) &vecEye);
}

void ShaderProgMeshPoly::SetValues( const Matrix4x4&    mat,
                                    const ColorFloat&   clr,
                                    float               fMetallic,
                                    float               fRoughness,
                                    bool                bWarnOverhang,
                                    float               fWarning,
                                    float               fError) const
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
    
    glUniform1f(m_nUfMetallic,  fMetallic);
    glUniform1f(m_nUfRoughness, fRoughness);
    
    glUniform1i(m_nUbWarnOverhang, bWarnOverhang);
    
    // fWarning and fError are normalized 0..1 values for 0..90º angle
    float fWarningDotDown = std::sin(fWarning * std::numbers::pi_v<float> / 2.0f);
    float fErrorDotDown   = std::sin(fError   * std::numbers::pi_v<float> / 2.0f);

    glUniform1f(m_nUfOverhangWarningDotDown, fWarningDotDown);
    glUniform1f(m_nUfOverhangErrorDotDown,   fErrorDotDown);
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
uniform mat4 mat4VP;

void main()
{
    vec3World   = (mat4OtoW * vec4(vec3Pos, 1)).xyz;
    gl_Position = mat4VP * vec4(vec3World, 1);
}

)VS";

const std::string ShaderProgMeshPoly::c_strFragShader = R"FS(
#version 330 core
//#extension GL_ARB_shader_texture_lod : enable

in highp vec3   vec3World;

uniform vec3    vec3Eye;

uniform vec4    vec4Color;
uniform float   fMetallic;
uniform float   fRoughness;

uniform bool  bWarnOverhang;
uniform float fOverhangWarningDotDown;
uniform float fOverhangErrorDotDown;

uniform samplerCube texDiff;
uniform samplerCube texSpec;

layout(location = 0) out vec4 vec4Fragment;

void main()
{
    vec3 vec3N     = normalize(cross(dFdx(vec3World), dFdy(vec3World)));
    vec3 vec3View  = normalize(vec3World - vec3Eye);
    vec3 vec3Ref   = normalize(reflect(vec3View, vec3N));

    vec3 vec3Color = vec3(vec4Color.r, vec4Color.g, vec4Color.b);

    if (bWarnOverhang)
    {
        vec3 vec3Down = vec3(0.0, 0.0, -1.0);

        // dot(normal, down) is:
        // -1: upward-facing horizontal surface
        //  0: vertical wall
        //  1: downward-facing horizontal overhang
        float fDotDown = dot(vec3N, vec3Down);

        if (fDotDown >= fOverhangWarningDotDown)
        {
            float fSeverity = smoothstep(
                fOverhangWarningDotDown,
                fOverhangErrorDotDown,
                fDotDown);

            // Warning threshold: orange
            // Error threshold and beyond: red
            vec3Color = mix(
                vec3(1.0, 0.5, 0.0),
                vec3(1.0, 0.0, 0.0),
                fSeverity);

            vec4Fragment = vec4(vec3Color, 0.9);
            return;
        }
        else
        {
            vec3Color = vec3(0.0, 1.0, 0.0);
        }
    }
    
    float fVdotN   = clamp(dot(-vec3View, vec3N), 0.0, 1.0);
    float fFresnel = fMetallic + (1.0 - fMetallic) * pow(1.0 - fVdotN, 5.0) * (1.0 - fRoughness * 0.9);
    
    vec3 vec3Diff  = textureLod(texDiff, vec3N, 0).xyz * vec3Color;
    vec3 vec3Spec  = textureLod(texSpec, vec3Ref, fRoughness * 6.0).xyz;
    
    vec3 vec3NonM  = vec3Diff + vec3Spec * fFresnel;
    vec3 vec3Metal = vec3Color * vec3Spec;
    float fMix     = smoothstep(0.25, 0.45, fMetallic);

    vec4Fragment   = vec4(mix(vec3NonM, vec3Metal, fMix) * vec4Color.a, vec4Color.a);
}
)FS";

//
//
// class ShaderProgMeshPolyOit
//
//

ShaderProgMeshPolyOit::ShaderProgMeshPolyOit()
: ShaderProgMeshPoly(c_strFragShaderOit)
{
    
}

const std::string ShaderProgMeshPolyOit::c_strFragShaderOit = R"FS(
#version 410 core

in highp vec3   vec3World;

uniform vec3    vec3Eye;
uniform vec4    vec4Color;
uniform float   fMetallic;
uniform float   fRoughness;

uniform bool    bWarnOverhang;
uniform float   fOverhangWarningDotDown;
uniform float   fOverhangErrorDotDown;

uniform samplerCube texDiff;
uniform samplerCube texSpec;

layout(location = 0) out vec4   vec4OutAccum;
layout(location = 1) out float  fOutReveal;

void main()
{
    vec3 vec3N     = normalize(cross(dFdx(vec3World), dFdy(vec3World)));
    vec3 vec3View  = normalize(vec3World - vec3Eye);
    vec3 vec3Ref   = normalize(reflect(vec3View, vec3N));
    vec3 vec3Color = vec3(vec4Color.r, vec4Color.g, vec4Color.b);

    if (bWarnOverhang)
    {
        vec3 vec3Down = vec3(0.0, 0.0, -1.0);

        // dot(normal, down) is:
        // -1: upward-facing horizontal surface
        //  0: vertical wall
        //  1: downward-facing horizontal overhang
        float fDotDown = dot(vec3N, vec3Down);

        if (fDotDown >= fOverhangWarningDotDown)
        {
            float fSeverity = smoothstep(
                fOverhangWarningDotDown,
                fOverhangErrorDotDown,
                fDotDown);

            vec3Color = mix(
                vec3(1.0, 0.5, 0.0),
                vec3(1.0, 0.0, 0.0),
                fSeverity);

            float a = 0.9;
            float z = gl_FragCoord.z;
            float w = clamp(0.03 / (1e-5 + pow(z, 7.0)), 1e-2, 3e3);

            vec4OutAccum = vec4(vec3Color * a * w, a * w);
            fOutReveal   = a;
            return;
        }
        else
        {
            vec3Color = vec3(0.0, 1.0, 0.0);
        }
    }

    // Lighting logic from standard shader
    
    float fVdotN   = clamp(dot(-vec3View, vec3N), 0.0, 1.0);
    float fFresnel = fMetallic + (1.0 - fMetallic) * pow(1.0 - fVdotN, 5.0) * (1.0 - fRoughness * 0.9);

    vec3 vec3Diff  = textureLod(texDiff, vec3N, 0).xyz * vec3Color;
    vec3 vec3Spec  = textureLod(texSpec, vec3Ref, fRoughness * 6.0).xyz;

    vec3 vec3NonM  = vec3Diff + vec3Spec * fFresnel;
    vec3 vec3Metal = vec3Color * vec3Spec;
    float fMix     = smoothstep(0.25, 0.45, fMetallic);

    vec3 rgb = mix(vec3NonM, vec3Metal, fMix);
    float a = vec4Color.a;

    // OIT weighted blend output

    float z = gl_FragCoord.z;
    float w = clamp(0.03 / (1e-5 + pow(z, 7.0)), 1e-2, 3e3);

    vec4OutAccum    = vec4(rgb * a * w, a * w);
    fOutReveal      = a;
}
)FS";


//
//
// class ShaderProgOitComposite : GlShaderProgram
//
//

ShaderProgOitComposite::ShaderProgOitComposite()
: GlShaderProgram(c_strVertShader, c_strFragShader)
{
    m_nUtexAccum        = nUniformLoc("texAccum");
    m_nUtexReveal       = nUniformLoc("texReveal");
    
    // Set up fullscreen quad VAO and VBO
    struct SVertex
    {
        float x, y, u, v;
    };

    // Triangle strip order: bottom-left -> bottom-right -> top-left -> top-right
    const SVertex aVertices[4] =
    {
        { -1.0f, -1.0f, 0.0f, 0.0f },  // Bottom-left
        {  1.0f, -1.0f, 1.0f, 0.0f },  // Bottom-right
        { -1.0f,  1.0f, 0.0f, 1.0f },  // Top-left
        {  1.0f,  1.0f, 1.0f, 1.0f }   // Top-right
    };

    glGenVertexArrays(1, &m_nVAO);
    glBindVertexArray(m_nVAO);

    glGenBuffers(1, &m_nVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(aVertices), aVertices, GL_STATIC_DRAW);

    // Position attribute (location 0: vec2)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)offsetof(SVertex, x));

    // UV attribute (location 1: vec2)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)offsetof(SVertex, u));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

ShaderProgOitComposite::~ShaderProgOitComposite()
{
    if (m_nVBO)
        glDeleteBuffers(1, &m_nVBO);

    if (m_nVAO)
        glDeleteVertexArrays(1, &m_nVAO);
}

void ShaderProgOitComposite::Use(   GLuint nAccum,
                                    GLuint nReveal)
{
    GlShaderProgram::Use();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, nAccum);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, nReveal);
    
    // Bind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, nAccum);
    glUniform1i(m_nUtexAccum, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, nReveal);
    glUniform1i(m_nUtexReveal, 1);

    // Draw fullscreen quad
    glBindVertexArray(m_nVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    // Cleanup texture bindings
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
    
const std::string ShaderProgOitComposite::c_strVertShader = R"VS(
#version 410 core

layout(location = 0) in vec2 vec2InPos;
layout(location = 1) in vec2 vec2InUV;

out vec2 vec2UV;

void main()
{
    vec2UV = vec2InUV;
    gl_Position = vec4(vec2InPos, 0.0, 1.0);
}
)VS";

const std::string ShaderProgOitComposite::c_strFragShader = R"FS(
#version 410 core

in vec2     vec2UV;
out vec4    vec4FragColor;

uniform vec3 rgbBackground;

uniform sampler2D texAccum;
uniform sampler2D texReveal;

void main()
{
    vec4 vec4AccumColor = texture(texAccum, vec2UV);
    float fReveal       = texture(texReveal, vec2UV).r;

    vec3 rgb = (vec4AccumColor.a > 1e-6) ? (vec4AccumColor.rgb / vec4AccumColor.a) : vec3(0.0);

    float fAlpha = 1.0 - fReveal;

    vec4FragColor = vec4(rgb, fAlpha);
}
)FS";

//
//
// class ShaderProgQuad
//
//


ShaderProgQuad::ShaderProgQuad()
: GlShaderProgram(c_strVertShader, c_strFragShader)
{
    m_nUmat4MVP         = nUniformLoc("mat4MVP");
    m_nUbRenderSolid    = nUniformLoc("bRenderSolid");
    m_nUvec4SolidColor  = nUniformLoc("vec4SolidColor");
    m_nUtexTexture      = nUniformLoc("texTexture");
    m_nUfAlpha          = nUniformLoc("fAlpha");
    m_nUbFlipX          = nUniformLoc("bFlipX");
    m_nUbFlipY          = nUniformLoc("bFlipY");

    m_nAvec3Pos           = nAttribLoc("vec3Pos");
    m_nAvec2InUV          = nAttribLoc("vec2InUV");
    
    // Build reusable quad with UV coords
    
    struct SVertex
    {
        float x,y,z,u,v;
    };

    // Triangle strip order:
    // v0 (-0.5,-0.5) → v1 (0.5,-0.5) → v2 (-0.5,0.5) → v3 (0.5,0.5)
    const SVertex aVertices[4] =
    {
        { -0.5f, -0.5f, 0.0f, 0.0f, 0.0f },  // v0
        {  0.5f, -0.5f, 0.0f, 1.0f, 0.0f },  // v1
        { -0.5f,  0.5f, 0.0f, 0.0f, 1.0f },  // v2
        {  0.5f,  0.5f, 0.0f, 1.0f, 1.0f },  // v3
    };
        
    glGenVertexArrays(1, &m_nVAO);
    glBindVertexArray(m_nVAO);

    glGenBuffers(1, &m_nVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
        
    glBufferData(GL_ARRAY_BUFFER, sizeof(aVertices), aVertices, GL_STATIC_DRAW);

    // Attribute locations are fixed by layout qualifiers (0:pos, 1:uv)
    glEnableVertexAttribArray(m_nAvec3Pos);
    glVertexAttribPointer(m_nAvec3Pos, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)offsetof(SVertex,x));

    glEnableVertexAttribArray(m_nAvec2InUV);
    glVertexAttribPointer(m_nAvec2InUV, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)offsetof(SVertex,u));

    glBindVertexArray(0);
}

void ShaderProgQuad::DrawQuad(  const Matrix4x4& matMVP,
                                bool bRenderSolid,
                                ColorFloat clrSolid,
                                float fAlpha,
                                GLuint hTexture,
                                bool bFlipX,
                                bool bFlipY,
                                bool bDoubleSided) const
{
    glUniformMatrix4fv(m_nUmat4MVP, 1, GL_FALSE, (GLfloat*) &matMVP);
    glUniform1i(m_nUbRenderSolid, bRenderSolid ? 1 : 0);
    glUniform4f(m_nUvec4SolidColor, clrSolid.R, clrSolid.G, clrSolid.B, clrSolid.A);
    glUniform1f(m_nUfAlpha, fAlpha);
    glUniform1i(m_nUbFlipX, bFlipX ? 1 : 0);
    glUniform1i(m_nUbFlipY, bFlipY ? 1 : 0);
    
    if (bDoubleSided)
        glDisable(GL_CULL_FACE);
    else
        glEnable(GL_CULL_FACE);

    if (hTexture != 0)
    {
        // Bind texture to texture unit 2 to avoid collisions with viewer textures
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, hTexture);
        glUniform1i(m_nUtexTexture, 2); // Tell shader that texTexture = texture unit 2
    }
    
    glBindVertexArray(m_nVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

/*static*/ const std::string ShaderProgQuad::c_strVertShader = R"VS(
#version 330 core

layout(location = 0) in vec3 vec3Pos;
layout(location = 1) in vec2 vec2InUV;

uniform mat4 mat4MVP;
uniform bool bFlipX;
uniform bool bFlipY;

out vec2 vec2UV;

void main()
{
    vec2UV.x = bFlipX ? (1.0 - vec2InUV.x) : vec2InUV.x;
    vec2UV.y = bFlipY ? (1.0 - vec2InUV.y) : vec2InUV.y;

    gl_Position = mat4MVP * vec4(vec3Pos, 1.0);
}
)VS";

/*static*/ const std::string ShaderProgQuad::c_strFragShader = R"FS(
#version 330 core

in vec2     vec2UV;
out vec4    vec4FragColor;

uniform float   fAlpha;
uniform bool    bRenderSolid;
uniform vec4    vec4SolidColor;

uniform sampler2D   texTexture;

void main()
{
    if (bRenderSolid)
    {
        vec4FragColor = vec4SolidColor;
        return;
    }

    vec4 vec4Color  = texture(texTexture, vec2UV);
    vec4Color.a     *= fAlpha;
    vec4FragColor   = vec4Color * vec4Color.a;
}

)FS";

} // namespace PicoGK


