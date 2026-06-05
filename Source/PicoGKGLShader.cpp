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
#include "PicoGKGLShader.h"
#include <vector>

namespace PicoGK
{

//
// class GlShaderProgram
//

GlShaderProgram::GlShaderProgram(   const std::string& strVertShader,
                                    const std::string& strFragShader)
{
    m_hProgram = glCreateProgram();
    
    if (m_hProgram == 0)
    {
        throw ShaderProgramException("Unable to create OpenGL program");
    }
    
    GLuint hVertShader = hCompileShader(EShaderType::Vertex,    strVertShader);
    GLuint hFragShader = hCompileShader(EShaderType::Fragment,  strFragShader);
    
    // CompileShader throws exception if it didn't work, so we have valid shaders now
    
    // Attach and link the shaders into the program
    glAttachShader(m_hProgram, hVertShader);
    glAttachShader(m_hProgram, hFragShader);
    glLinkProgram(m_hProgram);
    
    // After linking, the separate shader objects are
    // no longer needed. The program retains the linked
    // shaders internally
    
    glDetachShader(m_hProgram, hVertShader);
    glDeleteShader(hVertShader);
    glDetachShader(m_hProgram, hFragShader);
    glDeleteShader(hFragShader);
    
    GLint nLinkStatus;
    glGetProgramiv(m_hProgram, GL_LINK_STATUS, &nLinkStatus);
            
    if (!nLinkStatus)
    {
        GLchar pszLog[1024];
        glGetProgramInfoLog(m_hProgram, sizeof(pszLog), nullptr, pszLog);
        
        glDeleteProgram(m_hProgram);
        
        std::string strError = "Error linking shader program ";
        strError += pszLog;
        
        throw ShaderProgramException(strError);
    }
}
    

GlShaderProgram::~GlShaderProgram()
{
    if (m_hProgram != 0)
        glDeleteProgram(m_hProgram);
}
        
    
void GlShaderProgram::Use() const
{
    glUseProgram(m_hProgram);
}

std::string GlShaderProgram::strListUniforms() const
{
    if (m_hProgram == 0)
        return "";
    
    GLint nUniformCount = 0;
    glGetProgramiv(m_hProgram, GL_ACTIVE_UNIFORMS, &nUniformCount);
    
    if (nUniformCount < 1)
    {
        return "Shader program has no uniforms";
    }

    GLint nMaxNameLength = 0;
    glGetProgramiv( m_hProgram,
                    GL_ACTIVE_UNIFORM_MAX_LENGTH,
                    &nMaxNameLength);
    
    std::string str = "Shader Uniforms:\n";
    
    std::vector<GLchar> astrNameBuf(nMaxNameLength);
    for (GLint i = 0; i < nUniformCount; ++i)
    {
        GLenum  eType   = 0;
        GLint   nSize   = 0;
        GLsizei nLength = 0;

        glGetActiveUniform( m_hProgram,
                            i,
                            (GLsizei) astrNameBuf.size(),
                            &nLength,
                            &nSize,
                            &eType,
                            astrNameBuf.data());
        
        GLint nLocation = glGetUniformLocation( m_hProgram,
                                                astrNameBuf.data());
        
        str += "[" + std::to_string(i) + "] - ";
        str += std::string(astrNameBuf.data());
        str += " " + strGlTypeToString(eType);
        str += " Size(" + std::to_string(nSize) + ")";
        str += " Location(" + std::to_string(nLocation) + ")";
        str += "\n";
    }
    
    return str;
}

std::string GlShaderProgram::strListAttributes() const
{
    if (m_hProgram == 0)
        return "";
    
    GLint nAttribCount = 0;
    glGetProgramiv(m_hProgram, GL_ACTIVE_ATTRIBUTES, &nAttribCount);
    
    if (nAttribCount < 1)
    {
        return "Shader program has no attributes";
    }

    GLint nMaxNameLength = 0;
    glGetProgramiv( m_hProgram,
                    GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,
                    &nMaxNameLength);
    
    std::string str = "Shader Attributes:\n";
    
    std::vector<GLchar> astrNameBuf(nMaxNameLength);
    for (GLint i = 0; i < nAttribCount; ++i)
    {
        GLenum  eType   = 0;
        GLint   nSize   = 0;
        GLsizei nLength = 0;

        glGetActiveAttrib(  m_hProgram,
                            i,
                            (GLsizei) astrNameBuf.size(),
                            &nLength,
                            &nSize,
                            &eType,
                            astrNameBuf.data());
        
        GLint nLocation = glGetAttribLocation(  m_hProgram,
                                                astrNameBuf.data());
        
        str += "[" + std::to_string(i) + "] - ";
        str += std::string(astrNameBuf.data());
        str += " " + strGlTypeToString(eType);
        str += " Size(" + std::to_string(nSize) + ")";
        str += " Location(" + std::to_string(nLocation) + ")";
        str += "\n";
    }
    
    return str;
}

/*static*/ std::string GlShaderProgram::strGlTypeToString(GLenum eType)
{
switch (eType)
    {
        case GL_FLOAT:                          return "float";
        case GL_FLOAT_VEC2:                     return "vec2";
        case GL_FLOAT_VEC3:                     return "vec3";
        case GL_FLOAT_VEC4:                     return "vec4";
            
        case GL_DOUBLE:                         return "double";
        
        case GL_INT:                            return "int";
        case GL_INT_VEC2:                       return "ivec2";
        case GL_INT_VEC3:                       return "ivec3";
        case GL_INT_VEC4:                       return "ivec4";

        case GL_BOOL:                           return "bool";
        case GL_BOOL_VEC2:                      return "bvec2";
        case GL_BOOL_VEC3:                      return "bvec3";
        case GL_BOOL_VEC4:                      return "bvec4";

        case GL_FLOAT_MAT2:                     return "mat2";
        case GL_FLOAT_MAT3:                     return "mat3";
        case GL_FLOAT_MAT4:                     return "mat4";
        case GL_FLOAT_MAT2x3:                   return "mat2x3";
        case GL_FLOAT_MAT2x4:                   return "mat2x4";
        case GL_FLOAT_MAT3x2:                   return "mat3x2";
        case GL_FLOAT_MAT3x4:                   return "mat3x4";
        case GL_FLOAT_MAT4x2:                   return "mat4x2";
        case GL_FLOAT_MAT4x3:                   return "mat4x3";

        case GL_SAMPLER_1D:                     return "sampler1D";
        case GL_SAMPLER_2D:                     return "sampler2D";
        case GL_SAMPLER_3D:                     return "sampler3D";
        case GL_SAMPLER_CUBE:                   return "samplerCube";
        case GL_SAMPLER_1D_SHADOW:              return "sampler1DShadow";
        case GL_SAMPLER_2D_SHADOW:              return "sampler2DShadow";

        case GL_SAMPLER_1D_ARRAY:               return "sampler1DArray";
        case GL_SAMPLER_2D_ARRAY:               return "sampler2DArray";
        case GL_SAMPLER_1D_ARRAY_SHADOW:        return "sampler1DArrayShadow";
        case GL_SAMPLER_2D_ARRAY_SHADOW:        return "sampler2DArrayShadow";

        case GL_SAMPLER_2D_MULTISAMPLE:         return "sampler2DMS";
        case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:   return "sampler2DMSArray";

        case GL_SAMPLER_CUBE_SHADOW:            return "samplerCubeShadow";

        case GL_UNSIGNED_INT:                   return "uint";
        case GL_UNSIGNED_INT_VEC2:              return "uvec2";
        case GL_UNSIGNED_INT_VEC3:              return "uvec3";
        case GL_UNSIGNED_INT_VEC4:              return "uvec4";

        /*case GL_IMAGE_2D:                     return "image2D";
        case GL_IMAGE_3D:                       return "image3D";
        case GL_IMAGE_CUBE:                     return "imageCube";
        case GL_IMAGE_2D_ARRAY:                 return "image2DArray";

        case GL_UNSIGNED_INT_IMAGE_2D:          return "uimage2D";
        case GL_UNSIGNED_INT_IMAGE_3D:          return "uimage3D";
        case GL_UNSIGNED_INT_IMAGE_CUBE:        return "uimageCube";
        case GL_UNSIGNED_INT_IMAGE_2D_ARRAY:    return "uimage2DArray";*/
    }
    
    return "Unknown Type(" + std::to_string(eType) + ")";
}

GLuint GlShaderProgram::hCompileShader( EShaderType eType,
                                        const std::string& strSource) const
{
    GLuint hShader = glCreateShader(eType == EShaderType::Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
    
    const char* psz = strSource.c_str();
            
    glShaderSource(hShader, 1, &psz, nullptr);
    glCompileShader(hShader);
    
    GLint nCompiled = 0;
    glGetShaderiv(hShader, GL_COMPILE_STATUS, &nCompiled);
            
    if (nCompiled != GL_TRUE)
    {
        GLchar pszLog[1024];
        glGetShaderInfoLog(hShader, sizeof(pszLog), nullptr, pszLog);
        
        std::string strMessage = "Error compiling ";
        strMessage += (eType == EShaderType::Vertex) ? "Vertex" : "Fragment";
        strMessage += " shader ";
        strMessage += pszLog;
    
        glDeleteShader(hShader);
        
        throw ShaderProgramException(strMessage);
    }
    
    return hShader;
}

GLint GlShaderProgram::nUniformLoc(const std::string& strName) const
{
    GLint nLoc = glGetUniformLocation(m_hProgram, strName.c_str());
    
    if (nLoc == -1)
        throw ShaderProgramException("Unknown parameter (uniform) " + strName);
    
    return nLoc;
}

GLint GlShaderProgram::nAttribLoc(const std::string& strName) const
{
    GLint nLoc = glGetAttribLocation(m_hProgram, strName.c_str());
    
    if (nLoc == -1)
        throw ShaderProgramException("Unknown parameter (attribute) " + strName);
    
    return nLoc;
}
    
} // namespace PicoGK


