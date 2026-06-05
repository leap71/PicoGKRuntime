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

#ifndef PICOGKVDBVOXELS_H_
#define PICOGKVDBVOXELS_H_

#include <openvdb/openvdb.h>
#include <openvdb/tools/Composite.h>
#include <openvdb/tools/MeshToVolume.h>
#include <openvdb/tools/VolumeToMesh.h>
#include <openvdb/tools/LevelSetRebuild.h>
#include <openvdb/tools/LevelSetFilter.h>
#include <openvdb/tools/Diagnostics.h>
#include <openvdb/tools/LevelSetMeasure.h>
#include <openvdb/tools/RayIntersector.h>
#include <openvdb/tools/LevelSetSphere.h>
#include <openvdb/tools/LevelSetTubes.h>
#include <openvdb/tools/LevelSetDilatedMesh.h>

#include "PicoGKMesh.h"
#include "PicoGKTrace.h"

using namespace openvdb;

#define PICOGK_VOXEL_DEFAULTNARROWBAND 3

namespace PicoGK
{

class Voxels
{

public:
    typedef std::shared_ptr<Voxels> Ptr;
    
    Voxels( VoxelSize oVoxelSize,
            int nNarrowBand)
    {
        m_nSdfNarrowBand    = nNarrowBand;
        m_roGrid            = FloatGrid::create(oVoxelSize.fToMM(nNarrowBand));
        m_roGrid->setGridClass(GRID_LEVEL_SET);
        m_roGrid->setTransform(openvdb::math::Transform::createLinearTransform(oVoxelSize));
    };
    
    Voxels( FloatGrid::Ptr roGrid,
            int nNarrowBand)
    {
        PKTRACE(Voxels_FromGridConstructor);
        
        if (!bHasValidPicoGKTransform(roGrid))
            throw std::invalid_argument("Source grid sent to Voxels constructor has complex transformation");
        
        m_nSdfNarrowBand    = nNarrowBand;
        m_roGrid            = roGrid;
        m_roGrid->setGridClass(GRID_LEVEL_SET);
    };
    
    Voxels(const Voxels& oSource)
    {
        PKTRACE(Voxels_CopyConstructor);
        
        m_roGrid = deepCopyTypedGrid<FloatGrid>(oSource.m_roGrid);
        m_nSdfNarrowBand = oSource.m_nSdfNarrowBand;
        
        assert(m_roGrid->getGridClass() == GRID_LEVEL_SET);
        assert(bHasValidPicoGKTransform(m_roGrid));
    };
    
    Voxels( VoxelSize oVoxSize,
            int32_t nNarrowBand,
            const Vector3& vecCenter,
            float fRadius)
    {
        PKTRACE(Voxels_SphereConstructor);
        
        m_roGrid = openvdb::tools::createLevelSetSphere<FloatGrid>( fRadius,
                                                                    openvdb::Vec3f( vecCenter.X,
                                                                                    vecCenter.Y,
                                                                                    vecCenter.Z),
                                                                    oVoxSize,
                                                                    nNarrowBand);
        
        m_nSdfNarrowBand = nNarrowBand;
        
        assert(m_roGrid->getGridClass() == GRID_LEVEL_SET);
        assert(bHasValidPicoGKTransform(m_roGrid));
    }
    
    Voxels( VoxelSize oVoxSize,
            int32_t nNarrowBand,
            const Vector3& vecStart,
            const Vector3& vecEnd,
            float fRadiusStart,
            float fRadiusEnd)
    {
        PKTRACE(Voxels_CapsuleConstructor);
        
        m_roGrid = openvdb::tools::createLevelSetTaperedCapsule<FloatGrid>( openvdb::Vec3f( vecStart.X,
                                                                                            vecStart.Y,
                                                                                            vecStart.Z),
                                                                            openvdb::Vec3f( vecEnd.X,
                                                                                            vecEnd.Y,
                                                                                            vecEnd.Z),
                                                                            fRadiusStart,
                                                                            fRadiusEnd,
                                                                            oVoxSize,
                                                                            nNarrowBand);
        
        m_nSdfNarrowBand = nNarrowBand;
        
        assert(m_roGrid->getGridClass() == GRID_LEVEL_SET);
        assert(bHasValidPicoGKTransform(m_roGrid));
    }
    
    Voxels( VoxelSize oVoxSize,
            int32_t nNarrowBand,
            const Mesh& oMesh,
            float fRadius)
    {
        m_roGrid = openvdb::tools::createLevelSetDilatedMesh<FloatGrid>(    (const std::vector<math::Vec3<float>>&) oMesh.vVertices(),
                                                                            (const std::vector<Vec3I>&)             oMesh.vTriangles(),
                                                                            fRadius,
                                                                            oVoxSize,
                                                                            nNarrowBand);
        
        m_nSdfNarrowBand = nNarrowBand;
        
        assert(m_roGrid->getGridClass() == GRID_LEVEL_SET);
        assert(bHasValidPicoGKTransform(m_roGrid));
    }

    ~Voxels()
    {
    }
    
    std::string strDiagnose() const
    {
        return tools::checkLevelSet<FloatGrid>(*m_roGrid);
    }
    
    int64_t nMemUsage() const
    {
        return sizeof(Voxels) + m_roGrid->memUsage();
    }
    
    bool bIsEmpty() const
    {
        PKTRACE(Voxels_bIsEmpty);
        
        if (m_roGrid->tree().empty())
            return true;
        
        // We check for values below 0, which are inside the object.
        // Technically 0 would be at the surface, but 0 values are often
        // left over after boolean operations, and an object that has
        // 0 values, but no value below 0 is fundamentally empty
        for (auto iter = m_roGrid->cbeginValueOn(); iter.test(); ++iter)
        {
            if (*iter < 0.0f)
            {
                return false;
            }
        }
        
        return true;
    }
    
    bool bIsEqual(const Voxels& oCompare) const
    {
        PKTRACE(Voxels_bIsEqual);
        
        if (m_roGrid->transform() != oCompare.m_roGrid->transform())
            return false;
        
        CoordBBox oBBoxThis = m_roGrid->evalActiveVoxelBoundingBox();
        CoordBBox oBBoxComp = oCompare.m_roGrid->evalActiveVoxelBoundingBox();
        
        int32_t iMinX = std::min(   oBBoxThis.min().x(),
                                    oBBoxComp.min().x());
        
        int32_t iMinY = std::min(   oBBoxThis.min().y(),
                                    oBBoxComp.min().y());
        
        int32_t iMinZ = std::min(   oBBoxThis.min().z(),
                                    oBBoxComp.min().z());
        
        int32_t iMaxX = std::max(   oBBoxThis.max().x(),
                                    oBBoxComp.max().x());
        
        int32_t iMaxY = std::max(   oBBoxThis.max().y(),
                                    oBBoxComp.max().y());
        
        int32_t iMaxZ = std::max(   oBBoxThis.max().z(),
                                    oBBoxComp.max().z());
        
        auto oThis = m_roGrid->getConstAccessor();
        auto oComp = oCompare.m_roGrid->getConstAccessor();
        
        for(int32_t x = iMinX; x <= iMaxX; x++)
        for(int32_t y = iMinY; y <= iMaxY; y++)
        for(int32_t z = iMinZ; z <= iMaxZ; z++)
        {
            openvdb::Coord xyz(x,y,z);
            
            bool bThisInside = (oThis.getValue(xyz) <= 0.0f);
            bool bCompInside = (oComp.getValue(xyz) <= 0.0f);
            
            if (bThisInside != bCompInside)
                return false;
        }
        
        return true;
     }

    void BoolAdd(const Voxels& oOther)
    {
        PKTRACE(Voxels_BoolAdd);
        
        FloatGrid::Ptr roOperand = deepCopyTypedGrid<FloatGrid>(oOther.m_roGrid);
        openvdb::tools::csgUnion(*m_roGrid, *roOperand);
        
        RebuildGrid();
    }

    void BoolSubtract(const Voxels& oOther)
    {
        PKTRACE(Voxels_BoolSubtract);
        
        FloatGrid::Ptr roOperand = deepCopyTypedGrid<FloatGrid>(oOther.m_roGrid);
        openvdb::tools::csgDifference(*m_roGrid, *roOperand);
        
        RebuildGrid();
    }

    void BoolIntersect(const Voxels& oOther)
    {
        PKTRACE(Voxels_BoolIntersect);
        
        FloatGrid::Ptr roOperand = deepCopyTypedGrid<FloatGrid>(oOther.m_roGrid);
        openvdb::tools::csgIntersection(*m_roGrid, *roOperand);
        
        RebuildGrid();
    }
    
    void Offset(float fSize)
    {
        PKTRACE(Voxels_Offset);
        
        // Create a clean level set with the extended size narrow band
        RebuildGrid(std::abs(fSize));
        
        openvdb::tools::LevelSetFilter<openvdb::FloatGrid> oFilter(*m_roGrid);
        
        // OpenVDB treats offsets as inwards
        oFilter.offset(-fSize);
        
        RebuildGrid();
    }
    
    void DoubleOffset(  float fSize1,
                        float fSize2)
    {
        PKTRACE(Voxels_DoubleOffset);
        
        // Create a clean level set with the extended size narrow band
        float fNarrowBandMM = std::max(std::abs(fSize1), std::abs(fSize2));
        RebuildGrid(fNarrowBandMM);
        
        openvdb::tools::LevelSetFilter<openvdb::FloatGrid> oFilter(*m_roGrid);
        
        // OpenVDB treats offsets as inwards
        oFilter.offset(-fSize1);
        oFilter.offset(-fSize2);
        
        RebuildGrid();
    }
    
    void TripleOffset(float fSize)
    {
        PKTRACE(Voxels_TripleOffset);
        
        // Create a clean level set with the extended size narrow band
        RebuildGrid(std::abs(fSize));
        
        openvdb::tools::LevelSetFilter<openvdb::FloatGrid> oFilter(*m_roGrid);
        
        // offset inwards first
        oFilter.offset(-fSize);
        
        // offset twice the size outwards next
        oFilter.offset(fSize * 2);
        
        // offset inwards again. Now we are back where we started
        // but have lost a lot of detail = smooth
        oFilter.offset(-fSize);
        
        RebuildGrid();
    }

    void RenderMesh(const Mesh& oMesh)
    {
        PKTRACE(Voxels_RenderMesh);
        
        FloatGrid::Ptr roVoxelized = roFloatGridFromMesh(   oMesh,
                                                            oVoxelSize(),
                                                            m_nSdfNarrowBand);
        
        openvdb::tools::csgUnion(*m_roGrid, *roVoxelized);
    }
    
    void RenderLattice(const Lattice& oLattice)
    {
        PKTRACE(Voxels_RenderLattice);
        
        auto oAccess = m_roGrid->getAccessor();
        
        for (auto roSphere : oLattice.oSpheres())
        {
            DoRenderLattice(&oAccess, *roSphere, oVoxelSize(), m_nSdfNarrowBand);
        }
        
        for (auto roBeam : oLattice.oBeams())
        {
            DoRenderLattice(&oAccess, *roBeam, oVoxelSize(), m_nSdfNarrowBand);
        }
    }
    
    void RenderImplicit(    const BBox3& oBBox,
                            PKPFnfSdf pfn)
    {
        PKTRACE(Voxels_RenderImplicit);
        
        auto oAccess = m_roGrid->getAccessor();
        
        Coord xyzMin = oVoxelSize().xyzToVoxels(oBBox.vecMin);
        Coord xyzMax = oVoxelSize().xyzToVoxels(oBBox.vecMax);
        
        for(int32_t x = xyzMin.X - m_nSdfNarrowBand; x <= xyzMax.X + m_nSdfNarrowBand; x++)
        for(int32_t y = xyzMin.Y - m_nSdfNarrowBand; y <= xyzMax.Y + m_nSdfNarrowBand; y++)
        for(int32_t z = xyzMin.Z - m_nSdfNarrowBand; z <= xyzMax.Z + m_nSdfNarrowBand; z++)
        {
            Vector3 vecSample = oVoxelSize().vecToMM(Coord(x,y,z));
            openvdb::Coord xyz(x,y,z);
            
            float fValue = std::min(    (*pfn)(&vecSample),
                                        oAccess.getValue(xyz));
            
            SetSdValue(&oAccess, xyz, fBackgroundMM(), fValue);
        }
        
        RebuildGrid();
    }
    
    void IntersectImplicit(PKPFnfSdf pfn)
    {
        PKTRACE(Voxels_IntersectImplicit);
        
        Voxels oVox(oVoxelSize(), fBackgroundMM());
        
        CoordBBox oBBox = m_roGrid->evalActiveVoxelBoundingBox();
        
        BBox3 oBBoxMM;
        oBBoxMM.vecMin.X = oVoxelSize().fToMM(oBBox.min().x());
        oBBoxMM.vecMin.Y = oVoxelSize().fToMM(oBBox.min().y());
        oBBoxMM.vecMin.Z = oVoxelSize().fToMM(oBBox.min().z());
        
        oBBoxMM.vecMax.X = oVoxelSize().fToMM(oBBox.max().x());
        oBBoxMM.vecMax.Y = oVoxelSize().fToMM(oBBox.max().y());
        oBBoxMM.vecMax.Z = oVoxelSize().fToMM(oBBox.max().z());
        
        oVox.RenderImplicit(oBBoxMM, pfn);
        
        // Swap out the grids, so we keep using the "nice"
        // implict grid, and use our grid just as the mask
        // Not sure if this is really necessary, but the
        // implicit grid is "perfect"
        m_roGrid.swap(oVox.m_roGrid);
        
        BoolIntersect(oVox);
    }

    Mesh::Ptr roAsMesh() const
    {
        PKTRACE(Voxels_roAsMesh);
        
        Mesh::Ptr roMesh = std::make_shared<Mesh>();
        
    	std::vector< openvdb::Vec3s > oPoints;
    	std::vector< openvdb::Vec3I > oTriangles;
    	std::vector< openvdb::Vec4I > oQuads;
		
        openvdb::tools::volumeToMesh<openvdb::FloatGrid>(*m_roGrid,
                                                         oPoints,
                                                         oTriangles,
                                                         oQuads,
                                                         0.0f,
                                                         0.0,
                                                         false);

        for (const openvdb::Vec4I oQuad : oQuads)
        {
            openvdb::Vec3I o1(oQuad[0], oQuad[1], oQuad[2]);
            oTriangles.push_back(o1);

            openvdb::Vec3I o2(oQuad[2], oQuad[3], oQuad[0]);
            oTriangles.push_back(o2);
        }
	    
        for (const openvdb::Vec3s& v : oPoints)
        {
            roMesh->nAddVertex(Vector3(v.x(), v.y(), v.z()));
        }

        for (const openvdb::Vec3I oTri : oTriangles)
        {
            roMesh->nAddTriangle(   PicoGK::Triangle(   oTri[2],
                                                        oTri[1],
                                                        oTri[0]));
        }

        return roMesh;
    }
    
    void ProjectZSliceDn(   float fZStart,
                            float fZEnd)
    {
        assert(fZStart > fZEnd);
        
        int32_t iZStart = oVoxelSize().iToVoxels(fZStart);
        int32_t iZEnd   = oVoxelSize().iToVoxels(fZEnd);
        CoordBBox oBBox = m_roGrid->evalActiveVoxelBoundingBox();
        
        auto oAccess = m_roGrid->getAccessor();
        
        for(int32_t x = oBBox.min().x(); x <= oBBox.max().x(); x++)
        for(int32_t y = oBBox.min().y(); y <= oBBox.max().y(); y++)
        for(int32_t z = iZStart; z > iZEnd; z--)
        {
            
            openvdb::Coord xyz(x,y,z);
            openvdb::Coord xyzUnder(x,y,z-1);
            
            float fValue = std::min(    oAccess.getValue(xyzUnder),
                                        oAccess.getValue(xyz));
            
            SetSdValue(&oAccess, xyzUnder, fBackgroundMM(), fValue);
        }
        
        // Close the last slice, and update the background
        for(int32_t z = iZEnd; z > iZEnd - (int) (0.5f + m_roGrid->background()); z--)
        {
            for(int32_t x = oBBox.min().x(); x <= oBBox.max().x(); x++)
            for(int32_t y = oBBox.min().y(); y <= oBBox.max().y(); y++)
            {
                openvdb::Coord xyz(x,y,z);
                openvdb::Coord xyzUnder(x,y,z-1);
                
                float fValue = (oAccess.getValue(xyz) + oAccess.getValue(xyzUnder)) / 2.0f;
                SetSdValue(&oAccess, xyz, fBackgroundMM(), fValue);
            }
        }
        
        RebuildGrid();
    }
    
    void ProjectZSliceUp(   float fZStart,
                            float fZEnd)
    {
        assert(fZStart < fZEnd);
        
        int32_t iZStart = oVoxelSize().iToVoxels(fZStart);
        int32_t iZEnd   = oVoxelSize().iToVoxels(fZEnd);
        CoordBBox oBBox = m_roGrid->evalActiveVoxelBoundingBox();
        
        auto oAccess = m_roGrid->getAccessor();
        
        for(int32_t x = oBBox.min().x(); x <= oBBox.max().x(); x++)
        for(int32_t y = oBBox.min().y(); y <= oBBox.max().y(); y++)
        for(int32_t z = iZStart; z < iZEnd; z++)
        {
            openvdb::Coord xyz(x,y,z);
            openvdb::Coord xyzOver(x,y,z+1);
            
            float fValue = std::min(    oAccess.getValue(xyzOver),
                                        oAccess.getValue(xyz));
            
            SetSdValue(&oAccess, xyzOver, fBackgroundMM(), fValue);
        }
        
        // Close the last slice, and update the background
        for(int32_t z = iZEnd; z < iZEnd + (int) (0.5f + m_roGrid->background()); z++)
        {
            for(int32_t x = oBBox.min().x(); x <= oBBox.max().x(); x++)
            for(int32_t y = oBBox.min().y(); y <= oBBox.max().y(); y++)
            {
                openvdb::Coord xyz(x,y,z);
                openvdb::Coord xyzOver(x,y,z+1);
                
                float fValue = (oAccess.getValue(xyz) + oAccess.getValue(xyzOver)) / 2.0f;
                SetSdValue(&oAccess, xyz, fBackgroundMM(), fValue);
            }
        }
        
        RebuildGrid();
    }

    void ProjectZSlice( float fZStart,
                        float fZEnd)
    {
        if (fZStart > fZEnd)
            ProjectZSliceDn(fZStart, fZEnd);
        else
            ProjectZSliceUp(fZStart, fZEnd);
    }
    
    float fCalculateVolume()
    {
        PKTRACE(Voxels_fCalculateVolume);
        
        if (m_roGrid->tree().empty())
        {
            return 0.0f;
        }
        
        RebuildGrid();
        
        return openvdb::tools::levelSetVolume(*m_roGrid, true);
    }
    
    inline bool bIsInside(Vector3 vecTest)
    {
        auto oAccess = m_roGrid->getConstAccessor();
    
        openvdb::Coord xyz( oVoxelSize().iToVoxels(vecTest.X),
                            oVoxelSize().iToVoxels(vecTest.Y),
                            oVoxelSize().iToVoxels(vecTest.Z));
        
        return (oAccess.getValue(xyz) <= 0.0f);
    }
    
    inline void GetSurfaceNormal(   Vector3 vecPt,
                                    Vector3* pvecNormal)
    {
        math::GradStencil oStencil(*m_roGrid);
        Coord xyz = oVoxelSize().xyzToVoxels(vecPt);
    
        oStencil.moveTo(    openvdb::Coord( xyz.X,
                                            xyz.Y,
                                            xyz.Z));
        
        auto vecGradient = oStencil.gradient();
        vecGradient.normalize();
        
        pvecNormal->X = vecGradient.x();
        pvecNormal->Y = vecGradient.y();
        pvecNormal->Z = vecGradient.z();
    }
    
    inline bool bFindClosestPointOnSurface( Vector3 vecSearch,
                                            Vector3* pvecSurfacePoint)
    {
        Coord xyzSearch = oVoxelSize().xyzToVoxels(vecSearch);
        
        CoordBBox oBBox = m_roGrid->evalActiveVoxelBoundingBox();
        oBBox.expand(openvdb::Coord(xyzSearch.X, xyzSearch.Y, xyzSearch.Z));
        // Add the search point to the BBox and expand a little to avoid
        // stopping too soon
        
        auto oAccess    = m_roGrid->getConstAccessor();
        
        int32_t iMaxSearchRadius = (int32_t) std::ceil(sqrt(
                    oBBox.extents().x() * oBBox.extents().x() +
                    oBBox.extents().y() * oBBox.extents().y() +
                    oBBox.extents().z() * oBBox.extents().z()));
        
        bool bStartInside = oAccess.getValue(   
                                openvdb::Coord( xyzSearch.X,
                                                xyzSearch.Y,
                                                xyzSearch.Z)) <= 0.0f;
        
        for (int32_t iR=0; iR<iMaxSearchRadius; iR++)
        {
            bool bOutsideActiveBounds = true;
            Coord xyzSurfacePoint;
            
            if (bBresenhamSphereHitTest(    bStartInside,
                                            xyzSearch,
                                            iR,
                                            oAccess,
                                            oBBox,
                                            &xyzSurfacePoint,
                                            &bOutsideActiveBounds))
            {
                *pvecSurfacePoint = oVoxelSize().vecToMM(xyzSurfacePoint);
                return true;
            }
            
            if (bOutsideActiveBounds)
                return false;
        }
        
        return false;
    }
    
    inline bool bRayCastToSurface(  const Vector3& vecSearch,
                                    const Vector3& vecDirection,
                                    Vector3* pvecSurfacePoint)
    {
        tools::LevelSetRayIntersector oIntersector(*m_roGrid);
        math::Ray<Real> oRay(   Vec3f(  oVoxelSize().fToVoxels(vecSearch.X),
                                        oVoxelSize().fToVoxels(vecSearch.Y),
                                        oVoxelSize().fToVoxels(vecSearch.Z)),
                                Vec3f(  vecDirection.X,
                                        vecDirection.Y,
                                        vecDirection.Z));
        
        math::Vec3<Real> xyz;
        math::Vec3<Real> vecNormal;
        
        if (oIntersector.intersectsIS(oRay, xyz))
        {
            *pvecSurfacePoint = oVoxelSize().vecToMM( Coord(    xyz.x(),
                                                                xyz.y(),
                                                                xyz.z()));
            return true;
        }

        // If the ray reaches its maximum range without hitting a filled voxel,
        // return false to indicate that there was no intersection.
            
        return false;
    }
    
    void GetVoxelDimensions(    int32_t* pnXMin,
                                int32_t* pnYMin,
                                int32_t* pnZMin,
                                int32_t* pnXSize,
                                int32_t* pnYSize,
                                int32_t* pnZSize) const
    {
        CoordBBox oBBox = m_roGrid->evalActiveVoxelBoundingBox();
        
        *pnXMin     = oBBox.min().x();
        *pnYMin     = oBBox.min().y();
        *pnZMin     = oBBox.min().z();
        *pnXSize    = oBBox.extents().x();
        *pnYSize    = oBBox.extents().y();
        *pnZSize    = oBBox.extents().z();
    }
    
    void GetZSlice( int32_t nZSlice,
                    float* pfBuffer)
    {
        CoordBBox oBBox = m_roGrid->evalActiveVoxelBoundingBox();
        openvdb::Coord xyz(0, 0, nZSlice + oBBox.min().z());
        
        auto oAccess = m_roGrid->getConstAccessor();
        
        int32_t n=0;
        for (xyz.y()=oBBox.max().y(); xyz.y()>=oBBox.min().y(); xyz.y()--)
        for (xyz.x()=oBBox.min().x(); xyz.x()<=oBBox.max().x(); xyz.x()++)
        {
            pfBuffer[n] = oAccess.getValue(xyz);
            n++;
        }
    }
    
    void GetXSlice( int32_t nXSlice,
                    float* pfBuffer)
    {
        CoordBBox oBBox = m_roGrid->evalActiveVoxelBoundingBox();
        openvdb::Coord xyz(nXSlice + oBBox.min().x(), 0, 0);
        
        auto oAccess = m_roGrid->getConstAccessor();
        
        int32_t n=0;
        for (xyz.z()=oBBox.max().z(); xyz.z()>=oBBox.min().z(); xyz.z()--)
        for (xyz.y()=oBBox.min().y(); xyz.y()<=oBBox.max().y(); xyz.y()++)
        {
            pfBuffer[n] = oAccess.getValue(xyz);
            n++;
        }
    }
    
    void GetYSlice( int32_t nYSlice,
                    float* pfBuffer)
    {
        CoordBBox oBBox = m_roGrid->evalActiveVoxelBoundingBox();
        openvdb::Coord xyz(0, nYSlice + oBBox.min().y(), 0);
        
        auto oAccess = m_roGrid->getConstAccessor();
        
        int32_t n=0;
        for (xyz.z()=oBBox.max().z(); xyz.z()>=oBBox.min().z(); xyz.z()--)
        for (xyz.x()=oBBox.min().x(); xyz.x()<=oBBox.max().x(); xyz.x()++)
        {
            pfBuffer[n] = oAccess.getValue(xyz);
            n++;
        }
    }
    
    void GetInterpolatedZSlice( float fZSlice,
                                float* pfBuffer)
    {
        CoordBBox oBBox = m_roGrid->evalActiveVoxelBoundingBox();
        
        auto oAccess = m_roGrid->getConstAccessor();
        
        openvdb::Vec3R vec(0, 0, fZSlice + oBBox.min().z());
        openvdb::tools::BoxSampler oSampler;
        
        int32_t n=0;
        for (vec.y()=oBBox.max().y(); vec.y()>=oBBox.min().y(); vec.y()--)
        for (vec.x()=oBBox.min().x(); vec.x()<=oBBox.max().x(); vec.x()++)
        {
            pfBuffer[n] = oSampler.sample(oAccess, vec);
            n++;
        }
    }
    
    FloatGrid::Ptr roVdbGrid() const 	{return m_roGrid;}
    
    inline float fBackgroundMM() const  {return m_roGrid->background();}
    
    inline VoxelSize oVoxelSize() const
    {
        assert(bHasValidPicoGKTransform(m_roGrid));
        return VoxelSize(m_roGrid->voxelSize().x());
    }
    
protected:
    FloatGrid::Ptr      m_roGrid;
    int32_t             m_nSdfNarrowBand;
    
    bool bLevelSetNeedsRebuild() const
    {
        return strDiagnose().length() > 0;
    }
    
    /// Rebuilds the grid after possibly destructive changes to the signed distances
    void RebuildGrid(float fDistanceMM = 0.0f)
    {
        /// Disabled until we determine it is necessary
        return;
        
        PKTRACE(Voxels_RebuildGrid);
        
        /// TODO — this is probably triggering way too often.
        /// We will need a careful analysis of when this is necessary
        /// Also, we need to carefully evaluate what it does to internal cavities
        
        if ((fDistanceMM > 0.0f) || (bLevelSetNeedsRebuild()))
        {
            std::cerr << "- Rebuilding Level Set '" << strDiagnose() << " ... ";
            
            float fHalfVal = oVoxelSize().fToVoxels(fDistanceMM);
            
            if (fHalfVal < m_nSdfNarrowBand)
                fHalfVal = m_nSdfNarrowBand;
            
            m_roGrid = openvdb::tools::levelSetRebuild(*m_roGrid, 0.0f, fHalfVal, fHalfVal);
            
            std::cerr << "Done - Result: '" << strDiagnose() << "'\n";
        }
    }
    
    template<class TAccessor, class TLatticeBeam>
    static void DoRenderLattice(    TAccessor* poAccess,
                                    const TLatticeBeam& oLattice,
                                    VoxelSize oVoxelSize,
                                    int32_t nNarrowBand)
    {
        Vector3 vecMin(oLattice.vecMin());
        Vector3 vecMax(oLattice.vecMax());
            
        Coord xyzMin = oVoxelSize.xyzToVoxels(vecMin);
        Coord xyzMax = oVoxelSize.xyzToVoxels(vecMax);
        
        float fBackground = oVoxelSize.fToMM(nNarrowBand);
        
        for(int32_t x = xyzMin.X - nNarrowBand; x <= xyzMax.X + nNarrowBand; x++)
        for(int32_t y = xyzMin.Y - nNarrowBand; y <= xyzMax.Y + nNarrowBand; y++)
        for(int32_t z = xyzMin.Z - nNarrowBand; z <= xyzMax.Z + nNarrowBand; z++)
        {
            openvdb::Coord xyz(x,y,z);
            
            Vector3 vecSample = oVoxelSize.vecToMM(Coord(x,y,z));
            
            // Boolean add to existing value, if one exists
            float fValue = std::min(    oLattice.fSdValue(vecSample),
                                        poAccess->getValue(xyz));
            
            SetSdValue(poAccess, xyz, fBackground, fValue);
        }
    }
    
    static FloatGrid::Ptr roFloatGridFromMesh(  const Mesh& oMesh,
                                                VoxelSize oVoxelSize,
                                                int32_t nHalfWidth)
    {
        std::vector< openvdb::Vec3s >   oVertices;
        std::vector< openvdb::Vec3I >   oTriangles;
        
        for (int32_t n=0; n<oMesh.nVertexCount(); n++)
        {
            Vector3 v(0.0f,0.0f,0.0f);
            oMesh.GetVertex(n, (Vector3*)&v);
            oVertices.push_back(openvdb::Vec3s( v.X,
                                                v.Y,
                                                v.Z));
        }
        
        for (int32_t n=0; n<oMesh.nTriangleCount(); n++)
        {
            Triangle t;
            oMesh.GetTriangle(n, &t);
            oTriangles.push_back(openvdb::Vec3I(    (unsigned int) t.A,
                                                    (unsigned int) t.B,
                                                    (unsigned int) t.C));
        }

        openvdb::math::Transform::Ptr roTransform
            = openvdb::math::Transform::createLinearTransform(oVoxelSize);

        return openvdb::tools::meshToLevelSet<openvdb::FloatGrid>(  *roTransform,
                                                                    oVertices,
                                                                    oTriangles,
                                                                    nHalfWidth);
    }
    
    template <class TAccessor>
    inline bool bBresenhamSphereHitTest(    bool bReferenceInside,
                                            Coord xyzCenter,
                                            int iRadius,
                                            const TAccessor& oAccess,
                                            const openvdb::CoordBBox& oBB,
                                            Coord* pxyzPoint,
                                            bool* pbOutsideActiveBounds)
    {
        *pbOutsideActiveBounds = true;
        
        openvdb::Coord xyz;
        
        for (xyz.z() = xyzCenter.Z - iRadius; xyz.z() <= xyzCenter.Z + iRadius; xyz.z()++)
        {
            int r2 = iRadius * iRadius;
            for (xyz.y() = xyzCenter.Y - iRadius; xyz.y() <= xyzCenter.Y + iRadius; xyz.y()++)
            {
                for (xyz.x() = xyzCenter.X - iRadius; xyz.x() <= xyzCenter.X + iRadius; xyz.x()++)
                {
                    if (!oBB.isInside(xyz))
                        continue; // don't search outside active voxel area
                    
                    *pbOutsideActiveBounds = false; // we are still inside the bounds
                    
                    int dx = xyz.x() - xyzCenter.X;
                    int dy = xyz.y() - xyzCenter.Y;
                    int dz = xyz.z() - xyzCenter.Z;
                    if (dx * dx + dy * dy + dz * dz <= r2)
                    {
                        bool bInside = oAccess.getValue(xyz) <= 0.0f;
                        if (bInside != bReferenceInside)
                        {
                            pxyzPoint->X = xyz.x();
                            pxyzPoint->Y = xyz.y();
                            pxyzPoint->Z = xyz.z();
                            return true;
                        }
                    }
                }
            }
        }
        
        return false;
    }
    
    static void SetSdValue( FloatGrid::Accessor* poAccess,
                            openvdb::Coord xyz,
                            float fBackground,
                            float fValue)
    {
        poAccess->setValue(xyz, std::clamp( fValue,
                                            -fBackground,
                                            fBackground));
        
        if (std::abs(fValue) >= fBackground)
            poAccess->setValueOff(xyz);
    }
    
    static bool bHasValidPicoGKTransform(const FloatGrid::Ptr roGrid)
    {
        if (!roGrid->transform().isLinear())
            return false;
            
        if (roGrid->voxelSize().x() != roGrid->voxelSize().y())
            return false;
        
        if (roGrid->voxelSize().x() != roGrid->voxelSize().z())
            return false;
        
        return true;
    }
    
};

} // PicoGK namespace

#endif // PICOGKVDBVOXELS_H_
