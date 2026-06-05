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
#include "PicoGKGLViewer.h"
#include "PicoGKGLTexture.h"
#include <fstream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace PicoGK
{

void Viewer::SideBar::Setup()
{
    ImGuiIO& io = ImGui::GetIO();
    
    ImVec2 vecWindowSize = ImVec2(m_nDef, io.DisplaySize.y);

    // Set position to top-right, with pivot (1.0f, 0.0f)
    ImGui::SetNextWindowPos(    ImVec2(m_bLeft ? 0 : io.DisplaySize.x, 0),
                                ImGuiCond_Always,
                                ImVec2(m_bLeft ? 0.0f : 1.0f, 0.0f));
    
    ImGui::SetNextWindowSize(vecWindowSize, ImGuiCond_Once);
    
    ImGui::SetNextWindowSizeConstraints(    ImVec2(m_nMin, vecWindowSize.y),
                                            ImVec2(m_nMax, vecWindowSize.y));
    
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove;
    
    ImVec4 clr = m_bIsWindowHovered ? m_clrBackgroundHv.sToImVec4() : m_clrBackground.sToImVec4();

    ImGui::PushStyleColor(  ImGuiCol_WindowBg,
                            clr);
    
    ImGui::Begin(m_strName.c_str(), nullptr, flags);
}

void Viewer::SideBar::DrawMe()
{
    // Nothing to do (only children will be drawn)
}

void Viewer::SideBar::Close()
{
    ImGui::End();
    ImGui::PopStyleColor();
}


void Viewer::TextLabel::Setup()
{
    ImGui::PushStyleColor(  ImGuiCol_Text,
                            m_clrText.sToImVec4());
}

void Viewer::TextLabel::DrawMe()
{
    ImGui::Text("%s", m_strName.c_str());
}

void Viewer::TextLabel::Close()
{
    ImGui::PopStyleColor();
}

void Viewer::Slider::Setup()
{
    ImGui::PushStyleColor(ImGuiCol_FrameBg,             m_clrFrameBg.sToImVec4());
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,      m_clrFrameBgHv.sToImVec4());
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive,       m_clrFrameBgActive.sToImVec4());
    ImGui::PushStyleColor(ImGuiCol_SliderGrab,          m_clrGrab.sToImVec4());
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive,    m_clrGrabActive.sToImVec4());
    ImGui::PushStyleColor(ImGuiCol_Text,                m_clrText.sToImVec4());
}

void Viewer::Slider::DrawMe()
{
    ImGui::SliderFloat(m_strName.c_str(), &m_fValue, m_fMin, m_fMax);
}

void Viewer::Slider::Close()
{
    ImGui::PopStyleColor(6);
}

} // namespace PicoGK

