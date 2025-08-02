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
#include "PicoGKGLDebug.h"

namespace PicoGK
{

Viewer::Group::ViewPolyLine::ViewPolyLine(const PicoGK::PolyLine& oPoly)
: m_oVertexBuffer(oPoly.vVertices())
{
    m_clrLine       = oPoly.clrLines();
    m_oBBox         = oPoly.oBBox();
}

void Viewer::Group::ViewPolyLine::Draw( const ShaderProgMeshPoly& oShader,
                                        const Material& oMaterial,
                                        const Matrix4x4& mat)
{
    if (m_oVertexBuffer.bIsEmpty())
        return;
    
    oShader.SetValues(  mat,
                        m_clrLine);
    
    class GlVertexBuffer<Vector3>::Bind oBind(m_oVertexBuffer);
    
    // Specify the attribute location and format
    glEnableVertexAttribArray(oShader.nAttribPosition());
    glVertexAttribPointer(  oShader.nAttribPosition(),
                            m_oVertexBuffer.nComponents(),
                            m_oVertexBuffer.eType(),
                            m_oVertexBuffer.bNormalized(),
                            m_oVertexBuffer.nStride(),
                            nullptr);
    
    glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(m_oVertexBuffer.nVertexCount()));
    
    CHECKGLERRORS;
}

void Viewer::Group::Draw(   const Matrix4x4& matModelTrans,
                            const ShaderProgMeshPoly& oShader)
{
    if (!m_bVisible)
        return;
    
    Matrix4x4 matMult = matModelTrans;
    matMult *= m_mat;
    
    for (auto& oMapEntry : m_oViewPolyLines)
    {
        oMapEntry.second->Draw(oShader, m_sMaterial, matMult);
    }
    
    for (auto& oMapEntry : m_oViewMeshes)
    {
        oMapEntry.second->Draw(oShader, m_sMaterial, matMult);
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

Viewer::Group::ViewMesh::ViewMesh(const Mesh& oMesh)
{
    if (oMesh.nVertexCount() == 0)
        return;
    
    m_nTriangleCount = oMesh.nTriangleCount();
    
    if (m_nTriangleCount == 0)
        return;
    
    glGenVertexArrays(1, &sGLParams.nVertexArray);
    glBindVertexArray(sGLParams.nVertexArray);

    glGenBuffers(1, &sGLParams.nArrayBuffer);

    glBindBuffer(   GL_ARRAY_BUFFER,
                    sGLParams.nArrayBuffer);
        
    glBufferData(   GL_ARRAY_BUFFER,
                    oMesh.nVertexCount() * sizeof(Vector3),
                    oMesh.pVertexData(),
                    GL_STATIC_DRAW);
    
    glGenBuffers(1, &sGLParams.nElementArrayBuffer);
    
    glBindBuffer(   GL_ELEMENT_ARRAY_BUFFER,
                    sGLParams.nElementArrayBuffer);
        
    glBufferData(   GL_ELEMENT_ARRAY_BUFFER,
                    m_nTriangleCount * sizeof(Triangle),
                    oMesh.pTriangleData(),
                    GL_STATIC_DRAW);
    
    CHECKGLERRORS;
}

void Viewer::Group::ViewMesh::Draw( const ShaderProgMeshPoly& oShader,
                                    const Material& sMaterial,
                                    const Matrix4x4& mat)
{
    if (m_nTriangleCount == 0)
        return;
    
    oShader.SetValues(  mat,
                        sMaterial.clr,
                        sMaterial.fMetallic,
                        sMaterial.fRoughness);
                
    glBindVertexArray(sGLParams.nVertexArray);
            
    glBindBuffer(   GL_ARRAY_BUFFER,
                    sGLParams.nArrayBuffer);
    
    glEnableVertexAttribArray(oShader.nAttribPosition());
    
    glVertexAttribPointer(  oShader.nAttribPosition(),
                            3, GL_FLOAT, GL_FALSE,
                            sizeof(Vector3),
                            nullptr);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sGLParams.nElementArrayBuffer);
    glDrawElements(GL_TRIANGLES, m_nTriangleCount * 3, GL_UNSIGNED_INT, nullptr);

    CHECKGLERRORS;
}

} // namespace PicoGK

