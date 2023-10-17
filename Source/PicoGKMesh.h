//
// SPDX-License-Identifier: Apache-2.0
//
// PicoGK ("peacock") is a compact software kernel for computational geometry,
// specifically for use in Computational Engineering Models (CEM).
//
// For more information, please visit https://picogk.org
//
// PicoGK is developed and maintained by LEAP 71 - © 2023 by LEAP 71
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

#ifndef PICOGKMESH_H_
#define PICOGKMESH_H_

#include "PicoGKTypes.h"

#include <memory>
#include <vector>
#include <cassert>

namespace PicoGK
{

class Mesh
{
public:
    PKSHAREDPTR(Mesh);
    
    inline Mesh()
    {
    }
    
    inline int32_t  nAddTriangle(   const Vector3& vecA,
                                    const Vector3& vecB,
                                    const Vector3& vecC)
    {
        int32_t a = nAddVertex(vecA);
        int32_t b = nAddVertex(vecB);
        int32_t c = nAddVertex(vecC);
        return nAddTriangle(Triangle(a,b,c));
    }
    
    inline int32_t nAddVertex(const Vector3& vecVertex)
    {
        m_oBBox.Include(vecVertex);
        m_oVertices.push_back(vecVertex);
        return nVertexCount() - 1;
    }
    
    inline int32_t nAddTriangle(const Triangle& sTri)
    {
        assert(sTri.A < nVertexCount());
        assert(sTri.B < nVertexCount());
        assert(sTri.C < nVertexCount());
        m_oTriangles.push_back(sTri);
        return nTriangleCount() - 1;
    }
    
    inline int32_t nTriangleCount() const
    {
        size_t nSize = m_oTriangles.size();
        assert(nSize < std::numeric_limits<int32_t>::max());
        return (int32_t) nSize;
    }
    
    inline int32_t nVertexCount() const
    {
        size_t nSize = m_oVertices.size();
        assert(nSize < std::numeric_limits<int32_t>::max());
        return (int32_t)nSize;
    }
    
    inline void GetVertex(  int32_t     nVertex,
                            Vector3*    pvecVertex) const
    {
        assert(nVertex < nVertexCount());
        *pvecVertex = m_oVertices.at(nVertex);
    }
    
    inline void GetTriangle(    int32_t     nTriangle,
                                Triangle*   psTriangle) const
    {
        assert(nTriangle < nTriangleCount());
        *psTriangle = m_oTriangles.at(nTriangle);
    }
    
    inline void GetTriangle(    int32_t  nTriangle,
                                Vector3* pvecA,
                                Vector3* pvecB,
                                Vector3* pvecC) const
    {
        assert(nTriangle < nTriangleCount());
        
        Triangle sTri = m_oTriangles.at(nTriangle);
        *pvecA = m_oVertices.at(sTri.A);
        *pvecB = m_oVertices.at(sTri.B);
        *pvecC = m_oVertices.at(sTri.C);
    }
    
    inline void GetBoundingBox(BBox3* poBBox)
    {
        *poBBox = m_oBBox;
    }
    
    bool bGetSurfaceNormal( const Vector3& vecSurfacePoint,
                            Vector3* pvecNormal)
    {
        Vector3 vec1(0,0,0);
        Vector3 vec2(0,0,0);
        Vector3 vec3(0,0,0);
        
        for (int32_t n=0; n<nTriangleCount(); n++)
        {
            GetTriangle(n, &vec1, &vec2, &vec3);
            if (bIsPointInTriangle(vecSurfacePoint, vec1, vec2, vec3, pvecNormal))
                return true;
        }
        
        return false;
    }
    
public:
    
    void* pVertexData() const
    {
        return (void*) m_oVertices.data();
    }
    
    void* pTriangleData() const
    {
        return (void*) m_oTriangles.data();
    }
    
protected:
    BBox3                  m_oBBox;
    std::vector<Vector3>   m_oVertices;
    std::vector<Triangle>  m_oTriangles;
    
    bool bIsPointInTriangle(    const Vector3& vecSurfacePoint,
                                const Vector3& vecVertex1,
                                const Vector3& vecVertex2,
                                const Vector3& vecVertex3,
                                Vector3* pvecNormal)
    {
        // Calculate the normal of the triangle
        *pvecNormal = (vecVertex2 - vecVertex1).vecCross(vecVertex3 - vecVertex1);
        pvecNormal->Normalize();

        // Calculate the vectors from the point to each vertex of the triangle
        Vector3 v0 = vecVertex1 - vecSurfacePoint;
        Vector3 v1 = vecVertex2 - vecSurfacePoint;
        Vector3 v2 = vecVertex3 - vecSurfacePoint;

        // Calculate the dot products of these vectors with the triangle's normal
        float dot01 = pvecNormal->fDot(v0.vecCross(v1));
        float dot02 = pvecNormal->fDot(v0.vecCross(v2));

        // If the dot products have the same sign, the point is inside the triangle
        return (dot01 >= 0 && dot02 >= 0) || (dot01 <= 0 && dot02 <= 0);
    }
};

} // namespace PicoGK

#endif
