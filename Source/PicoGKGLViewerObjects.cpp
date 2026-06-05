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
#include "PicoGKGLDebug.h"

namespace PicoGK
{

Viewer::Group::ViewPolyLine::ViewPolyLine(  const ShaderProgMeshPoly& oShader,
                                            const PicoGK::PolyLine& oPoly)
{
    oShader.CreateBufferInstance(oPoly.vVertices(), &m_roVertexBuffer);
    m_clrLine           = oPoly.clrLines();
    m_oBBox             = oPoly.oBBox();
}

void Viewer::Group::ViewPolyLine::SetMatrix(const Matrix4x4& mat)
{
    m_mat = mat;
}

void Viewer::Group::ViewPolyLine::Draw( const ShaderProgMeshPoly& oShader,
                                        const Material& oMaterial,
                                        const Matrix4x4& mat)
{
    // Apply object matrix first, then apply group matrix
    Matrix4x4 matResult = mat;
    matResult *= m_mat;
    
    oShader.SetValues(  matResult,
                        m_clrLine);
    
    GlBind oBind(*m_roVertexBuffer);
    glDrawArrays(GL_LINE_STRIP, 0, m_roVertexBuffer->nVertexCount());
    
    CHECKGLERRORS;
}

void Viewer::Group::Draw(const ShaderProgMeshPoly& oShader)
{
    if (!m_bVisible)
        return;
    
    for (auto& oMapEntry : m_oViewPolyLines)
    {
        oMapEntry.second->Draw(oShader, m_sMaterial, m_mat);
    }
    
    for (auto& oMapEntry : m_oViewMeshes)
    {
        oMapEntry.second->Draw( oShader,
                                m_sMaterial,
                                m_mat,
                                m_bWarnOverhang,
                                m_fWarningOverhang,
                                m_fErrorOverhang);
    }
}

BBox3 Viewer::Group::oCalculateBBox() const
{
    BBox3 oBBox;
    
    for (auto& oMapEntry : m_oViewPolyLines)
    {
        oBBox.Include(oMapEntry.second->m_oBBox);
    }
    
    for (auto& oMapEntry : m_oViewMeshes)
    {
        oBBox.Include(oMapEntry.second->m_oBBox);
    }
    
    return oBBox;
}

Viewer::Group::ViewMesh::ViewMesh(  const ShaderProgMeshPoly& oShader,
                                    const Mesh& oMesh)
{
    oShader.CreateBufferInstance(   oMesh.vVertices(),
                                    oMesh.vTriangles(),
                                    &m_roElementBuffer);
    m_oBBox = oMesh.oBBox();
    CHECKGLERRORS;
}

void Viewer::Group::ViewMesh::SetMatrix(const Matrix4x4& mat)
{
    m_mat = mat;
}

void Viewer::Group::ViewMesh::Draw( const ShaderProgMeshPoly& oShader,
                                    const Material& sMaterial,
                                    const Matrix4x4& mat,
                                    bool bWarnOverhang,
                                    float fWarning,
                                    float fError)
{
    // Apply object matrix first, then apply group matrix
    Matrix4x4 matResult = mat;
    matResult *= m_mat;
    
    oShader.SetValues(  matResult,
                        sMaterial.clr,
                        sMaterial.fMetallic,
                        sMaterial.fRoughness,
                        bWarnOverhang,
                        fWarning,
                        fError);
    
    GlBind oBind(*m_roElementBuffer);
    glDrawElements(GL_TRIANGLES, m_roElementBuffer->nIndexCount(), GL_UNSIGNED_INT, nullptr);

    CHECKGLERRORS;
}

//
//
// class Viewer::ViewQuad
//
//

Viewer::ViewQuad::ViewQuad( const ShaderProgQuad& oShader,
                            uint64_t    hTexObject,
                            ColorFloat  clrDefault,
                            float       fAlpha,
                            Matrix4x4   mat,
                            bool        bFlipX,
                            bool        bFlipY,
                            bool        bDoubleSided)
:   PKINIT(bFlipX),
    PKINIT(bFlipY),
    PKINIT(bDoubleSided)
{
    PKTRACE(Viewer_ViewQuad_ViewQuad);
    m_mat       = mat;
    m_fAlpha    = fAlpha;
    m_clr       = clrDefault;
    m_hTexture  = hTexObject;
}

void Viewer::ViewQuad::Draw(    const Matrix4x4& matVP,
                                const Viewer& oViewer,
                                const ShaderProgQuad& oShader) const
{
    PKTRACE(Viewer_ViewQuad_Draw);
    
    Matrix4x4 matMVP = m_mat;
    matMVP *= matVP;
    
    std::unique_ptr<GpuTextureList::UseTexture> roUseTex = oViewer.roGetTexture(m_hTexture);
    
    if (roUseTex->bFound())
    {
        PKTRACE(Viewer_ViewQuad_Draw_GlTexAvailable);
    }
        
    oShader.DrawQuad(   matMVP,
                        !roUseTex->bFound(),
                        m_clr,
                        m_fAlpha,
                        roUseTex->nGlTexHandle(),
                        m_bFlipX,
                        m_bFlipY,
                        m_bDoubleSided);
}
    
    
} // namespace PicoGK

