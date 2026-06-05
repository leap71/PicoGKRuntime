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
#include "PicoGKGLViewerManager.h"
#include <GLFW/glfw3.h>
#include "PicoGKGLViewer.h"

namespace PicoGK
{

static void ErrorCallback(int iError, const char* pszDescription)
{
    ViewerManager::Info("GL Callback Error: " + std::string(pszDescription));
}

ViewerManager::ViewerManager()
{
    m_pfnInfoCallback = nullptr;
    
    if (!glfwInit())
    {
        fprintf(stderr, "GLFW initialization failed\n");
        return;
    }
    
    IMGUI_CHECKVERSION();
    m_psSharedImGuiContext = ImGui::CreateContext(); // optional, shared fonts
    ImGui::StyleColorsDark();
}

ViewerManager::~ViewerManager()
{
    for (auto& it : m_oViewers)
    {
        delete it.second;
    }
        
    m_oViewers.clear();

    if (m_psSharedImGuiContext)
    {
        ImGui::DestroyContext(m_psSharedImGuiContext);
    }

    glfwTerminate();
}

static GLFWwindow* pCreateWindowWithGLVersion(
    int                 nWidth,
    int                 nHeight,
    const std::string&  strWindowTitle,
    int                 nMajor,
    int                 nMinor)
{
    glfwDefaultWindowHints();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, nMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, nMinor);

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // required on macOS
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

    return glfwCreateWindow(
        nWidth,
        nHeight,
        strWindowTitle.c_str(),
        nullptr,
        nullptr);
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
    {
        std::unique_lock lk(m_mtx);
        m_pfnInfoCallback = pfnInfoCallback;
    }
    
    glfwSetErrorCallback(ErrorCallback);
    
     // Try OpenGL 4.1
    GLFWwindow* pWindow = pCreateWindowWithGLVersion(   (GLint) vecSize.X,
                                                        (GLint) vecSize.Y,
                                                        strWindowTitle.c_str(),
                                                        4,1);

    if (pWindow == nullptr)
    {
        ViewerManager::Info("Failed to create OpenGL 4.1 context, trying OpenGL 3.3", false);
        // Failed, fall back to OpenGL 3.3
        pWindow = pCreateWindowWithGLVersion(   (GLint) vecSize.X,
                                                (GLint) vecSize.Y,
                                                strWindowTitle.c_str(),
                                                3,3);
    }

    if (pWindow == nullptr)
    {
        ViewerManager::Info("Failed to get OpenGL window", true);
        return nullptr;
    }
    
    glfwMakeContextCurrent(pWindow);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        ViewerManager::Info("Unable to initialize OpenGL", true);
        return nullptr;
    }

    const GLubyte* pVersion  = glGetString(GL_VERSION);
    const GLubyte* pRenderer = glGetString(GL_RENDERER);
    const GLubyte* pVendor   = glGetString(GL_VENDOR);

    ViewerManager::Info(
        std::string("OpenGL: ") +
        (pVersion  ? reinterpret_cast<const char*>(pVersion)  : "<unknown version>") +
        " / " +
        (pRenderer ? reinterpret_cast<const char*>(pRenderer) : "<unknown renderer>") +
        " / " +
        (pVendor   ? reinterpret_cast<const char*>(pVendor)   : "<unknown vendor>"),
        false);
    
    {
        std::unique_lock lk(m_mtx);
        glfwSetKeyCallback(         pWindow, KeyPressed);
        glfwSetCursorPosCallback(   pWindow, MouseMoved);
        glfwSetMouseButtonCallback( pWindow, MouseButton);
        glfwSetScrollCallback(      pWindow, ScrollWheel);
        glfwSetWindowSizeCallback(  pWindow, WindowSize);
    }
    
    Viewer* poViewer = new Viewer(  pWindow,
                                    m_psSharedImGuiContext,
                                    pfnUpdateCallback,
                                    pfnKeyPressedCallback,
                                    pfnMouseMoveCallback,
                                    pfnMouseButtonCallback,
                                    pfnScrollWheelCallback,
                                    pfnWindowSizeCallback);

    {
        std::unique_lock lk(m_mtx);
        m_oViewers[pWindow] = poViewer;
    }
    
    poViewer->RequestUpdate();
    return poViewer;
}

void ViewerManager::Destroy(Viewer* poViewer)
{
    std::unique_lock lk(m_mtx);
    
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
    std::shared_lock lk(m_mtx);
    for (auto Entry : m_oViewers)
    {
        if (Entry.second == poViewer)
            return true;
    }
    
    return false;
}

void ViewerManager::ReportInfo(const std::string strInfo, bool bFatal)
{
    std::shared_lock lk(m_mtx);
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
    
    if (poViewer == nullptr)
        return;
    
    poViewer->OnKeyPressed(iKey, iScanCode, iAction, iModifiers);
}

void ViewerManager::MouseMoved(  GLFWwindow* pWindow,
                                 double dMouseX,
                                 double dMouseY)
{
    Viewer* poViewer = oMgr().m_oViewers[pWindow];
    
    if (poViewer == nullptr)
        return;
    
    poViewer->OnMouseMoved(dMouseX, dMouseY);
}

void ViewerManager::MouseButton( GLFWwindow* pWindow,
                                 int iButton,
                                 int iAction,
                                 int iModifiers)
{
    Viewer* poViewer = oMgr().m_oViewers[pWindow];
    
    if (poViewer == nullptr)
        return;
    
    poViewer->OnMouseButton(iButton, iAction, iModifiers);
}

void ViewerManager::ScrollWheel(    GLFWwindow* pWindow,
                                    double dX,
                                    double dY)
{
    Viewer* poViewer = oMgr().m_oViewers[pWindow];
    
    if (poViewer == nullptr)
        return;

    poViewer->OnScrollWheel(dX, dY);
}

void ViewerManager::WindowSize(    GLFWwindow* pWindow,
                                   int nWidth,
                                   int nHeight)
{
    Viewer* poViewer = oMgr().m_oViewers[pWindow];
    
    if (poViewer == nullptr)
        return;
    
    poViewer->OnWindowSize(nWidth, nHeight);
}

} // namespace PicoGK

