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
#include "PicoGKGLViewer.h"
#include "PicoGKGLTexture.h"

#include <GLFW/glfw3.h>

#include <fstream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "PicoGKGLViewerManager.h"
#include "PicoGKGLDebug.h"

namespace PicoGK
{

Viewer::Viewer( GLFWwindow*             pTheWindow,
                ImGuiContext*           psSharedImGuiContext,
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

    glfwMakeContextCurrent(m_pTheWindow);
    gladLoadGL(glfwGetProcAddress);
    
    m_psImGuiContext = psSharedImGuiContext;
    ImGui::SetCurrentContext(m_psImGuiContext);
    
    // Then: backend init
    ImGui_ImplGlfw_InitForOpenGL(pTheWindow, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    ImGuiIO& io = ImGui::GetIO(); (void)io;

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
    glfwMakeContextCurrent(m_pTheWindow);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    if (m_psImGuiContext != nullptr)
    {
        /// TODO
        //ImGui::SetCurrentContext(m_psImGuiContext);
        //ImGui::DestroyContext(m_psImGuiContext);
    }
    
    m_oTextures.CleanupAllTextures();

    if (m_sConfig.nProgram != 0)
        glDeleteProgram(m_sConfig.nProgram);
    
    if (m_sConfig.nVertexShader != 0)
        glDeleteShader(m_sConfig.nVertexShader);
    
    if (m_sConfig.nFragmentShader != 0)
        glDeleteShader(m_sConfig.nFragmentShader);

    if (m_nSceneTex != 0)
        glDeleteTextures(1, &m_nSceneTex);
    
    if (m_nSceneFBO != 0)
        glDeleteFramebuffers(1, &m_nSceneFBO);

    // Destroy GLFW window
    if (m_pTheWindow != nullptr)
    {
        glfwDestroyWindow(m_pTheWindow);
        m_pTheWindow = nullptr;
    }
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
    if (m_bRedrawNeeded)
    {
        m_bRedrawNeeded = false;
        Redraw(true);
    }
    else
    {
        Redraw(false);
    }
    
    glfwPollEvents();
    return !glfwWindowShouldClose(m_pTheWindow);
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

void Viewer::AddMesh(   int32_t         nGroupID,
                        int64_t         hLib,
                        int64_t         hMesh)
{
    // Remove Mesh if already there
    RemoveMesh(hLib, hMesh);
    
    RecalcNeeded();
    roGroupAt(nGroupID)->AddMesh(hLib, hMesh);
    RequestUpdate();
}

void Viewer::RemoveMesh(    int64_t         hLib,
                            int64_t         hMesh)
{
    RecalcNeeded();
    for (auto Pair : m_oGroups)
    {
        Group::Ptr poGroup = Pair.second;
        if (poGroup->bFindMesh(hLib, hMesh))
        {
            poGroup->RemoveMesh(hLib, hMesh);
            RequestUpdate();
            return;
        }
    }
}

void Viewer::AddVoxels(     int32_t         nGroupID,
                            int64_t         hLib,
                            int64_t         hVoxels)
{
    // Remove Voxels if already there
    RemoveVoxels(hLib, hVoxels);
    
    RecalcNeeded();
    roGroupAt(nGroupID)->AddVoxels(hLib, hVoxels);
    RequestUpdate();
}

void Viewer::RemoveVoxels(  int64_t hLib,
                            int64_t hVoxels)
{
    RecalcNeeded();
    for (auto Pair : m_oGroups)
    {
        Group::Ptr poGroup = Pair.second;
        if (poGroup->bFindVoxels(hLib, hVoxels))
        {
            poGroup->RemoveVoxels(hLib, hVoxels);
            RequestUpdate();
            return;
        }
    }
}

void Viewer::AddPolyLine(   int32_t nGroupID,
                            int64_t hLib,
                            int64_t hPoly)
{
    // Remove PolyLine if already there
    RemoveMesh(hLib, hPoly);
    
    RecalcNeeded();
    roGroupAt(nGroupID)->AddPolyLine(hLib, hPoly);
    RequestUpdate();
}

void Viewer::RemovePolyLine(    int64_t hLib,
                                int64_t hPoly)
{
    RecalcNeeded();
    for (auto Pair : m_oGroups)
    {
        Group::Ptr poGroup = Pair.second;
        if (poGroup->bFindPolyLine(hLib, hPoly))
        {
            poGroup->RemovePolyLine(hLib, hPoly);
            RequestUpdate();
            return;
        }
    }
}

void Viewer::RemoveAllObjects()
{
    for (auto Pair : m_oGroups)
    {
        Pair.second->RemoveAllObjects();
    }
    
    RecalcNeeded();
    RequestUpdate();
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

void Viewer::OnKeyPressed(  int iKey,
                            int iScanCode,
                            int iAction,
                            int iModifiers)
{
    ImGui::SetCurrentContext(m_psImGuiContext);
    if (ImGui::GetIO().WantCaptureKeyboard)
        return;
    
    if (m_pfnKeyPressedCallback != nullptr)
    {
        m_pfnKeyPressedCallback(    this,
                                    iKey,
                                    iScanCode,
                                    iAction,
                                    iModifiers);
    }
}

void Viewer::OnMouseMoved(  double dMouseX,
                            double dMouseY)
{
    ImGui::SetCurrentContext(m_psImGuiContext);
    if (ImGui::GetIO().WantCaptureMouse)
        return;
    
    m_vecMousePos.X = (float) dMouseX;
    m_vecMousePos.Y = (float) dMouseY;

    if (m_pfnMouseMoveCallback != nullptr)
    {
        m_pfnMouseMoveCallback(this, &m_vecMousePos);
    }
}

void Viewer::OnMouseButton( int iButton,
                            int iAction,
                            int iModifiers)
{
    ImGui::SetCurrentContext(m_psImGuiContext);
    if (ImGui::GetIO().WantCaptureMouse)
        return;
            
    if (m_pfnMouseButtonCallback != nullptr)
    {
        m_pfnMouseButtonCallback(   this,
                                    iButton,
                                    iAction,
                                    iModifiers,
                                    &m_vecMousePos);
    }
}

void Viewer::OnScrollWheel( double dX,
                            double dY)
{
    ImGui::SetCurrentContext(m_psImGuiContext);
    if (ImGui::GetIO().WantCaptureMouse)
        return;
            
    if (m_pfnScrollWheelCallback != nullptr)
    {
        Vector2 vec;
        vec.X = dX;
        vec.Y = dY;
        m_pfnScrollWheelCallback(this, &vec, &m_vecMousePos);
    }
}

void Viewer::OnWindowSize(  int nWidth,
                            int nHeight)
{
    if (m_pfnWindowSizeCallback != nullptr)
    {
        Vector2 vec;
        vec.X = nWidth;
        vec.Y = nHeight;
        m_pfnWindowSizeCallback(this, &vec);
    }
}

void Viewer::EnsureFrameBuffer(int nX, int nY)
{
    if (m_nSceneFBO != 0 && nX == m_nSceneWidth && nY == m_nSceneHeight)
        return; // no change

    if (m_nSceneFBO != 0)
    {
        // clean up existing framebuffer
        glDeleteFramebuffers(1, &m_nSceneFBO);
        glDeleteTextures(1, &m_nSceneTex);
        glDeleteRenderbuffers(1, &m_nSceneDepth);
    }

    m_nSceneWidth   = nX;
    m_nSceneHeight  = nY;

    glGenFramebuffers(1, &m_nSceneFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_nSceneFBO);

    // Color texture
    glGenTextures(1, &m_nSceneTex);
    glBindTexture(GL_TEXTURE_2D, m_nSceneTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nX, nY, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_nSceneTex, 0);

    // Depth renderbuffer
    glGenRenderbuffers(1, &m_nSceneDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, m_nSceneDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, nX, nY);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_nSceneDepth);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(status == GL_FRAMEBUFFER_COMPLETE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Viewer::Redraw(bool bRedraw3dScane)
{
    try
    {
        glfwMakeContextCurrent(m_pTheWindow);
        
        m_oTextures.ManageTextureState();
        
        int iWidth, iHeight;
        glfwGetFramebufferSize(m_pTheWindow, &iWidth, &iHeight);
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        EnsureFrameBuffer(iWidth, iHeight);
        
        if (bRedraw3dScane)
            DrawScene();
        
        ImVec2 vScale   = ImGui::GetIO().DisplayFramebufferScale;
        ImVec2 vSize    = ImVec2(iWidth / vScale.x, iHeight / vScale.y);
        
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(vSize);
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::Begin("Scene View", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);
        ImTextureID texID = (ImTextureID)(intptr_t)m_nSceneTex;
        ImGui::Image(texID, vSize, ImVec2(0, 1), ImVec2(1, 0)); // Flip Y

        ImGui::End();
        ImGui::PopStyleVar(2);
        
        DrawGui();
        
        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(m_pTheWindow);
    }
    
    catch (...)
    {
    }
}

void Viewer::DrawScene()
{
    if (m_nSceneFBO == 0)
    {
        ViewerManager::Info("No scene FBO available - not drawing scene");
        return;
    }
    
    // Bind FBO instead of default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_nSceneFBO);
    glViewport(0, 0, m_nSceneWidth, m_nSceneHeight);
    
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
        vecViewSize.X = (float) m_nSceneWidth;
        vecViewSize.Y = (float) m_nSceneHeight;
        
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
        std::vector<unsigned char> image(m_nSceneWidth * m_nSceneHeight * 3); // 3 bytes per pixel (RGB)
        glReadPixels(0, 0, m_nSceneWidth, m_nSceneHeight, GL_BGR, GL_UNSIGNED_BYTE, image.data());
        
        // Save the image as a TGA file
        SaveTGA(m_strScreenShotPath, image, m_nSceneWidth, m_nSceneHeight);
        m_strScreenShotPath = "";
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Viewer::DrawGui()
{
   
    /*SideBar::Ptr    roSBL   = std::make_shared<SideBar>     (0, this, ColorFloat(1,1,1,0.5f), 100, 400, 200, true);
    TextLabel::Ptr  roLabel = std::make_shared<TextLabel>   (1, this, ColorFloat(0,0,1,0.9f), "Hello World");
    Slider::Ptr     roSlider = std::make_shared<Slider>  (      3,
                                                                this,
                                                                ColorFloat(1,1,1,.7f),
                                                                ColorFloat(1,1,1,.9f),
                                                                ColorFloat(1,1,1,1),
                                                                ColorFloat(1,0,0,.7f),
                                                                ColorFloat(1,0,0,1),
                                                                ColorFloat(.5f,.5f,.5f,.9f),
                                                                "Value",
                                                                -1.0f,2.0f,
                                                                0.0f);
    
    
                                                                           
    roSBL->AddChild(roLabel);
    roSBL->AddChild(roSlider);
    
    roSBL->Draw();
    
    SideBar oSBR(2, this, ColorFloat(1,1,1,0.5f), 100, 400, 200, false);
    oSBR.Draw();*/
}

void Viewer::RecalculateInformationIfNeeded()
{
    if (!m_bRecalcNeeded)
        return;
    
    // initialize empty
    m_oBBox = BBox3();
    
    for (auto Pair : m_oGroups)
    {
        Group::Ptr poGroup = Pair.second;
        m_oBBox.Include(poGroup->oCalculateBBox());
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

