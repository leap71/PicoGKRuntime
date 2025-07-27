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

#ifndef PICOGKGLVIEWER_H_
#define PICOGKGLVIEWER_H_

#include "PicoGKTypes.h"
#include "PicoGKApiTypes.h"
#include "PicoGKPolyLine.h"
#include "PicoGKMesh.h"
#include <string>
#include <map>
#include "gl/gl.h"
#include "PicoGKLibraryMgr.h"
#include "PicoGKTrace.h"

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
    
    void SetGroupStatic(    int32_t     nGroupID,
                            bool        bStatic);
    
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
    
    static const std::string            m_strVertexShader;
    static const std::string            m_strFragmentShader;
    
    struct ShaderConfig
    {
        uint32_t    nVertexShader         = 0;
        uint32_t    nFragmentShader       = 0;
        uint32_t    nProgram              = 0;

        GLuint      nTexDiffuse           = 0;
        GLuint      nTexSpecular          = 0;

        int         iOtoWUniform          = -1;
        int         iMVPUniform           = -1;
        int         iEyeUniform           = -1;
        int         iColorUniform         = -1;
        int         iMetallicUniform      = -1;
        int         iRoughnessUniform     = -1;
        int         iDiffuseUniform       = -1;
        int         iSpecularUniform      = -1;

        int         iPosAttrib            = -1;
    } m_sConfig;
    
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
        
        Group()
        {
            m_bStatic  = false;
            m_bVisible = true;
        }
        
        ~Group()
        {
            
        }
        
        void AddMesh(int64_t hLib, int64_t hMesh)
        {
            PKTRACE(AddMesh);
            
            auto roLib  = Library::oLib().roGetInstance(hLib);
            auto roMesh = roLib->m_oMeshes.roGet(hMesh);
            
            // Add a copy, so we dont depend on the original mesh
            Mesh::Ptr roNew = std::make_shared<Mesh>(*roMesh);
            
            m_oViewMeshes[std::make_pair(hLib, hMesh)] = std::make_shared<ViewMesh>(roNew);
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
        
        void AddVoxels(int64_t hLib, int64_t hVoxels)
        {
            PKTRACE(AddVoxels);
            
            auto roLib      = Library::oLib().roGetInstance(hLib);
            auto roVoxels   = roLib->m_oVoxels.roGet(hVoxels);
            
            // Transform to Mesh
            Mesh::Ptr roNew = roVoxels->roAsMesh(roLib->fVoxelSizeMM());
            
            m_oViewMeshes[std::make_pair(hLib, hVoxels)] = std::make_shared<ViewMesh>(roNew);
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
        
        void AddPolyLine(int64_t hLib, int64_t hPoly)
        {
            auto roLib  = Library::oLib().roGetInstance(hLib);
            auto roPoly = roLib->m_oPolyLines.roGet(hPoly);
            
            // Add a copy, so we dont depend on the original polyline
            PolyLine::Ptr roNew = std::make_shared<PolyLine>(*roPoly);
            
            m_oViewPolyLines[std::make_pair(hLib, hPoly)] = std::make_shared<ViewPolyLine>(roNew);
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
        
        inline void SetStatic(bool bStatic)
        {
            m_bStatic = bStatic;
        }
        
        inline bool bStatic() const
        {
            return m_bStatic;
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
                    const ShaderConfig& sConfig);
        
        BBox3 oCalculateBBox() const;
        
    protected:
        bool m_bVisible;
        bool m_bStatic;
        
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
            
            ViewMesh(const Mesh::Ptr& roMesh);
            
            void Draw(  const ShaderConfig& sConfig,
                        const Material& sMaterial,
                        const Matrix4x4& mat);
            
            struct GLParams
            {
                GLuint  nVertexArray;
                GLuint  nArrayBuffer;
                GLuint  nElementArrayBuffer;
            } sGLParams;
            
            Mesh::Ptr m_roMesh;
        };
        
        struct ViewPolyLine
        {
            PKSHAREDPTR(ViewPolyLine);
            
            ViewPolyLine(const PolyLine::Ptr& roPoly);
            
            void Draw(  const ShaderConfig& sConfig,
                        const Material& sMaterial,
                        const Matrix4x4& mat);
            
            struct GLParams
            {
                GLuint  nVertexArray;
                GLuint  nArrayBuffer;
            } sGLParams;
            
            PolyLine::Ptr m_roPolyLine;
        };
        
        std::map<std::pair<int64_t, int64_t>, ViewMesh::Ptr>        m_oViewMeshes;
        std::map<std::pair<int64_t, int64_t>, ViewPolyLine::Ptr>    m_oViewPolyLines;
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
        
        GuiElement( uint64_t        hHandle,
                    Viewer*         poViewer,
                    std::string     strName,
                    bool            bVisible = true)
        
            :   PKINIT(hHandle),
                PKINIT(poViewer),
                m_strName(strName + "##" + std::to_string(hHandle)),
                PKINIT(bVisible)
        {
           
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
            }
            
            Close();
        }
        
        void SetVisible(bool bVisible) {m_bVisible = bVisible;}
        
    protected:
        
        virtual void Setup() = 0;
        
        virtual void DrawMe() = 0;
        
        virtual void Close() = 0;
        
        uint64_t                        m_hHandle;
        Viewer*                         m_poViewer;
        std::string                     m_strName;
        GuiElement::Ptr                 m_roParent;
        bool                            m_bVisible;
        std::deque<GuiElement::Ptr>     m_oChildren;
    };
    
    class SideBar : public GuiElement
    {
    public:
        SideBar(    uint64_t    hHandle,
                    Viewer*     poViewer,
                    ColorFloat  clrBackground,
                    int         nMin,
                    int         nMax,
                    int         nDef,
                    bool        bLeft,
                    bool        bVisible = true)
        
        :   GuiElement(     hHandle,
                            poViewer,
                            bLeft ? "Sidebar_Left" : "Sidebar_Right",
                            bVisible),
            PKINIT(clrBackground),
            PKINIT(nMin),
            PKINIT(nMax),
            PKINIT(nDef),
            PKINIT(bLeft)
        {
            
        }
        
    protected:
        virtual void Setup();
        
        virtual void DrawMe();
        
        virtual void Close();
        
        ColorFloat  m_clrBackground;
        int         m_nMin;
        int         m_nMax;
        int         m_nDef;
        bool        m_bLeft;
    };
    
    class TextLabel : public GuiElement
    {
    public:
        TextLabel(  uint64_t        hHandle,
                    Viewer*         poViewer,
                    ColorFloat      clrText,
                    std::string     strText,
                    bool            bVisible = true)
        
        :   GuiElement( hHandle,
                        poViewer,
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
        Slider(     uint64_t        hHandle,
                    Viewer*         poViewer,
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
        
        :   GuiElement( hHandle,
                        poViewer,
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
};

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
