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

#ifndef PICOGKGLVIEWER_H_
#define PICOGKGLVIEWER_H_

#include "PicoGKTypes.h"
#include "PicoGKApiTypes.h"
#include "PicoGKPolyLine.h"
#include "PicoGKMesh.h"
#include <string>
#include <map>
#include "PicoGKLibraryMgr.h"
#include "PicoGKTrace.h"
#include "PicoGKGLTexture.h"
#include "PicoGKGLViewerShaders.h"
#include "PicoGKGLObjects.h"

struct GLFWwindow;
struct ImGuiContext;

namespace PicoGK
{

class Viewer
{
public:
    
    friend class ViewerManager;
    
    // PKSHAREDPTR(Viewer);
    // Intentionally not using shared pointers with Viewers
    // it's much simpler to pass "this" to the callbacks than
    // a handle that's based on an allocated shared pointer
    // Plus viewers are never shared, so shared pointers are
    // just overkill

    Viewer( GLFWwindow*             pTheWindow,
            ImGuiContext*           psSharedImGuiContext,
            PKPFUpdateRequested     pfnUpdateCallback,
            PKPFKeyPressed          pfnKeyPressedCallback,
            PKPFMouseMoved          pfnMouseMoveCallback,
            PKPFMouseButton         pfnMouseButtonCallback,
            PKPFScrollWheel         pfnScrollWheelCallback,
            PKPFWindowSize          pfnWindowSize);
    
    ~Viewer();
 
    bool bLoadLightSetup(   const char* pDiffuseTextureDDS,
                            uint32_t nDiffuseBufferSize,
                            const char* pSpecularTextureDDS,
                            uint32_t nSepcularBufferSize);
    
    inline void RequestUpdate()
    {
        m_bRedrawNeeded = true;
    }
    
    bool bPoll();
    
    void RequestScreenShot(const std::string& strScreenShotPath);
    
    void RequestClose();
    
    void AddMesh(   int32_t nGroupID,
                    int64_t hLib,
                    int64_t hMesh);
    
    void RemoveMesh(    int64_t hLib,
                        int64_t hMesh);
    
    void AddVoxels( int32_t nGroupID,
                    int64_t hLib,
                    int64_t hVoxels);
    
    void RemoveVoxels(  int64_t hLib,
                        int64_t hVoxels);
    
    void AddPolyLine(   int32_t nGroupID,
                        int64_t hLib,
                        int64_t hPoly);
        
    void RemovePolyLine(    int64_t hLib,
                            int64_t hPoly);
    
    void RemoveAllObjects();

    void SetGroupVisible(   int32_t     nGroupID,
                            bool        bVisible);
    
    void EnableGroupWarnOverhang(   int32_t     nGroupID,
                                    int32_t     nWarningAngleDeg,
                                    int32_t     nErrorAngleDeg);

    void DisableGroupWarnOverhang(int32_t nGroupID);
    
    void SetGroupMaterial(  int32_t     nGroupID,
                            ColorFloat  clr,
                            float       fMetallic,
                            float       fRoughness);
    
    void SetGroupMatrix(    int32_t             nGroupID,
                            const Matrix4x4&    mat);
    
    GLFWwindow* pTheWindow() const      {return m_pTheWindow;}
    
    BBox3 oBBox()
    {
        RecalculateInformationIfNeeded();
        return m_oBBox;
    }
    
protected:
    GLFWwindow*                         m_pTheWindow                = nullptr;
    
    Vector2                             m_vecMousePos;
    bool                                m_bRedrawNeeded             = true;
    
    std::string                         m_strScreenShotPath         = "";
    
    PKPFUpdateRequested                 m_pfnUpdateCallback         = nullptr;
    PKPFKeyPressed                      m_pfnKeyPressedCallback     = nullptr;
    PKPFMouseMoved                      m_pfnMouseMoveCallback      = nullptr;
    PKPFMouseButton                     m_pfnMouseButtonCallback    = nullptr;
    PKPFScrollWheel                     m_pfnScrollWheelCallback    = nullptr;
    PKPFWindowSize                      m_pfnWindowSizeCallback     = nullptr;
    
    void OnKeyPressed(  int iKey,
                        int iScanCode,
                        int iAction,
                        int iModifiers);
    
    void OnMouseMoved(  double dMouseX,
                        double dMouseY);
    
    void OnMouseButton( int iButton,
                        int iAction,
                        int iModifiers);
    
    void OnScrollWheel( double dX,
                        double dY);
    
    void OnWindowSize(  int nWidth,
                        int nHeight);
    
    std::unique_ptr<ShaderProgMeshPoly>  m_roShaderProgMeshPoly;
    
    void Redraw(bool bDraw3dScene);
    
    void DrawScene();
    
    void DrawGui();
    
    void EnsureFrameBuffer(int nX, int nY);
    
GLuint              m_nSceneFBO         = 0;
    GLuint          m_nSceneTex         = 0;
    GLuint          m_nSceneDepth       = 0;
    int             m_nSceneWidth       = 0;
    int             m_nSceneHeight      = 0;
    ImGuiContext*   m_psImGuiContext    = nullptr;
    
    
    class Group
    {
    public:
        PKSHAREDPTR(Group);
        
        void AddMesh(   int64_t hLib,
                        int64_t hMesh,
                        const ShaderProgMeshPoly& oShader)
        {
            PKTRACE(AddMesh);
            
            auto roLib  = Library::oLib().roGetInstance(hLib);
            auto roMesh = roLib->m_oMeshes.roGet(hMesh);
            
            m_oViewMeshes[std::make_pair(hLib, hMesh)] = std::make_unique<ViewMesh>(oShader, *roMesh);
        }
        
        void RemoveMesh(int64_t hLib, int64_t hMesh)
        {
            PKTRACE(RemoveMesh);
            
            auto it = m_oViewMeshes.find(std::make_pair(hLib, hMesh));
            
            if (it == m_oViewMeshes.end())
            {
                assert(false);
                // Trying to remove a Mesh object that doesn't exist
            }
            else
            {
                m_oViewMeshes.erase(it);
            }
        }
        
        bool bFindMesh(int64_t hLib, int64_t hMesh)
        {
            return !(m_oViewMeshes.find(std::make_pair(hLib, hMesh)) == m_oViewMeshes.end());
        }
        
        void AddVoxels( int64_t hLib, int64_t hVoxels,
                       const ShaderProgMeshPoly& oShader)
        {
            PKTRACE(AddVoxels);
            
            auto roLib      = Library::oLib().roGetInstance(hLib);
            auto roVoxels   = roLib->m_oVoxels.roGet(hVoxels);
            
            // Transform to Mesh
            Mesh::Ptr roNew = roVoxels->roAsMesh(roLib->fVoxelSizeMM());
            m_oViewMeshes[std::make_pair(hLib, hVoxels)] = std::make_unique<ViewMesh>(oShader, *roNew);
        }
        
        void RemoveVoxels(int64_t hLib, int64_t hVoxels)
        {
            PKTRACE(RemoveVoxels);
            
            // Find a mesh that is stored with the voxel ID
            auto it = m_oViewMeshes.find(std::make_pair(hLib, hVoxels));
            
            if (it == m_oViewMeshes.end())
            {
                assert(false);
                // Trying to remove a Voxels object that doesn't exist
            }
            else
            {
                m_oViewMeshes.erase(it);
            }
        }
        
        bool bFindVoxels(int64_t hLib, int64_t hVoxels)
        {
            return !(m_oViewMeshes.find(std::make_pair(hLib, hVoxels)) == m_oViewMeshes.end());
        }
        
        void AddPolyLine(   int64_t hLib,
                            int64_t hPoly,
                            const ShaderProgMeshPoly& oShader)
        {
            auto roLib  = Library::oLib().roGetInstance(hLib);
            auto roPoly = roLib->m_oPolyLines.roGet(hPoly);
            
            if (roPoly->nVertexCount() == 0)
                return; // nothing to do
            
            m_oViewPolyLines[std::make_pair(hLib, hPoly)] = std::make_unique<ViewPolyLine>(oShader, *roPoly);
        }
        
        void RemovePolyLine(int64_t hLib, int64_t hPoly)
        {
            auto it = m_oViewPolyLines.find(std::make_pair(hLib, hPoly));
            
            if (it == m_oViewPolyLines.end())
            {
                assert(false);
                // Trying to remove a PolyLine object that doesn't exist
            }
            else
            {
                m_oViewPolyLines.erase(it);
            }
        }
        
        inline bool bFindPolyLine(int64_t hLib, int64_t hPoly)
        {
            return !(m_oViewPolyLines.find(std::make_pair(hLib, hPoly)) == m_oViewPolyLines.end());
        }
        
        inline void RemoveAllObjects()
        {
            m_oViewMeshes.clear();
            m_oViewPolyLines.clear();
        }
        
        inline void SetVisible(bool bVisible)
        {
            m_bVisible = bVisible;
        }
        
        inline bool bVisible() const
        {
            return m_bVisible;
        }
        
        inline void EnableWarnOverhang( int nWarningAngleDeg,
                                        int nErrorAngleDeg)
        {
            m_bWarnOverhang     = true;
            m_nWarningAngleDeg  = nWarningAngleDeg;
            m_nErrorAngleDeg    = nErrorAngleDeg;
        }
        
        inline void DisableWarnOverhang()
        {
            m_bWarnOverhang = false;
        }
        
        void SetMaterial(   ColorFloat  clr,
                            float       fMetallic,
                            float       fRoughness)
        {
            
            m_sMaterial.clr         = clr;
            m_sMaterial.fMetallic   = fMetallic;
            m_sMaterial.fRoughness  = fRoughness;
        }
        
        void SetMatrix(const Matrix4x4& mat)
        {
            m_mat = mat;
        }
        
        void Draw(  const Matrix4x4& matModelTrans,
                    const ShaderProgMeshPoly& oShaderProg);
        
        BBox3 oCalculateBBox() const;
        
    protected:
        bool    m_bVisible          = true;
        bool    m_bWarnOverhang     = false;
        int     m_nWarningAngleDeg  = 0;
        int     m_nErrorAngleDeg    = 0;
        
        Matrix4x4 m_mat;
        
        struct Material
        {
            Material()
            {
                clr.R = 0.13f;
                clr.G = 0.13f;
                clr.B = 0.13f;
                clr.A = 0.9f;
                
                fMetallic   = 0.0f;
                fRoughness  = 0.9f;
            }
            
            ColorFloat  clr;
            float       fMetallic;
            float       fRoughness;
            
        } m_sMaterial;
        
        struct ViewMesh
        {
            PKSHAREDPTR(ViewMesh);
            
            ViewMesh(   const ShaderProgMeshPoly& oShader,
                        const Mesh& oMesh);
            
            void Draw(  const ShaderProgMeshPoly& oShader,
                        const Material& sMaterial,
                        const Matrix4x4& mat,
                        bool bWarnOverhang,
                        int nWarningAngleDeg,
                        int nErrorAngleDeg);
            
            std::unique_ptr<GlElementBuffer<Vector3>>   m_roElementBuffer;
            BBox3                                       m_oBBox;
        };
        
        struct ViewPolyLine
        {
            PKSHAREDPTR(ViewPolyLine);
            
            ViewPolyLine(   const ShaderProgMeshPoly& oShader,
                            const PolyLine& oPoly);
            
            void Draw(  const ShaderProgMeshPoly& oShaderProg,
                        const Material& sMaterial,
                        const Matrix4x4& mat);
            
            std::unique_ptr<GlVertexBuffer<Vector3>>    m_roVertexBuffer;
            ColorFloat                                  m_clrLine;
            BBox3                                       m_oBBox;
        };
        
        std::map<std::pair<int64_t, int64_t>, std::unique_ptr<ViewMesh>>        m_oViewMeshes;
        std::map<std::pair<int64_t, int64_t>, std::unique_ptr<ViewPolyLine>>    m_oViewPolyLines;
    };
    
    Group::Ptr roGroupAt(int nGroupID)
    {
        Group::Ptr roGroup = nullptr;
        
        auto it = m_oGroups.find(nGroupID);
        
        if (it == m_oGroups.end())
        {
            roGroup = std::make_shared<Group>();
            m_oGroups[nGroupID] = roGroup;
        }
        else
        {
            roGroup = it->second;
        }
        
        return roGroup;
    }
    
    std::map<int,Group::Ptr> m_oGroups;
    
    void RecalculateInformationIfNeeded();
    
    void RecalcNeeded()     {m_bRecalcNeeded = true;}
    
    bool                    m_bRecalcNeeded = true;
    
    BBox3                   m_oBBox;
    
    // Viewer GUI elements
  
    class GuiElement
    {
    public:
        PKSHAREDPTR(GuiElement);
        
        GuiElement( Viewer*         poViewer,
                    std::string     strName,
                    bool            bVisible = true)
        
            :   PKINIT(poViewer),
                m_strName(strName),
                PKINIT(bVisible)
        {
           /// TODO, find out if we need to make handle available
        }
        
        void AddChild(GuiElement::Ptr roChild)
        {
            m_oChildren.push_back(roChild);
        }
        
        void RemoveChild(GuiElement::Ptr roChild)
        {
            auto it = std::find(m_oChildren.begin(), m_oChildren.end(), roChild);
            if (it != m_oChildren.end())
            {
                m_oChildren.erase(it);
            }
        }
        
        virtual void Draw()
        {
            if (!m_bVisible)
                return;
            
            Setup();
            
            DrawMe();
            
            for (auto roElement : m_oChildren)
            {
                roElement->Draw();
                roElement->m_bIsItemHovered = ImGui::IsItemHovered();
            }
            
            m_bIsWindowHovered = ImGui::IsWindowHovered();
            Close();
        }
        
        void SetVisible(bool bVisible) {m_bVisible = bVisible;}
        
    protected:
        
        virtual void Setup() = 0;
        
        virtual void DrawMe() = 0;
        
        virtual void Close() = 0;
        
        Viewer*                         m_poViewer;
        std::string                     m_strName;
        GuiElement::Ptr                 m_roParent;
        bool                            m_bVisible;
        std::deque<GuiElement::Ptr>     m_oChildren;
        
        bool                            m_bIsItemHovered      = false;
        bool                            m_bIsWindowHovered    = false;
    };
    
    class SideBar : public GuiElement
    {
    public:
        PKSHAREDPTR(SideBar);
        
        SideBar(    Viewer*     poViewer,
                    bool        bLeft,
                    int         nMin,
                    int         nMax,
                    int         nDef,
                    ColorFloat  clrBackground,
                    ColorFloat  clrBackgroundHv,
                    bool        bVisible = true)
        
        :   GuiElement(     poViewer,
                            bLeft ? "Sidebar_Left" : "Sidebar_Right",
                            bVisible),
            PKINIT(bLeft),
            PKINIT(nMin),
            PKINIT(nMax),
            PKINIT(nDef),
            PKINIT(clrBackground),
            PKINIT(clrBackgroundHv)
        {
            
        }
        
    protected:
        virtual void Setup();
        
        virtual void DrawMe();
        
        virtual void Close();
        
        bool        m_bLeft;
        int         m_nMin;
        int         m_nMax;
        int         m_nDef;
        ColorFloat  m_clrBackground;
        ColorFloat  m_clrBackgroundHv;
    };
    
    class TextLabel : public GuiElement
    {
    public:
        PKSHAREDPTR(TextLabel);
        
        TextLabel(  Viewer*         poViewer,
                    ColorFloat      clrText,
                    std::string     strText,
                    bool            bVisible = true)
        
        :   GuiElement( poViewer,
                        strText,
                        bVisible),
            PKINIT(strText),
            PKINIT(clrText)
        {
            
        }
        
    protected:
        virtual void Setup();
        
        virtual void DrawMe();
        
        virtual void Close();
        
        std::string m_strText;
        ColorFloat  m_clrText;
    };
    
    class Slider : public GuiElement
    {
    public:
        PKSHAREDPTR(GuiElement);
        
        Slider(     Viewer*         poViewer,
                    ColorFloat      clrFrameBg,
                    ColorFloat      clrFrameBgHv,
                    ColorFloat      clrFrameBgActive,
                    ColorFloat      clrGrab,
                    ColorFloat      clrGrabActive,
                    ColorFloat      clrText,
                    std::string     strText,
                    float           fMin,
                    float           fMax,
                    float           fValue,
                    bool            bVisible = true)
        
        :   GuiElement( poViewer,
                        strText,
                        bVisible),
            PKINIT(clrFrameBg),
            PKINIT(clrFrameBgHv),
            PKINIT(clrFrameBgActive),
            PKINIT(clrGrab),
            PKINIT(clrGrabActive),
            PKINIT(clrText),
            PKINIT(fMin),
            PKINIT(fMax),
            PKINIT(fValue)
        {
            
        }
        
    protected:
        virtual void Setup();
        
        virtual void DrawMe();
        
        virtual void Close();
        
        ColorFloat      m_clrFrameBg;
        ColorFloat      m_clrFrameBgHv;
        ColorFloat      m_clrFrameBgActive;
        ColorFloat      m_clrGrab;
        ColorFloat      m_clrGrabActive;
        ColorFloat      m_clrText;
        float           m_fMin;
        float           m_fMax;
        float           m_fValue;
    };

public:
    uint64_t hCreateGpuTexture( int nWidth,
                                int nHeight,
                                const char* pBuffer)
    {
        return m_oTextures.hAdd(nWidth, nHeight, pBuffer);
    }
    
    void MarkGpuTextureForCleanup(uint64_t hTexture)
    {
        m_oTextures.MarkForDestruction(hTexture);
    }
    
protected:
    GpuTextureList  m_oTextures;
    
public:
    bool bGetTexture(   uint64_t nTextureId,
                        GLuint* pnGlTexture,
                        int*    pnWidth,
                        int*    pnHeight) const
    {
        return m_oTextures.bGetGlHandle(    nTextureId,
                                            pnGlTexture,
                                            pnWidth,
                                            pnHeight);
    }
    
    void ShowAllTextures() const
    {
        m_oTextures.ShowAllTextures();
    }
    
public:
    uint64_t hCreateSideBar(    bool                bLeft,
                                int                 nMin,
                                int                 nMax,
                                int                 nDef,
                                ColorFloat          clrBackground,
                                ColorFloat          clrBackgroundHv);
    
    void DestroySideBar(uint64_t hSideBar);
    
protected:
    HandleManager<GuiElement>   m_oGuiElements;
    uint64_t                    m_hSideBarLeft     = 0;
    uint64_t                    m_hSideBarRight    = 0;
};

} // namespace PicoGK

#endif /* PICOGKGLVIEWER_H_ */
