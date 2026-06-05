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

#ifndef PICOGKGLVIEWERMANAGER_H_
#define PICOGKGLVIEWERMANAGER_H_

struct GLFWwindow;
struct ImGuiContext;

#include "PicoGKTypes.h"
#include "PicoGKApiTypes.h"

#include <string>
#include <shared_mutex>
#include <map>

struct GLFWwindow;
struct ImGuiContext;

namespace PicoGK
{

class Viewer;

class ViewerManager
{
public:
    
    Viewer* poCreate(   const std::string&      strWindowTitle,
                        const Vector2&          vecSize,
                        PKFInfo                 pfnInfoCallback,
                        PKPFUpdateRequested     pfnUpdateCallback,
                        PKPFKeyPressed          pfnKeyPressedCallback,
                        PKPFMouseMoved          pfnMouseMoveCallback,
                        PKPFMouseButton         pfnMouseButtonCallback,
                        PKPFScrollWheel         pfnScrollWheelCallback,
                        PKPFWindowSize          pfnWindowSizeCallback);

    void Destroy(Viewer* poViewer);
    
    bool bIsValid(const Viewer* poViewer) const;
    
    bool bExists(const Viewer* poViewer) const;
    
    void ReportInfo(const std::string strInfo, bool bFatal);
    
    static ViewerManager& oMgr()
    {
        static ViewerManager oSingleton;
        return oSingleton;
    }
    
    static void Info(   const std::string strInfo,
                        bool bFatalError = false)
    {
        oMgr().ReportInfo(strInfo, bFatalError);
    }
    
    static void KeyPressed( GLFWwindow* psWindow,
                            int iKey,
                            int iScanCode,
                            int iAction,
                            int iModifiers);

    static void MouseMoved( GLFWwindow* psWindow,
                            double dMouseX,
                            double dMouseY);

    static void MouseButton(    GLFWwindow* psWindow,
                                int iButton,
                                int iAction,
                                int iModifiers);
    
    static void ScrollWheel(    GLFWwindow* psWindow,
                                double dX,
                                double dY);
    
    static void WindowSize(     GLFWwindow* psWindow,
                                int nWidth,
                                int nHeight);
    
    ViewerManager(const ViewerManager&)                 = delete;
    ViewerManager& operator = (const ViewerManager&)    = delete;
    
private:
    ViewerManager();
    
    ~ViewerManager();
    
protected:
    ImGuiContext*                   m_psSharedImGuiContext  = nullptr;
    PKFInfo                         m_pfnInfoCallback       = nullptr;
    mutable std::shared_mutex       m_mtx;
    std::map<GLFWwindow*, Viewer*>  m_oViewers;
};

} // namespace PicoGK

#endif /* PICOGKGLVIEWER_H_ */
