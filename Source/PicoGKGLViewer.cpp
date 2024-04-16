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

#include "gl/gl.h"
#include "PicoGKGLViewer.h"
#include "PicoGKGLTexture.h"

#include <GLFW/glfw3.h>

#include <fstream>

namespace PicoGK
{

namespace
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

static void ErrorCallback(int iError, const char* pszDescription)
{
    ViewerManager::Info("GL Callback Error: " + std::string(pszDescription));
}

}; // anonymous namespace


Viewer::Viewer( GLFWwindow*             pTheWindow,
                PKPFUpdateRequested     pfnUpdateCallback,
                PKPFKeyPressed          pfnKeyPressedCallback,
                PKPFMouseMoved          pfnMouseMoveCallback,
                PKPFMouseButton         pfnMouseButtonCallback,
                PKPFScrollWheel         pfnScrollWheelCallback,
                PKPFWindowSize          pfnWindowSizeCallback)

:   PKINIT(pTheWindow),
    PKINIT(pfnUpdateCallback),
    PKINIT(pfnKeyPressedCallback),
    PKINIT(pfnMouseMoveCallback),
    PKINIT(pfnMouseButtonCallback),
    PKINIT(pfnScrollWheelCallback),
    PKINIT(pfnWindowSizeCallback)
{
    m_vecMousePos.X = 0.0f;
    m_vecMousePos.Y = 0.0f;
    
    glfwMakeContextCurrent(m_pTheWindow);
    gladLoadGL(glfwGetProcAddress);
    
    CHECKGLERRORS;

    const char* pszVS = m_strVertexShader.c_str();

    m_sConfig.nVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(     m_sConfig.nVertexShader, 1, &pszVS, NULL);
    glCompileShader(    m_sConfig.nVertexShader);
 
    CHECKSHADERERRORS(m_sConfig.nVertexShader)
    
    const char* pszFS = m_strFragmentShader.c_str();
        
    m_sConfig.nFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(     m_sConfig.nFragmentShader, 1, &pszFS, NULL);
    glCompileShader(    m_sConfig.nFragmentShader);

    CHECKSHADERERRORS(m_sConfig.nFragmentShader)

    m_sConfig.nProgram = glCreateProgram();
    glAttachShader( m_sConfig.nProgram, m_sConfig.nVertexShader);
    glAttachShader( m_sConfig.nProgram, m_sConfig.nFragmentShader);
    glLinkProgram(  m_sConfig.nProgram);
    
    CHECKGLERRORS;
     
    m_sConfig.iOtoWUniform          = glGetUniformLocation(m_sConfig.nProgram, "mat4OtoW");
    m_sConfig.iMVPUniform           = glGetUniformLocation(m_sConfig.nProgram, "mat4MVP");
    m_sConfig.iEyeUniform           = glGetUniformLocation(m_sConfig.nProgram, "vec3Eye");
    m_sConfig.iColorUniform         = glGetUniformLocation(m_sConfig.nProgram, "vec4Color");
    m_sConfig.iMetallicUniform      = glGetUniformLocation(m_sConfig.nProgram, "fMetallic");
    m_sConfig.iRoughnessUniform     = glGetUniformLocation(m_sConfig.nProgram, "fRoughness");
    m_sConfig.iDiffuseUniform       = glGetUniformLocation(m_sConfig.nProgram, "texDiff");
    m_sConfig.iSpecularUniform      = glGetUniformLocation(m_sConfig.nProgram, "texSpec");

    m_sConfig.iPosAttrib            = glGetAttribLocation(m_sConfig.nProgram, "vec3Pos");
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    CHECKGLERRORS;
}


bool Viewer::bLoadLightSetup(   const char* pDiffuseTextureDDS,
                                uint32_t nDiffuseBufferSize,
                                const char* pSpecularTextureDDS,
                                uint32_t nSpecularBufferSize)
{
    ViewerManager::Info("Loading Lights");
    
    glfwMakeContextCurrent(m_pTheWindow);
    
    // Texture 0 is the diffuse cube map for lighting
    
    glGenTextures(1, &m_sConfig.nTexDiffuse);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_sConfig.nTexDiffuse);

    if (!bLoadDdsTexture(pDiffuseTextureDDS, nDiffuseBufferSize, GL_TEXTURE_CUBE_MAP))
    {
        ViewerManager::Info("Failed to load diffuse texture dds");
        return false;
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    // Texture 1 is the specular cube map for lighting

    glGenTextures(1, &m_sConfig.nTexSpecular);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_sConfig.nTexSpecular);

    if (!bLoadDdsTexture(pSpecularTextureDDS, nSpecularBufferSize, GL_TEXTURE_CUBE_MAP))
    {
        ViewerManager::Info("Failed to load specular texture dds");
        return false;
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    CHECKGLERRORS;
    
    RequestUpdate();
    
    return true;
}

Viewer::~Viewer()
{
}

namespace
{

void SaveTGA(   const std::string strPath,
                const std::vector<unsigned char>& oImage,
                int iWidth,
                int iHeight)
{
    std::ofstream oFile(strPath, std::ios::out | std::ios::binary);
    
    // TGA header
    unsigned char header[18] = {0};
    header[2]   = 2; // Uncompressed RGB image type
    header[12]  = iWidth & 0xFF;
    header[13]  = (iWidth >> 8) & 0xFF;
    header[14]  = iHeight & 0xFF;
    header[15]  = (iHeight >> 8) & 0xFF;
    header[16]  = 24; // 24 bits per pixel
    
    oFile.write(reinterpret_cast<char*>(header), 18);
    oFile.write(reinterpret_cast<const char*>(oImage.data()), oImage.size());
    
    oFile.close();
}

}

bool Viewer::bPoll()
{
    glfwPollEvents();
    bool bContinue = !glfwWindowShouldClose(m_pTheWindow);
    
    if (!m_bRedrawNeeded)
        return bContinue;
    
    m_bRedrawNeeded = false;
    
    try
    {
        glfwMakeContextCurrent(m_pTheWindow);
        
        //get framebuffer size in device pixels
        int iWidth, iHeight;
        glfwGetFramebufferSize(m_pTheWindow, &iWidth, &iHeight);
        glViewport(0, 0, iWidth, iHeight);
        
        ColorFloat clrBackground;
        clrBackground.R = 1.0f;
        clrBackground.G = 0.0f;
        clrBackground.B = 0.0f;
        clrBackground.A = 0.0f;
        
        Matrix4x4 matMVP;
        Matrix4x4 matModelTrans;
        Vector3   vecEye(0,0,0);
        Matrix4x4 matStatic;
        Vector3   vecEyeStatic(0,0,0);
        
        if (m_pfnUpdateCallback != nullptr)
        {
            Vector2 vecViewSize;
            vecViewSize.X = (float) iWidth;
            vecViewSize.Y = (float) iHeight;
            
            m_pfnUpdateCallback(    this,
                                    &vecViewSize,
                                    &clrBackground,
                                    &matMVP,
                                    &matModelTrans,
                                    &matStatic,
                                    &vecEye,
                                    &vecEyeStatic);
        }
        
        glClearColor(   clrBackground.R,
                        clrBackground.G,
                        clrBackground.B,
                        clrBackground.A);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glUseProgram(m_sConfig.nProgram);
        
        glUniformMatrix4fv( m_sConfig.iMVPUniform,  1,  GL_FALSE, (GLfloat*) &matMVP);
        glUniform3fv(       m_sConfig.iEyeUniform,  1,  (GLfloat*) &vecEye);
        
        if (m_sConfig.nTexDiffuse != 0)
        {
            // We have a light setup
            glUniform1i(m_sConfig.iDiffuseUniform,  0);
            glUniform1i(m_sConfig.iSpecularUniform, 1);
        }
        
        CHECKGLERRORS;
        
        // Draw the not-static stuff
        
        for (auto Pair : m_oGroups)
        {
            Group::Ptr poGroup = Pair.second;
            if (!poGroup->bStatic())
            {
                poGroup->Draw(matModelTrans, m_sConfig);
            }
        }
        
        // Now draw the static stuff
        
        glUniformMatrix4fv( m_sConfig.iMVPUniform,  1,  GL_FALSE, (GLfloat*) &matStatic);
        glUniform3fv(       m_sConfig.iEyeUniform,  1,  (GLfloat*) &vecEyeStatic);
        
        for (auto Pair : m_oGroups)
        {
            Group::Ptr poGroup = Pair.second;
            if (poGroup->bStatic())
            {
                poGroup->Draw(matModelTrans, m_sConfig);
            }
        }
        
        if (m_strScreenShotPath != "")
        {
            std::vector<unsigned char> image(iWidth * iHeight * 3); // 3 bytes per pixel (RGB)
            glReadPixels(0, 0, iWidth, iHeight, GL_BGR, GL_UNSIGNED_BYTE, image.data());
            
            // Save the image as a TGA file
            SaveTGA(m_strScreenShotPath, image, iWidth, iHeight);
            m_strScreenShotPath = "";
        }
        
        glfwSwapBuffers(m_pTheWindow);
    }
    
    catch (...)
    {
    }
    
    return bContinue;
}

void Viewer::RequestScreenShot(const std::string &strScreenShotPath)
{
    m_strScreenShotPath = strScreenShotPath;
    RequestUpdate();
}

void Viewer::RequestClose()
{
    glfwSetWindowShouldClose(m_pTheWindow, GLFW_TRUE);
}

void Viewer::AddMesh(   int32_t             nGroupID,
                        const Mesh::Ptr*    proMesh)
{
    roGroupAt(nGroupID)->AddMesh(proMesh);
    RequestUpdate();
}

void Viewer::RemoveMesh(const Mesh::Ptr* proMesh)
{
    for (auto Pair : m_oGroups)
    {
        Group::Ptr poGroup = Pair.second;
        if (poGroup->bFindMesh(proMesh))
        {
            poGroup->RemoveMesh(proMesh);
            return;
        }
    }
    
    ViewerManager::Info("Viewer::RemoveMesh - Trying to remove a mesh that doesn't exist.");
}

void Viewer::AddPolyLine(   int32_t                 nGroupID,
                            const PolyLine::Ptr*    proPoly)
{
    roGroupAt(nGroupID)->AddPolyLine(proPoly);
    RequestUpdate();
}

void Viewer::RemovePolyLine(const PolyLine::Ptr* proPolyLine)
{
    for (auto Pair : m_oGroups)
    {
        Group::Ptr poGroup = Pair.second;
        if (poGroup->bFindPolyLine(proPolyLine))
        {
            poGroup->RemovePolyLine(proPolyLine);
            return;
        }
    }
    
    ViewerManager::Info("Viewer::RemovePolyLine - Trying to remove a polyline that doesn't exist.");
}

void Viewer::SetGroupVisible(   int32_t     nGroupID,
                                bool        bVisible)
{
    roGroupAt(nGroupID)->SetVisible(bVisible);
    RequestUpdate();
}

void Viewer::SetGroupStatic(    int32_t     nGroupID,
                                bool        bStatic)
{
    roGroupAt(nGroupID)->SetStatic(bStatic);
    RequestUpdate();
}

void Viewer::SetGroupMaterial(  int32_t     nGroupID,
                                ColorFloat  clr,
                                float       fMetallic,
                                float       fRoughness)
{
    roGroupAt(nGroupID)->SetMaterial(   clr,
                                        fMetallic,
                                        fRoughness);
}

void Viewer::SetGroupMatrix(    int32_t             nGroupID,
                                const Matrix4x4&    mat)
{
    roGroupAt(nGroupID)->SetMatrix(mat);
}

Viewer::Group::ViewPolyLine::ViewPolyLine(const PicoGK::PolyLine::Ptr& roPoly)
{
    m_roPolyLine = roPoly;
    
    if (m_roPolyLine->nVertexCount() == 0)
        return;
    
    glGenVertexArrays(1, &sGLParams.nVertexArray);
    glBindVertexArray(sGLParams.nVertexArray);

    glGenBuffers(1, &sGLParams.nArrayBuffer);

    glBindBuffer(   GL_ARRAY_BUFFER,
                    sGLParams.nArrayBuffer);
        
    glBufferData(   GL_ARRAY_BUFFER,
                    m_roPolyLine->nVertexCount() * sizeof(Vector3),
                    m_roPolyLine->pVertexData(),
                    GL_STATIC_DRAW);
    
    CHECKGLERRORS;
}

void Viewer::Group::ViewPolyLine::Draw( const ShaderConfig& sConfig,
                                        const Material& oMaterial,
                                        const Matrix4x4& mat)
{
    if (m_roPolyLine->nVertexCount() == 0)
        return;
        
    glUniformMatrix4fv( sConfig.iOtoWUniform, 1, GL_FALSE,
                        (GLfloat*) &mat);
    
    ColorFloat clr = m_roPolyLine->clrLines();
    
    glUniform4f(    sConfig.iColorUniform,
                    clr.R,
                    clr.G,
                    clr.B,
                    clr.A);
    
    // Draw the polyline
    glBindVertexArray(sGLParams.nVertexArray);
            
    glBindBuffer(   GL_ARRAY_BUFFER,
                    sGLParams.nArrayBuffer);
    
    // Specify the attribute location and format
    glEnableVertexAttribArray(sConfig.iPosAttrib);
    glVertexAttribPointer(sConfig.iPosAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), nullptr);
    
    glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(m_roPolyLine->nVertexCount()));
    glBindVertexArray(0);
    
    CHECKGLERRORS;
}

void Viewer::Group::Draw(   const Matrix4x4& matModelTrans,
                            const ShaderConfig& sConfig)
{
    if (!m_bVisible)
        return;
    
    Matrix4x4 matMult = matModelTrans;
    matMult *= m_mat;
    
    for (auto o : m_oViewPolyLines)
    {
        ViewPolyLine::Ptr roLine = o.second;
        roLine->Draw(sConfig, m_sMaterial, matMult);
    }
    
    for (auto o : m_oViewMeshes)
    {
        ViewMesh::Ptr roMesh = o.second;
        roMesh->Draw(sConfig, m_sMaterial, matMult);
    }
}

Viewer::Group::ViewMesh::ViewMesh(const Mesh::Ptr& roMesh)
{
    m_roMesh = roMesh;
    glGenVertexArrays(1, &sGLParams.nVertexArray);
    glBindVertexArray(sGLParams.nVertexArray);

    glGenBuffers(1, &sGLParams.nArrayBuffer);

    glBindBuffer(   GL_ARRAY_BUFFER,
                    sGLParams.nArrayBuffer);
        
    glBufferData(   GL_ARRAY_BUFFER,
                    m_roMesh->nVertexCount() * sizeof(Vector3),
                    m_roMesh->pVertexData(),
                    GL_STATIC_DRAW);
    
    glGenBuffers(1, &sGLParams.nElementArrayBuffer);
    
    glBindBuffer(   GL_ELEMENT_ARRAY_BUFFER,
                    sGLParams.nElementArrayBuffer);
        
    glBufferData(   GL_ELEMENT_ARRAY_BUFFER,
                    m_roMesh->nTriangleCount() * sizeof(Triangle),
                    m_roMesh->pTriangleData(),
                    GL_STATIC_DRAW);
    
    CHECKGLERRORS;
}

void Viewer::Group::ViewMesh::Draw( const ShaderConfig& sConfig,
                                    const Material& sMaterial,
                                    const Matrix4x4& mat)
{
    glUniformMatrix4fv( sConfig.iOtoWUniform, 1, GL_FALSE,
                        (GLfloat*) &mat);
    
    glUniform4f(    sConfig.iColorUniform,
                    sMaterial.clr.R,
                    sMaterial.clr.G,
                    sMaterial.clr.B,
                    sMaterial.clr.A);
    
    glUniform1f(sConfig.iMetallicUniform,   sMaterial.fMetallic);
    glUniform1f(sConfig.iRoughnessUniform,  sMaterial.fRoughness);

    CHECKGLERRORS;
                
    glBindVertexArray(sGLParams.nVertexArray);
            
    glBindBuffer(   GL_ARRAY_BUFFER,
                    sGLParams.nArrayBuffer);
    
    glEnableVertexAttribArray( sConfig.iPosAttrib);
    
    glVertexAttribPointer(  sConfig.iPosAttrib,
                            3, GL_FLOAT, GL_FALSE,
                            sizeof(Vector3),
                            nullptr);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sGLParams.nElementArrayBuffer);
    glDrawElements(GL_TRIANGLES, m_roMesh->nTriangleCount() * 3, GL_UNSIGNED_INT, nullptr);

    CHECKGLERRORS;
}

ViewerManager::ViewerManager()
{
    m_pfnInfoCallback = nullptr;
    
    if (!glfwInit())
    {
        fprintf(stderr, "GLFW initialization failed\n");
        return;
    }
}

ViewerManager::~ViewerManager()
{
    glfwTerminate();
}

Viewer* ViewerManager::poCreate(    const std::string&  strWindowTitle,
                                    const Vector2&      vecSize,
                                    PKFInfo             pfnInfoCallback,
                                    PKPFUpdateRequested pfnUpdateCallback,
                                    PKPFKeyPressed      pfnKeyPressedCallback,
                                    PKPFMouseMoved      pfnMouseMoveCallback,
                                    PKPFMouseButton     pfnMouseButtonCallback,
                                    PKPFScrollWheel     pfnScrollWheelCallback,
                                    PKPFWindowSize      pfnWindowSizeCallback)
{
    m_pfnInfoCallback = pfnInfoCallback;
    
    glfwSetErrorCallback(ErrorCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            //required for Mac OS
    //glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
    //glfwWindowHint(GLFW_SAMPLES, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    
    GLFWwindow* pWindow = glfwCreateWindow( (GLint) vecSize.X,
                                            (GLint) vecSize.Y,
                                            strWindowTitle.c_str(),
                                            NULL,  // which monitor
                                            NULL); // "share" ??

    if (pWindow == nullptr)
    {
        ViewerManager::Info("Failed to get OpenGL window", true);
        return nullptr;
    }
    
    glfwSetKeyCallback(         pWindow, KeyPressed);
    glfwSetCursorPosCallback(   pWindow, MouseMoved);
    glfwSetMouseButtonCallback( pWindow, MouseButton);
    glfwSetScrollCallback(      pWindow, ScrollWheel);
    glfwSetWindowSizeCallback(  pWindow, WindowSize);
    
    Viewer* poViewer = new Viewer(  pWindow,
                                    pfnUpdateCallback,
                                    pfnKeyPressedCallback,
                                    pfnMouseMoveCallback,
                                    pfnMouseButtonCallback,
                                    pfnScrollWheelCallback,
                                    pfnWindowSizeCallback);

    m_oViewers[pWindow] = poViewer;
    
    poViewer->RequestUpdate();
    return poViewer;
}

void ViewerManager::Destroy(Viewer* poViewer)
{
    for (auto Entry : m_oViewers)
    {
        if (Entry.second == poViewer)
        {
            m_oViewers.erase(Entry.first);
            delete poViewer;
            return;
        }
    }
}

bool ViewerManager::bIsValid(const Viewer* poViewer) const
{
    if (poViewer == nullptr)
        return false;
    
    return bExists(poViewer);
}

bool ViewerManager::bExists(const Viewer* poViewer) const
{
    for (auto Entry : m_oViewers)
    {
        if (Entry.second == poViewer)
            return true;
    }
    
    return false;
}

void ViewerManager::ReportInfo(const std::string strInfo, bool bFatal)
{
    if (m_pfnInfoCallback != nullptr)
        m_pfnInfoCallback(strInfo.c_str(), bFatal);
}

void ViewerManager::KeyPressed(  GLFWwindow* pWindow,
                                 int iKey,
                                 int iScanCode,
                                 int iAction,
                                 int iModifiers)
{
    Viewer* poViewer = oMgr().m_oViewers[pWindow];
    assert(poViewer != nullptr);
    
    if (poViewer->m_pfnKeyPressedCallback != nullptr)
    {
        poViewer->m_pfnKeyPressedCallback(  poViewer,
                                            iKey,
                                            iScanCode,
                                            iAction,
                                            iModifiers);
    }
}

void ViewerManager::MouseMoved(  GLFWwindow* pWindow,
                                 double dMouseX,
                                 double dMouseY)
{
    Viewer* poViewer = oMgr().m_oViewers[pWindow];
    assert(poViewer != nullptr);
    
    poViewer->m_vecMousePos.X = (float) dMouseX;
    poViewer->m_vecMousePos.Y = (float) dMouseY;
    
    if (poViewer->m_pfnMouseMoveCallback != nullptr)
    {
        poViewer->m_pfnMouseMoveCallback(   poViewer,
                                            &poViewer->m_vecMousePos);
    }
}

void ViewerManager::MouseButton( GLFWwindow* pWindow,
                                 int iButton,
                                 int iAction,
                                 int iModifiers)
{
    Viewer* poViewer = oMgr().m_oViewers[pWindow];
    assert(poViewer != nullptr);
    
    if (poViewer->m_pfnMouseButtonCallback != nullptr)
    {
        poViewer->m_pfnMouseButtonCallback(    poViewer,
                                               iButton,
                                               iAction,
                                               iModifiers,
                                               &poViewer->m_vecMousePos);
        
    }
}

void ViewerManager::ScrollWheel(    GLFWwindow* pWindow,
                                    double dX,
                                    double dY)
{
    Viewer* poViewer = oMgr().m_oViewers[pWindow];
    assert(poViewer != nullptr);

    if (poViewer->m_pfnScrollWheelCallback != nullptr)
    {
        Vector2 vec;
        vec.X = dX;
        vec.Y = dY;
        poViewer->m_pfnScrollWheelCallback( poViewer,
                                            &vec,
                                            &poViewer->m_vecMousePos);
    }
}

void ViewerManager::WindowSize(    GLFWwindow* pWindow,
                                   int nWidth,
                                   int nHeight)
{
    Viewer* poViewer = oMgr().m_oViewers[pWindow];
    assert(poViewer != nullptr);

    if (poViewer->m_pfnWindowSizeCallback != nullptr)
    {
        Vector2 vec;
        vec.X = nWidth;
        vec.Y = nHeight;
        poViewer->m_pfnWindowSizeCallback(  poViewer,
                                            &vec);
    }
}

const std::string Viewer::m_strVertexShader =
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

const std::string Viewer::m_strFragmentShader = R"FS(
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

