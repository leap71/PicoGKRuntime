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

struct GLFWwindow;

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

    Viewer( GLFWwindow*,
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
    
    void AddMesh(   int32_t             nGroupID,
                    const Mesh::Ptr*    proMesh);
    
    void RemoveMesh(const Mesh::Ptr* proMesh);
    
    void AddPolyLine(   int32_t                 nGroupID,
                        const PolyLine::Ptr*    proPoly);
        
    void RemovePolyLine(const PolyLine::Ptr* proPoly);

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
    
    void Redraw();

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
        
        void AddMesh(const Mesh::Ptr* proMesh)
        {
            m_oViewMeshes[proMesh] = std::make_shared<ViewMesh>(*proMesh);
        }
        
        void RemoveMesh(const Mesh::Ptr* proMesh)
        {
            auto it = m_oViewMeshes.find(proMesh);
            
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
        
        bool bFindMesh(const Mesh::Ptr* proMesh)
        {
            return !(m_oViewMeshes.find(proMesh) == m_oViewMeshes.end());
        }
        
        void AddPolyLine(const PolyLine::Ptr* proPoly)
        {
            m_oViewPolyLines[proPoly] = std::make_shared<ViewPolyLine>(*proPoly);
        }
        
        void RemovePolyLine(const PolyLine::Ptr* proPoly)
        {
            auto it = m_oViewPolyLines.find(proPoly);
            
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
        
        inline bool bFindPolyLine(const PicoGK::PolyLine::Ptr* proPoly)
        {
            return !(m_oViewPolyLines.find(proPoly) == m_oViewPolyLines.end());
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
        
        std::map<const Mesh::Ptr*,      ViewMesh::Ptr>        m_oViewMeshes;
        std::map<const PolyLine::Ptr*,  ViewPolyLine::Ptr>    m_oViewPolyLines;
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
    PKFInfo                         m_pfnInfoCallback;
    std::map<GLFWwindow*, Viewer*>  m_oViewers;
};

} // namespace PicoGK

#endif /* PICOGKGLVIEWER_H_ */
