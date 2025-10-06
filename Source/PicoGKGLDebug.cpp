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

#include "gl/glad.h"
#include "PicoGKGLDebug.h"
#include "PicoGKGLViewer.h"
#include "PicoGKGLViewerManager.h"

namespace PicoGK
{

bool bCheckGLErrors()
{
    uint32_t nInvalidOps = 0;

    GLenum e = glGetError();

    bool bResult = true;

    while (e != GL_NO_ERROR)
    {
        bResult = false;

        switch (e)
        {
        case GL_INVALID_ENUM:
            ViewerManager::Info("OpenGL Error: Invalid Enum");
            break;
        case GL_INVALID_VALUE:
            ViewerManager::Info("OpenGL Error: Invalid Value");
            break;
        case GL_INVALID_INDEX:
            ViewerManager::Info("OpenGL Error: Invalid Index");
            break;
        case GL_INVALID_OPERATION:
            ViewerManager::Info("OpenGL Error: Invalid Operation");
            nInvalidOps++;
            break;
        case GL_OUT_OF_MEMORY:
            ViewerManager::Info("OpenGL Error: Out of Memory");
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            ViewerManager::Info("OpenGL Error: Invalid Framebuffer Operation");
            break;
        default:
            ViewerManager::Info("OpenGL Error: Unknown Error");
        }
        
        if (nInvalidOps > 10)
        {
            ViewerManager::Info("OpenGL Error: more than 10 invalid operations, probably called at a wrong time");
            break;
        }
        
        e = glGetError();
    }

    return bResult;
}

#define CHECKGLERRORS assert(bCheckGLErrors())

bool bCheckShaderErrors(GLuint nShader)
{
    GLint iCompileRes;
    glGetShaderiv(nShader, GL_COMPILE_STATUS, &iCompileRes);
    
    if (iCompileRes != GL_TRUE)
    {
        GLsizei nLogLength;
        glGetShaderiv(nShader, GL_INFO_LOG_LENGTH, &nLogLength);

        std::vector<GLchar> oLog(nLogLength + 1);
    
        glGetShaderInfoLog(nShader, nLogLength + 1, nullptr, oLog.data());

        ViewerManager::Info("Shader compilation error:\n" + std::string(oLog.data()));
        return false;
    }

    return true;
}

#define CHECKSHADERERRORS(nShader)    assert(bCheckShaderErrors(nShader));

}
