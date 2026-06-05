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
#include "PicoGKTypes.h"
#include "PicoGK.h"

#include "PicoGKLibraryMgr.h"
#include "PicoGKGLViewer.h"
#include "PicoGKGLViewerManager.h"

#include "PicoGKTrace.h"

using namespace PicoGK;

void SafeCopyInfoString(const std::string s, char psz[PKINFOSTRINGLEN])
{
#ifdef _WINDOWS
    strncpy_s(psz, PKINFOSTRINGLEN-1, s.c_str(), s.length());
#else
    strncpy(psz, s.c_str(), PKINFOSTRINGLEN-1);
#endif
    psz[PKINFOSTRINGLEN-1] = 0;
}

PICOGK_API void Library_GetName(char psz[PKINFOSTRINGLEN])
{
   SafeCopyInfoString(Library::oLib().strName(), psz);
}

PICOGK_API void Library_GetVersion(char psz[PKINFOSTRINGLEN])
{
    SafeCopyInfoString(Library::oLib().strVersion(), psz);
}

PICOGK_API void Library_GetBuildInfo(char psz[PKINFOSTRINGLEN])
{
    SafeCopyInfoString(Library::oLib().strBuildInfo(), psz);
}

PICOGK_API PKINSTANCE Library_hCreateInstance(float fVoxelSizeMM)
{
    return Library::oLib().nCreateInstance(fVoxelSizeMM);
}

PICOGK_API void Library_DestroyInstance(PKINSTANCE hThis)
{
    PKTRACE(Library_DestroyInstance);
    Library::oLib().DestroyInstance(hThis);
}

PICOGK_API int64_t Library_nTotalMemUsage(PKINSTANCE hThis)
{
    PKTRACE(Library_nTotalMemUsage);
    return Library::oLib().roGetInstance(hThis)->nMemUsage();
}

PICOGK_API int64_t Library_nMeshesMemUsage(PKINSTANCE hThis)
{
    PKTRACE(Library_nMeshesMemUsage);
    return Library::oLib().roGetInstance(hThis)->m_oMeshes.nMemUsage();
}

PICOGK_API int64_t Library_nLatticesMemUsage(PKINSTANCE hThis)
{
    PKTRACE(Library_nLatticesMemUsage);
    return Library::oLib().roGetInstance(hThis)->m_oLattices.nMemUsage();
}

PICOGK_API int64_t Library_nPolyLinesMemUsage(PKINSTANCE hThis)
{
    PKTRACE(Library_nPolyLinesMemUsage);
    return Library::oLib().roGetInstance(hThis)->m_oPolyLines.nMemUsage();
}

PICOGK_API int64_t Library_nVoxelsMemUsage(PKINSTANCE hThis)
{
    PKTRACE(Library_nVoxelsMemUsage);
    return Library::oLib().roGetInstance(hThis)->m_oVoxels.nMemUsage();
}

PICOGK_API int64_t Library_nVdbFilesMemUsage(PKINSTANCE hThis)
{
    PKTRACE(Library_nVdbFilesMemUsage);
    return Library::oLib().roGetInstance(hThis)->m_oVdbFiles.nMemUsage();
}

PICOGK_API int64_t Library_nScalarFieldsMemUsage(PKINSTANCE hThis)
{
    PKTRACE(Library_nScalarFieldsMemUsage);
    return Library::oLib().roGetInstance(hThis)->m_oScalarFields.nMemUsage();
}

PICOGK_API int64_t Library_nVectorFieldsMemUsage(PKINSTANCE hThis)
{
    PKTRACE(Library_nVectorFieldsMemUsage);
    return Library::oLib().roGetInstance(hThis)->m_oVectorFields.nMemUsage();
}

PICOGK_API int64_t Library_nVdbMetasMemUsage(PKINSTANCE hThis)
{
    PKTRACE(Library_nVdbMetasMemUsage);
    return Library::oLib().roGetInstance(hThis)->m_oVdbMetas.nMemUsage();
}

PICOGK_API int64_t Library_nMeshesAllocated(PKINSTANCE hThis)
{
    PKTRACE(Library_nMeshesAllocated);
    return Library::oLib().roGetInstance(hThis)->m_oMeshes.nAllocatedCount();
}

PICOGK_API int64_t Library_nLatticesAllocated(PKINSTANCE hThis)
{
    PKTRACE(Library_nLatticesAllocated);
    return Library::oLib().roGetInstance(hThis)->m_oLattices.nAllocatedCount();
}

PICOGK_API int64_t Library_nPolyLinesAllocated(PKINSTANCE hThis)
{
    PKTRACE(Library_nPolyLinesAllocated);
    return Library::oLib().roGetInstance(hThis)->m_oPolyLines.nAllocatedCount();
}

PICOGK_API int64_t Library_nVoxelsAllocated(PKINSTANCE hThis)
{
    PKTRACE(Library_nVoxelsAllocated);
    return Library::oLib().roGetInstance(hThis)->m_oVoxels.nAllocatedCount();
}

PICOGK_API int64_t Library_nVdbFilesAllocated(PKINSTANCE hThis)
{
    PKTRACE(Library_nVdbFilesAllocated);
    return Library::oLib().roGetInstance(hThis)->m_oVdbFiles.nAllocatedCount();
}

PICOGK_API int64_t Library_nScalarFieldsAllocated(PKINSTANCE hThis)
{
    PKTRACE(Library_nScalarFieldsAllocated);
    return Library::oLib().roGetInstance(hThis)->m_oScalarFields.nAllocatedCount();
}

PICOGK_API int64_t Library_nVectorFieldsAllocated(PKINSTANCE hThis)
{
    PKTRACE(Library_nVectorFieldsAllocated);
    return Library::oLib().roGetInstance(hThis)->m_oVectorFields.nAllocatedCount();
}

PICOGK_API int64_t Library_nVdbMetasAllocated(PKINSTANCE hThis)
{
    PKTRACE(Library_nVdbMetasAllocated);
    return Library::oLib().roGetInstance(hThis)->m_oVdbMetas.nAllocatedCount();
}

PICOGK_API void Library_VoxelsToMm( PKINSTANCE hLib,
                                    const PKVector3* pvecVoxelCoordinate,
                                    PKVector3* pvecMmCoordinate)
{
    PKTRACE(Library_VoxelsToMm);
    
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    VoxelSize oVoxelSize(roLib->fVoxelSizeMM());
    
    pvecMmCoordinate->X = oVoxelSize.fToMM(pvecVoxelCoordinate->X);
    pvecMmCoordinate->Y = oVoxelSize.fToMM(pvecVoxelCoordinate->Y);
    pvecMmCoordinate->Z = oVoxelSize.fToMM(pvecVoxelCoordinate->Z);
}

PICOGK_API void Library_MmToVoxels( PKINSTANCE hLib,
                                    const PKVector3* pvecMmCoordinate,
                                    PKVector3* pvecVoxelCoordinate)
{
    PKTRACE(Library_MmToVoxels);
    
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    VoxelSize oVoxelSize(roLib->fVoxelSizeMM());
    
    pvecVoxelCoordinate->X = oVoxelSize.iToVoxels(pvecMmCoordinate->X);
    pvecVoxelCoordinate->Y = oVoxelSize.iToVoxels(pvecMmCoordinate->Y);
    pvecVoxelCoordinate->Z = oVoxelSize.iToVoxels(pvecMmCoordinate->Z);
}

PICOGK_API PKMESH Mesh_hCreate(PKINSTANCE hLib)
{
    PKTRACE(Mesh_hCreate);
    
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oMeshes.hAdd(std::make_shared<Mesh>());
}

PICOGK_API PKMESH Mesh_hCreateFromVoxels(   PKINSTANCE hLib,
                                            PKVOXELS hVoxels)
{
    PKTRACE(Mesh_hCreateFromVoxels);
    
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    
    Voxels::Ptr roVoxels = roLib->m_oVoxels.roGet(hVoxels);
    return roLib->m_oMeshes.hAdd(roVoxels->roAsMesh());
}

PICOGK_API bool Mesh_bIsValid(  PKINSTANCE hLib,
                                PKMESH hThis)
{
    PKTRACE(Mesh_bIsValid);
    
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oMeshes.bIsValid(hThis);
}

PICOGK_API void Mesh_Destroy(   PKINSTANCE hLib,
                                PKMESH hThis)
{
    // Library may have been cleaned up already, whereas the object
    // could still be alive due to delayed gargabe collector
    if (Library::oLib().bIsValid(hLib))
    {
        Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
        roLib->m_oMeshes.bDestroy(hThis);
    }
}

PICOGK_API int64_t Mesh_nMemUsage(  PKINSTANCE hLib,
                                    PKMESH hThis)
{
    PKTRACE(Mesh_nMemUsage);
    
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oMeshes.roGet(hThis)->nMemUsage();
}

PICOGK_API int32_t Mesh_nAddVertex( PKINSTANCE hLib,
                                    PKMESH hThis,
                                    const Vector3* pvecVertex)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oMeshes.roGet(hThis)->nAddVertex(*pvecVertex);
}
    
PICOGK_API void Mesh_GetVertex( PKINSTANCE hLib,
                                PKMESH      hThis,
                                int32_t     nVertex,
                                PKVector3*    pvecVertex)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    roLib->m_oMeshes.roGet(hThis)->GetVertex(nVertex, pvecVertex);
}

PICOGK_API int32_t Mesh_nVertexCount(   PKINSTANCE hLib,
                                        PKMESH hThis)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oMeshes.roGet(hThis)->nVertexCount();
}

PICOGK_API int32_t Mesh_nAddTriangle(   PKINSTANCE hLib,
                                        PKMESH hThis,
                                        const PKTriangle* psTri)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oMeshes.roGet(hThis)->nAddTriangle(*psTri);
}

PICOGK_API void Mesh_GetTriangle(   PKINSTANCE hLib,
                                    PKMESH hThis,
                                    int32_t nTriangle,
                                    PKTriangle* psTri)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oMeshes.roGet(hThis)->GetTriangle( nTriangle, psTri);
}

PICOGK_API void Mesh_GetTriangleV(  PKINSTANCE hLib,
                                    PKMESH      hThis,
                                    int32_t     nTriangle,
                                    PKVector3*  pvecA,
                                    PKVector3*  pvecB,
                                    PKVector3*  pvecC)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    roLib->m_oMeshes.roGet(hThis)->GetTriangle(
                                nTriangle,
                                pvecA,
                                pvecB,
                                pvecC);
}

PICOGK_API void Mesh_GetBoundingBox(    PKINSTANCE hLib,
                                        PKMESH hThis,
                                        PKBBox3* poBox)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    *poBox = roLib->m_oMeshes.roGet(hThis)->oBBox();
}

PICOGK_API int32_t Mesh_nTriangleCount( PKINSTANCE hLib,
                                        PKMESH hThis)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oMeshes.roGet(hThis)->nTriangleCount();
}

PICOGK_API PKLATTICE Lattice_hCreate(PKINSTANCE hLib)
{
    PKTRACE(Lattice_hCreate);
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oLattices.hAdd(std::make_shared<Lattice>());
}

PICOGK_API bool Lattice_bIsValid(   PKINSTANCE hLib,
                                    PKLATTICE hThis)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oLattices.bIsValid(hThis);
}

PICOGK_API void Lattice_Destroy(    PKINSTANCE hLib,
                                    PKLATTICE hThis)
{
    // Library may have been cleaned up already, whereas the object
    // could still be alive due to delayed gargabe collector
    if (Library::oLib().bIsValid(hLib))
    {
        Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
        roLib->m_oLattices.bDestroy(hThis);
    }
}

PICOGK_API int64_t Lattice_nMemUsage(   PKINSTANCE hLib,
                                        PKLATTICE hThis)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    
    Lattice::Ptr* proThis = (Lattice::Ptr*) hThis;
    return roLib->m_oLattices.roGet(hThis)->nMemUsage();
}

PICOGK_API void Lattice_AddSphere(  PKINSTANCE hLib,
                                    PKLATTICE hThis,
                                    const PKVector3* vecCenter,
                                    float fRadius)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    roLib->m_oLattices.roGet(hThis)
        ->AddSphere(     *vecCenter,
                         fRadius);
}

PICOGK_API void Lattice_AddBeam(    PKINSTANCE hLib,
                                    PKLATTICE hThis,
                                    const PKVector3* pvecA,
                                    const PKVector3* pvecB,
                                    float fRadiusA,
                                    float fRadiusB,
                                    bool  bRoundCap)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    roLib->m_oLattices.roGet(hThis)
        ->AddBeam(  *pvecA,
                    *pvecB,
                    fRadiusA,
                    fRadiusB,
                    bRoundCap);
}

PICOGK_API PKVOXELS Voxels_hCreate(PKINSTANCE hLib)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVoxels.hAdd(   std::make_shared<Voxels>(   roLib->fVoxelSizeMM(),
                                                                PICOGK_VOXEL_DEFAULTNARROWBAND));
}

PICOGK_API PKVOXELS Voxels_hCreateCopy( PKINSTANCE hLib,
                                        PKVOXELS hSource)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVoxels.hAdd(std::make_shared<Voxels>(*roLib->m_oVoxels.roGet(hSource)));
}

PICOGK_API PKVOXELS Voxels_hCreateSphere(   PKINSTANCE          hLib,
                                            const PKVector3*    pvecCenter,
                                            float               fRadius)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVoxels.hAdd(std::make_shared<Voxels>(  roLib->fVoxelSizeMM(),
                                                            PICOGK_VOXEL_DEFAULTNARROWBAND,
                                                            *pvecCenter,
                                                            fRadius));
}

PICOGK_API PKVOXELS Voxels_hCreateCapsule(  PKINSTANCE          hLib,
                                            const PKVector3*    pvecStart,
                                            const PKVector3*    pvecStop,
                                            float               fRadius1,
                                            float               fRadius2)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVoxels.hAdd(std::make_shared<Voxels>(  roLib->fVoxelSizeMM(),
                                                            PICOGK_VOXEL_DEFAULTNARROWBAND,
                                                            *pvecStart,
                                                            *pvecStop,
                                                            fRadius1,
                                                            fRadius2));
}

PICOGK_API PKVOXELS Voxels_hCreateMeshShell(    PKINSTANCE          hLib,
                                                PKMESH              hMesh,
                                                float               fRadius)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVoxels.hAdd(std::make_shared<Voxels>(  roLib->fVoxelSizeMM(),
                                                            PICOGK_VOXEL_DEFAULTNARROWBAND,
                                                            *roLib->m_oMeshes.roGet(hMesh),
                                                            fRadius));
}

PICOGK_API bool Voxels_bIsValid(    PKINSTANCE hLib,
                                    PKVOXELS hThis)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVoxels.bIsValid(hThis);
}

PICOGK_API bool Voxels_bIsEmpty(    PKINSTANCE hLib,
                                    PKVOXELS hThis)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVoxels.roGet(hThis)->bIsEmpty();
}

PICOGK_API void Voxels_Destroy( PKINSTANCE hLib,
                                PKVOXELS hThis)
{
    
    // Library may have been cleaned up already, whereas the object
    // could still be alive due to delayed gargabe collector
    if (Library::oLib().bIsValid(hLib))
    {
        Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
        roLib->m_oVoxels.bDestroy(hThis);
    }
}

PICOGK_API bool Voxels_bDiagnose(   PKINSTANCE  hLib,
                                    PKVOXELS    hThis,
                                    char    psz[PKINFOSTRINGLEN])
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    std::string strDiagnostic = roLib->m_oVoxels.roGet(hThis)->strDiagnose();
    SafeCopyInfoString(strDiagnostic, psz);
    return strDiagnostic.length() == 0;
}

PICOGK_API int64_t Voxels_nMemUsage(    PKINSTANCE hLib,
                                        PKVOXELS hThis)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVoxels.roGet(hThis)->nMemUsage();
}

PICOGK_API float Voxels_fVoxelSize( PKINSTANCE  hLib,
                                    PKVOXELS    hThis)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return (float) roLib->m_oVoxels.roGet(hThis)->oVoxelSize();
}

PICOGK_API void Voxels_BoolAdd( PKINSTANCE hLib,
                                PKVOXELS hThis,
                                PKVOXELS hOther)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    roLib->m_oVoxels.roGet(hThis)->BoolAdd(*roLib->m_oVoxels.roGet(hOther));
}

PICOGK_API void Voxels_BoolSubtract(    PKINSTANCE hLib,
                                        PKVOXELS hThis,
                                        PKVOXELS hOther)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    roLib->m_oVoxels.roGet(hThis)->BoolSubtract(*roLib->m_oVoxels.roGet(hOther));
}

PICOGK_API void Voxels_BoolIntersect(   PKINSTANCE hLib,
                                        PKVOXELS hThis,
                                        PKVOXELS hOther)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    roLib->m_oVoxels.roGet(hThis)->BoolIntersect(*roLib->m_oVoxels.roGet(hOther));
}

PICOGK_API void Voxels_Offset(  PKINSTANCE hLib,
                                PKVOXELS hThis,
                                float fDist)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    roLib->m_oVoxels.roGet(hThis)->Offset(fDist);
}

PICOGK_API void Voxels_DoubleOffset(    PKINSTANCE hLib,
                                        PKVOXELS hThis,
                                        float fDist1,
                                        float fDist2)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    roLib->m_oVoxels.roGet(hThis)->DoubleOffset(fDist1, fDist2);
}

PICOGK_API void Voxels_TripleOffset(    PKINSTANCE hLib,
                                        PKVOXELS hThis,
                                        float fDist)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    roLib->m_oVoxels.roGet(hThis)->TripleOffset(fDist);
}


PICOGK_API void Voxels_RenderMesh(  PKINSTANCE hLib,
                                    PKVOXELS hThis,
                                    PKMESH hMesh)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    roLib->m_oVoxels.roGet(hThis)
        ->RenderMesh(*roLib->m_oMeshes.roGet(hMesh));
}

PICOGK_API void Voxels_RenderImplicit(  PKINSTANCE hLib,
                                        PKVOXELS hThis,
                                        const PKBBox3* poBBox,
                                        PKPFnfSdf pfnSDF)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    roLib->m_oVoxels.roGet(hThis)
        ->RenderImplicit(   *poBBox,
                            pfnSDF);
}

PICOGK_API void Voxels_IntersectImplicit(   PKINSTANCE hLib,
                                            PKVOXELS hThis,
                                            PKPFnfSdf pfnSDF)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    roLib->m_oVoxels.roGet(hThis)
        ->IntersectImplicit(    pfnSDF);
}

PICOGK_API void Voxels_RenderLattice(   PKINSTANCE hLib,
                                        PKVOXELS hThis,
                                        PKLATTICE hLattice)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    roLib->m_oVoxels.roGet(hThis)
        ->RenderLattice(    *roLib->m_oLattices.roGet(hLattice));
}

PICOGK_API void Voxels_ProjectZSlice(   PKINSTANCE hLib,
                                        PKVOXELS hThis,
                                        float fZStart,
                                        float fZEnd)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    roLib->m_oVoxels.roGet(hThis)
        ->ProjectZSlice(    fZStart,
                            fZEnd);
}

PICOGK_API bool Voxels_bIsEqual(    PKINSTANCE hLib,
                                    PKVOXELS hThis,
                                    PKVOXELS hOther)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVoxels.roGet(hThis)
        ->bIsEqual(*roLib->m_oVoxels.roGet(hOther));
}

PICOGK_API float Voxels_fCalculateVolume(   PKINSTANCE hLib,
                                            PKVOXELS hThis)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVoxels.roGet(hThis)->fCalculateVolume();
}

PICOGK_API bool Voxels_bIsInside(   PKINSTANCE hLib,
                                    PKVOXELS hThis,
                                    const PKVector3* pvecTestPoint)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVoxels.roGet(hThis)->bIsInside(*pvecTestPoint);
}

PICOGK_API void Voxels_GetSurfaceNormal(    PKINSTANCE hLib,
                                            PKVOXELS            hThis,
                                            const PKVector3*    pvecSurfacePoint,
                                            PKVector3*          pvecNormal)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    roLib->m_oVoxels.roGet(hThis)
        ->GetSurfaceNormal( *pvecSurfacePoint,
                            pvecNormal);
}

PICOGK_API bool Voxels_bClosestPointOnSurface(  PKINSTANCE hLib,
                                                PKVOXELS            hThis,
                                                const PKVector3*    pvecSearch,
                                                PKVector3*          pvecSurfacePoint)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVoxels.roGet(hThis)
        ->bFindClosestPointOnSurface(  *pvecSearch,
                                        pvecSurfacePoint);
}

PICOGK_API bool Voxels_bRayCastToSurface(   PKINSTANCE hLib,
                                            PKVOXELS            hThis,
                                            const PKVector3*    pvecSearch,
                                            const PKVector3*    pvecDirection,
                                            PKVector3*          pvecSurfacePoint)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVoxels.roGet(hThis)
        ->bRayCastToSurface(    *pvecSearch,
                                *pvecDirection,
                                pvecSurfacePoint);
}

PICOGK_API void Voxels_GetVoxelDimensions(  PKINSTANCE hLib,
                                            PKVOXELS hThis,
                                            int32_t* pnXOrigin,
                                            int32_t* pnYOrigin,
                                            int32_t* pnZOrigin,
                                            int32_t* pnXSize,
                                            int32_t* pnYSize,
                                            int32_t* pnZSize)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVoxels.roGet(hThis)
        ->GetVoxelDimensions(   pnXOrigin,
                                pnYOrigin,
                                pnZOrigin,
                                pnXSize,
                                pnYSize,
                                pnZSize);
}

PICOGK_API void Voxels_GetXSlice(   PKINSTANCE hLib,
                                    PKVOXELS    hThis,
                                    int32_t     nXSlice,
                                    float*      pfBuffer,
                                    float*      pfBackgroundValue)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    
    Voxels::Ptr roThis = roLib->m_oVoxels.roGet(hThis);
    *pfBackgroundValue = roThis->fBackgroundMM();
    return roThis->GetXSlice(nXSlice, pfBuffer);
}

PICOGK_API void Voxels_GetYSlice(   PKINSTANCE hLib,
                                    PKVOXELS    hThis,
                                    int32_t     nYSlice,
                                    float*      pfBuffer,
                                    float*      pfBackgroundValue)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    
    Voxels::Ptr roThis = roLib->m_oVoxels.roGet(hThis);
    *pfBackgroundValue = roThis->fBackgroundMM();
    return roThis->GetYSlice(nYSlice, pfBuffer);
}

PICOGK_API void Voxels_GetZSlice(   PKINSTANCE hLib,
                                    PKVOXELS    hThis,
                                    int32_t     nZSlice,
                                    float*      pfBuffer,
                                    float*      pfBackgroundValue)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    
    Voxels::Ptr roThis = roLib->m_oVoxels.roGet(hThis);
    *pfBackgroundValue = roThis->fBackgroundMM();
    return roThis->GetZSlice(nZSlice, pfBuffer);
}

PICOGK_API void Voxels_GetInterpolatedZSlice(   PKINSTANCE hLib,
                                                PKVOXELS    hThis,
                                                float       fZSlice,
                                                float*      pfBuffer,
                                                float*      pfBackgroundValue)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    
    Voxels::Ptr roThis = roLib->m_oVoxels.roGet(hThis);
    *pfBackgroundValue = roThis->fBackgroundMM();
    return roThis->GetInterpolatedZSlice(fZSlice, pfBuffer);
}

PICOGK_API PKPOLYLINE PolyLine_hCreate( PKINSTANCE hLib,
                                        const PKColorFloat*  pclr)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oPolyLines.hAdd(std::make_shared<PolyLine>(*pclr));
}

PICOGK_API bool PolyLine_bIsValid(  PKINSTANCE hLib,
                                    PKPOLYLINE hThis)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oPolyLines.bIsValid(hThis);
}

PICOGK_API void PolyLine_Destroy(   PKINSTANCE hLib,
                                    PKPOLYLINE hThis)
{
    // Library may have been cleaned up already, whereas the object
    // could still be alive due to delayed gargabe collector
    if (Library::oLib().bIsValid(hLib))
    {
        Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
        roLib->m_oPolyLines.bDestroy(hThis);
    }
}

PICOGK_API int64_t PolyLine_nMemUsage(  PKINSTANCE hLib,
                                        PKPOLYLINE hThis)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oPolyLines.roGet(hThis)->nMemUsage();
}

PICOGK_API int32_t PolyLine_nAddVertex( PKINSTANCE hLib,
                                        PKPOLYLINE hThis,
                                        const Vector3* pvec)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oPolyLines.roGet(hThis)->nAddVertex(*pvec);
}

PICOGK_API void PolyLine_GetVertex( PKINSTANCE hLib,
                                    PKPOLYLINE hThis,
                                    int32_t nIndex,
                                    PKVector3* pvec)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    roLib->m_oPolyLines.roGet(hThis)->GetVertex(nIndex, pvec);
}

PICOGK_API void PolyLine_GetBoundingBox(    PKINSTANCE hLib,
                                            PKPOLYLINE hThis,
                                            PKBBox3* poBox)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    *poBox = roLib->m_oPolyLines.roGet(hThis)->oBBox();
}

PICOGK_API int32_t PolyLine_nVertexCount(   PKINSTANCE hLib,
                                            PKPOLYLINE hThis)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oPolyLines.roGet(hThis)->nVertexCount();
}

PICOGK_API void PolyLine_GetColor(  PKINSTANCE hLib,
                                    PKPOLYLINE hThis,
                                    PKColorFloat* pclr)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    *pclr = roLib->m_oPolyLines.roGet(hThis)->clrLines();
}




PICOGK_API PKVDBFILE VdbFile_hCreate(PKINSTANCE hLib)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVdbFiles.hAdd(std::make_shared<VdbFile>());
}

PICOGK_API PKVDBFILE VdbFile_hCreateFromFile(   PKINSTANCE hLib,
                                                const char* pszFileName)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    
    VdbFile::Ptr roVdbFile = VdbFile::roFromFile(pszFileName);
            
    if (roVdbFile == nullptr)
        return 0;
    
    return roLib->m_oVdbFiles.hAdd(roVdbFile);
}

PICOGK_API bool VdbFile_bIsValid(   PKINSTANCE hLib,
                                    PKVDBFILE hThis)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVdbFiles.bIsValid(hThis);
}

PICOGK_API void VdbFile_Destroy(    PKINSTANCE hLib,
                                    PKVDBFILE hThis)
{
    // Library may have been cleaned up already, whereas the object
    // could still be alive due to delayed gargabe collector
    if (Library::oLib().bIsValid(hLib))
    {
        Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
        roLib->m_oVdbFiles.bDestroy(hThis);
    }
}

PICOGK_API int64_t VdbFile_nMemUsage(   PKINSTANCE hLib,
                                        PKVDBFILE hThis)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVdbFiles.roGet(hThis)->nMemUsage();
}

PICOGK_API bool VdbFile_bSaveToFile(    PKINSTANCE hLib,
                                        PKVDBFILE       hThis,
                                        const char*     pszFileName)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVdbFiles.roGet(hThis)->bSaveToFile(pszFileName);
}

PICOGK_API PKVOXELS VdbFile_hGetVoxels( PKINSTANCE hLib,
                                        PKVDBFILE   hThis,
                                        int32_t     nIndex)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    auto roVdbFile = roLib->m_oVdbFiles.roGet(hThis);
    
    GridBase::Ptr roGrid = roVdbFile->roGridAt(nIndex);
            
    if (!roGrid->isType<FloatGrid>())
        return 0;
            
    if (roGrid->getGridClass() != GRID_LEVEL_SET)
        return 0; // not a voxel field
            
    return roLib->m_oVoxels.hAdd(std::make_shared<Voxels>(  gridPtrCast<FloatGrid>(roGrid),
                                                            PICOGK_VOXEL_DEFAULTNARROWBAND));
}

PICOGK_API int32_t VdbFile_nAddVoxels(  PKINSTANCE hLib,
                                        PKVDBFILE   hThis,
                                        const char* pszFieldName,
                                        PKVOXELS    hVoxels)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVdbFiles.roGet(hThis)
        ->nAddGrid(pszFieldName, roLib->m_oVoxels.roGet(hVoxels)->roVdbGrid());
    
}

PICOGK_API PKSCALARFIELD VdbFile_hGetScalarField(   PKINSTANCE hLib,
                                                    PKVDBFILE hThis,
                                                    int32_t nIndex)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    auto roVdbFile = roLib->m_oVdbFiles.roGet(hThis);
    
    GridBase::Ptr roGrid = roVdbFile->roGridAt(nIndex);
            
    if (!roGrid->isType<FloatGrid>())
        return 0;
            
    // We treat all float grids as scalar fields, if loaded through this function
    // PicoGK stores scalar fields as fog volumes
    
    return roLib->m_oScalarFields.hAdd(std::make_shared<ScalarField>(gridPtrCast<FloatGrid>(roGrid)));
}

PICOGK_API int32_t VdbFile_nAddScalarField( PKINSTANCE hLib,
                                            PKVDBFILE       hThis,
                                            const char*     pszFieldName,
                                            PKSCALARFIELD   hScalarField)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVdbFiles.roGet(hThis)
        ->nAddGrid(pszFieldName, roLib->m_oScalarFields.roGet(hScalarField)->roVdbGrid());
}

PICOGK_API PKVECTORFIELD VdbFile_hGetVectorField(   PKINSTANCE hLib,
                                                    PKVDBFILE   hThis,
                                                    int32_t     nIndex)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    auto roVdbFile = roLib->m_oVdbFiles.roGet(hThis);
    
    GridBase::Ptr roGrid = roVdbFile->roGridAt(nIndex);
    
    if (!roGrid->isType<Vec3SGrid>())
        return 0;
        
    return roLib->m_oVectorFields.hAdd(std::make_shared<VectorField>(gridPtrCast<Vec3SGrid>(roGrid)));
}

PICOGK_API int32_t VdbFile_nAddVectorField( PKINSTANCE hLib,
                                            PKVDBFILE       hThis,
                                            const char*     pszFieldName,
                                            PKVECTORFIELD   hVectorField)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    
    return roLib->m_oVdbFiles.roGet(hThis)
        ->nAddGrid(pszFieldName, roLib->m_oVectorFields.roGet(hVectorField)->roVdbGrid());
}

PICOGK_API int32_t VdbFile_nFieldCount( PKINSTANCE hLib,
                                        PKVDBFILE hThis)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVdbFiles.roGet(hThis)->nGridCount();
}

PICOGK_API void VdbFile_GetFieldName(   PKINSTANCE hLib,
                                        PKVDBFILE   hThis,
                                        int32_t     nIndex,
                                        char        psz[PKINFOSTRINGLEN])
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    SafeCopyInfoString( roLib->m_oVdbFiles.roGet(hThis)->strNameAt(nIndex),
                        psz);
}

PICOGK_API int VdbFile_nFieldType(  PKINSTANCE hLib,
                                    PKVDBFILE   hThis,
                                    int32_t     nIndex)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVdbFiles.roGet(hThis)->nTypeAt(nIndex);
}

PICOGK_API PKSCALARFIELD ScalarField_hCreate(PKINSTANCE hLib)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oScalarFields.hAdd(std::make_shared<ScalarField>(roLib->fVoxelSizeMM()));
}

PICOGK_API PKSCALARFIELD ScalarField_hCreateCopy(   PKINSTANCE hLib,
                                                    PKSCALARFIELD hSource)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    
    ScalarField::Ptr roSource = roLib->m_oScalarFields.roGet(hSource);
    ScalarField::Ptr roNew = std::make_shared<ScalarField>(*roSource);
    return roLib->m_oScalarFields.hAdd(roNew);
}

PICOGK_API bool ScalarField_bIsValid(   PKINSTANCE hLib,
                                        PKSCALARFIELD hThis)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oScalarFields.bIsValid(hThis);
}

PICOGK_API void ScalarField_Destroy(    PKINSTANCE hLib,
                                        PKSCALARFIELD   hThis)
{
    // Library may have been cleaned up already, whereas the object
    // could still be alive due to delayed gargabe collector
    if (Library::oLib().bIsValid(hLib))
    {
        Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
        roLib->m_oScalarFields.bDestroy(hThis);
    }
}

PICOGK_API int64_t ScalarField_nMemUsage(   PKINSTANCE hLib,
                                            PKSCALARFIELD hThis)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oScalarFields.roGet(hThis)->nMemUsage();
}

PICOGK_API PKSCALARFIELD ScalarField_hCreateFromVoxels( PKINSTANCE hLib,
                                                        PKVOXELS hVoxels)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    
    Voxels::Ptr         roSource = roLib->m_oVoxels.roGet(hVoxels);
    ScalarField::Ptr    roNew = std::make_shared<ScalarField>(*roSource);
    return roLib->m_oScalarFields.hAdd(roNew);
}

PICOGK_API PKSCALARFIELD ScalarField_hBuildFromVoxels(  PKINSTANCE hLib,
                                                        PKVOXELS    hVoxels,
                                                        float       fScalarValue,
                                                        float       fSdThreshold)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    
    Voxels::Ptr         roSource = roLib->m_oVoxels.roGet(hVoxels);
    ScalarField::Ptr    roNew = std::make_shared<ScalarField>(*roSource);
    
    roNew->BuildFieldFrom(roSource, fScalarValue, fSdThreshold);
    return roLib->m_oScalarFields.hAdd(roNew);
}

PICOGK_API void ScalarField_SetValue(   PKINSTANCE hLib,
                                        PKSCALARFIELD       hThis,
                                        const PKVector3*    pvecPosition,
                                        float               fValue)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    
    roLib->m_oScalarFields.roGet(hThis)
        ->SetValue(     *pvecPosition,
                        fValue);
}

PICOGK_API bool ScalarField_bGetValue(  PKINSTANCE hLib,
                                        PKSCALARFIELD       hThis,
                                        const PKVector3*    pvecPosition,
                                        float*              pfValue)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oScalarFields.roGet(hThis)
        ->bGetValue(    *pvecPosition,
                        pfValue);
}

PICOGK_API void ScalarField_RemoveValue(    PKINSTANCE          hLib,
                                            PKSCALARFIELD       hThis,
                                            const PKVector3*    pvecPosition)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    roLib->m_oScalarFields.roGet(hThis)->RemoveValue(   *pvecPosition);
}

PICOGK_API void ScalarField_GetVoxelDimensions( PKINSTANCE hLib,
                                                PKSCALARFIELD hThis,
                                                int32_t* pnXOrigin,
                                                int32_t* pnYOrigin,
                                                int32_t* pnZOrigin,
                                                int32_t* pnXSize,
                                                int32_t* pnYSize,
                                                int32_t* pnZSize)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oScalarFields.roGet(hThis)
        ->GetVoxelDimensions(   pnXOrigin,
                                pnYOrigin,
                                pnZOrigin,
                                pnXSize,
                                pnYSize,
                                pnZSize);
    }

PICOGK_API void ScalarField_GetSlice(   PKINSTANCE hLib,
                                        PKSCALARFIELD   hThis,
                                        int32_t     nZSlice,
                                        float*      pfBuffer)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oScalarFields.roGet(hThis)->GetSlice(nZSlice, pfBuffer);
}

PICOGK_API void ScalarField_TraverseActive( PKINSTANCE hLib,
                                            PKSCALARFIELD hThis,
                                            PKFnTraverseActiveS pfnCallback)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    roLib->m_oScalarFields.roGet(hThis)
        ->TraverseActive(pfnCallback);
}

PICOGK_API PKVECTORFIELD VectorField_hCreate(PKINSTANCE hLib)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVectorFields.hAdd(std::make_shared<VectorField>(roLib->fVoxelSizeMM()));
}

PICOGK_API PKVECTORFIELD VectorField_hCreateCopy(   PKINSTANCE hLib,
                                                    PKVECTORFIELD hSource)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    VectorField::Ptr roSource = roLib->m_oVectorFields.roGet(hSource);
    VectorField::Ptr roNew = std::make_shared<VectorField>(*roSource);
    return roLib->m_oVectorFields.hAdd(roNew);
}

PICOGK_API bool VectorField_bIsValid(   PKINSTANCE hLib,
                                        PKVECTORFIELD hThis)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVectorFields.bIsValid(hThis);
}

PICOGK_API void VectorField_Destroy(    PKINSTANCE hLib,
                                        PKVECTORFIELD hThis)
{
    // Library may have been cleaned up already, whereas the object
    // could still be alive due to delayed gargabe collector
    if (Library::oLib().bIsValid(hLib))
    {
        Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
        roLib->m_oVectorFields.bDestroy(hThis);
    }
}

PICOGK_API PKVECTORFIELD VectorField_hCreateFromVoxels( PKINSTANCE hLib,
                                                        PKVOXELS hVoxels)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    
    Voxels::Ptr         roVoxels    = roLib->m_oVoxels.roGet(hVoxels);
    VectorField::Ptr    roField     = std::make_shared<VectorField>(roVoxels->oVoxelSize());
    
    roField->AddGradientFieldFrom(roVoxels);
    return roLib->m_oVectorFields.hAdd(roField);
}

PICOGK_API PKVECTORFIELD VectorField_hBuildFromVoxels(  PKINSTANCE hLib,
                                                        PKVOXELS hVoxels,
                                                        const PKVector3* pvecValue,
                                                        float fSdThreshold)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    
    Voxels::Ptr         roVoxels    = roLib->m_oVoxels.roGet(hVoxels);
    VectorField::Ptr    roField     = std::make_shared<VectorField>(roVoxels->oVoxelSize());
    
    roField->BuildFieldFrom(roVoxels, *pvecValue, fSdThreshold);
    return roLib->m_oVectorFields.hAdd(roField);
}

PICOGK_API int64_t VectorField_nMemUsage(   PKINSTANCE hLib,
                                            PKVECTORFIELD hThis)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVectorFields.roGet(hThis)->nMemUsage();
}

PICOGK_API void VectorField_SetValue(   PKINSTANCE hLib,
                                        PKVECTORFIELD       hThis,
                                        const PKVector3*    pvecPosition,
                                        const PKVector3*    pvecValue)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    
    roLib->m_oVectorFields.roGet(hThis)
        ->SetValue( *pvecPosition,
                    *pvecValue);
}

PICOGK_API bool VectorField_bGetValue(  PKINSTANCE          hLib,
                                        PKVECTORFIELD       hThis,
                                        const PKVector3*    pvecPosition,
                                        PKVector3*          pvecValue)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    
    return roLib->m_oVectorFields.roGet(hThis)
        ->bGetValue(    *pvecPosition,
                        pvecValue);
}

PICOGK_API void VectorField_RemoveValue(    PKINSTANCE          hLib,
                                            PKVECTORFIELD       hThis,
                                            const PKVector3*    pvecPosition)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    
    roLib->m_oVectorFields.roGet(hThis)
        ->RemoveValue(*pvecPosition);
}

PICOGK_API void VectorField_TraverseActive( PKINSTANCE hLib,
                                            PKVECTORFIELD hThis,
                                            PKFnTraverseActiveV pfnCallback)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    roLib->m_oVectorFields.roGet(hThis)
        ->TraverseActive(pfnCallback);
}

PICOGK_API PKMETADATA Metadata_hFromVoxels( PKINSTANCE hLib,
                                            PKVOXELS hField)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    auto roField = roLib->m_oVoxels.roGet(hField);
    return roLib->m_oVdbMetas.hAdd(std::make_shared<VdbMeta>(roField->roVdbGrid()));
}

PICOGK_API PKMETADATA Metadata_hFromScalarField(    PKINSTANCE hLib,
                                                    PKSCALARFIELD hField)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    auto roField = roLib->m_oScalarFields.roGet(hField);
    return roLib->m_oVdbMetas.hAdd(std::make_shared<VdbMeta>(roField->roVdbGrid()));
}

PICOGK_API PKMETADATA Metadata_hFromVectorField(    PKINSTANCE hLib,
                                                    PKVECTORFIELD hField)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    auto roField = roLib->m_oVectorFields.roGet(hField);
    return roLib->m_oVdbMetas.hAdd(std::make_shared<VdbMeta>(roField->roVdbGrid()));
}

PICOGK_API void Metadata_Destroy(   PKINSTANCE hLib,
                                    PKMETADATA hThis)
{
    // Library may have been cleaned up already, whereas the object
    // could still be alive due to delayed gargabe collector
    if (Library::oLib().bIsValid(hLib))
    {
        Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
        roLib->m_oVdbMetas.bDestroy(hThis);
    }
}

PICOGK_API int32_t Metadata_nCount( PKINSTANCE hLib,
                                    PKMETADATA hThis)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVdbMetas.roGet(hThis)->nCount();
}

PICOGK_API int32_t Metadata_nNameLengthAt(  PKINSTANCE hLib,
                                            PKMETADATA  hThis,
                                            int32_t     nIndex)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    std::string strName = roLib->m_oVdbMetas.roGet(hThis)->strNameAt(nIndex);
    return (int32_t) strName.length();
}

PICOGK_API bool Metadata_bGetNameAt(    PKINSTANCE hLib,
                                        PKMETADATA  hThis,
                                        int32_t     nIndex,
                                        char*       psz,
                                        int32_t     nMaxStringLen)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    
    if (nIndex >= roLib->m_oVdbMetas.roGet(hThis)->nCount())
        return false;
    
    std::string s = roLib->m_oVdbMetas.roGet(hThis)->strNameAt(nIndex);
    
#ifdef _WINDOWS
    strncpy_s(psz, nMaxStringLen-1, s.c_str(), s.length());
#else
    strncpy(psz, s.c_str(), nMaxStringLen-1);
#endif
    psz[nMaxStringLen-1] = 0;
    
    return true;
}

PICOGK_API int32_t Metadata_nTypeAt(    PKINSTANCE hLib,
                                        PKMETADATA  hThis,
                                        const char* psz)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return (int32_t) roLib->m_oVdbMetas.roGet(hThis)->eTypeAt(psz);
}

PICOGK_API int32_t Metadata_nStringLengthAt(    PKINSTANCE hLib,
                                                PKMETADATA          hThis,
                                                const char*         psz)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    
     std::string str;
    if (!roLib->m_oVdbMetas.roGet(hThis)->bGetValueAt(psz, &str))
        return 0;
    
    return (int32_t) str.length();
}

PICOGK_API bool Metadata_bGetStringAt(  PKINSTANCE hLib,
                                        PKMETADATA      hThis,
                                        const char*     psz,
                                        char*           pszValue,
                                        int32_t         nMaxStringLen)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
     std::string s;
  
    if (!roLib->m_oVdbMetas.roGet(hThis)->bGetValueAt(psz, &s))
        return false;
    
#ifdef _WINDOWS
    strncpy_s(pszValue, nMaxStringLen-1, s.c_str(), s.length());
#else
    strncpy(pszValue, s.c_str(), nMaxStringLen-1);
#endif
    pszValue[nMaxStringLen-1] = 0;
    
    return true;
}

PICOGK_API bool Metadata_bGetFloatAt(   PKINSTANCE hLib,
                                        PKMETADATA      hThis,
                                        const char*     psz,
                                        float*          pfValue)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVdbMetas.roGet(hThis)->bGetValueAt(psz, pfValue);
}

PICOGK_API bool Metadata_bGetVectorAt(  PKINSTANCE hLib,
                                        PKMETADATA      hThis,
                                        const char*     psz,
                                        PKVector3*      pvecValue)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVdbMetas.roGet(hThis)->bGetValueAt(psz, pvecValue);
}

PICOGK_API void Metadata_SetStringValue(    PKINSTANCE hLib,
                                            PKMETADATA     hThis,
                                            const char*    pszFieldName,
                                            const char*    pszValue)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVdbMetas.roGet(hThis)->SetValue(pszFieldName, pszValue);
}

PICOGK_API void Metadata_SetFloatValue( PKINSTANCE hLib,
                                        PKMETADATA      hThis,
                                        const char*     pszFieldName,
                                        float           fValue)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    return roLib->m_oVdbMetas.roGet(hThis)->SetValue(pszFieldName, fValue);
}

PICOGK_API void Metadata_SetVectorValue(    PKINSTANCE hLib,
                                            PKMETADATA          hThis,
                                            const char*         pszFieldName,
                                            const PKVector3*    pvecValue)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    roLib->m_oVdbMetas.roGet(hThis)->SetValue(pszFieldName, *pvecValue);
}

PICOGK_API void MetaData_RemoveValue(   PKINSTANCE hLib,
                                        PKMETADATA  hThis,
                                        const char* pszFieldName)
{
    Library::Instance::Ptr roLib = Library::oLib().roGetInstance(hLib);
    roLib->m_oVdbMetas.roGet(hThis)->RemoveAt(pszFieldName);
}

PICOGK_API PKVIEWER Viewer_hCreate( const char*             pszWindowTitle,
                                    const PKVector2*        pvecSize,
                                    PKFInfo                 pfnInfoCallback,
                                    PKPFUpdateRequested     pfnUpdateCallback,
                                    PKPFKeyPressed          pfnKeyPressedCallback,
                                    PKPFMouseMoved          pfnMouseMoveCallback,
                                    PKPFMouseButton         pfnMouseButtonCallback,
                                    PKPFScrollWheel         pfnScrollWheelCallback,
                                    PKPFWindowSize          pfnWindowSize)
{
    PKTRACE(Viewer_hCreate);
    
    return (PKVIEWER) ViewerManager::oMgr().poCreate(
                pszWindowTitle,
                *pvecSize,
                pfnInfoCallback,
                pfnUpdateCallback,
                pfnKeyPressedCallback,
                pfnMouseMoveCallback,
                pfnMouseButtonCallback,
                pfnScrollWheelCallback,
                pfnWindowSize);
}

PICOGK_API bool Viewer_bIsValid(PKVIEWER hThis)
{
    //PKTRACE(Viewer_bIsValid);
    
    Viewer* poThis = (Viewer*) hThis;
    return ViewerManager::oMgr().bIsValid(poThis);
}

PICOGK_API void Viewer_Destroy(PKVIEWER hThis)
{
    PKTRACE(Viewer_Destroy);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    PicoGK::ViewerManager::oMgr().Destroy(poThis);
}

PICOGK_API void Viewer_RequestUpdate(PKVIEWER hThis)
{
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->RequestUpdate();
}

PICOGK_API bool Viewer_bPoll(PKVIEWER hThis)
{
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    return poThis->bPoll();
}

PICOGK_API  void Viewer_RequestScreenShot(  PKVIEWER        hThis,
                                            const char*     pszScreenShotPath)
{
    PKTRACE(Viewer_RequestScreenShot);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->RequestScreenShot(pszScreenShotPath);
}

PICOGK_API void Viewer_EnableExperimental(  PKVIEWER    hThis,
                                            bool        bEnable)
{
    PKTRACE(Viewer_EnableExperimental);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->EnableExperimental(bEnable);
}

PICOGK_API void Viewer_RequestClose(PKVIEWER hThis)
{
    PKTRACE(Viewer_RequestClose);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->RequestClose();
}

PICOGK_API bool Viewer_bLoadLightSetup( PKVIEWER        hThis,
                                        const char*     pDiffTextureDDS,
                                        int32_t         nDiffTextureSize,
                                        const char*     pSpecTextureDDS,
                                        int32_t         nSpecTextureSize)
{
    PKTRACE(Viewer_bLoadLightSetup);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    return poThis->bLoadLightSetup( pDiffTextureDDS,
                                    nDiffTextureSize,
                                    pSpecTextureDDS,
                                    nSpecTextureSize);
}

PICOGK_API void Viewer_AddMesh( PKINSTANCE hLib,
                                PKVIEWER    hThis,
                                int32_t     nGroupID,
                                PKMESH      hMesh)
{
    PKTRACE(Viewer_AddMesh);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->AddMesh(nGroupID, hLib, hMesh);
}

PICOGK_API void Viewer_RemoveMesh(  PKINSTANCE hLib,
                                    PKVIEWER    hThis,
                                    PKMESH      hMesh)
{
    PKTRACE(Viewer_RemoveMesh);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->RemoveMesh(hLib, hMesh);
}

PICOGK_API  void Viewer_SetMeshMatrix(  PKINSTANCE          hLib,
                                        PKVIEWER            hThis,
                                        PKMESH              hMesh,
                                        const PKMatrix4x4*  pmat)
{
    PKTRACE(Viewer_SetMeshMatrix);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->SetMeshMatrix(hLib, hMesh, *pmat);
}

PICOGK_API void Viewer_AddVoxels(   PKINSTANCE hLib,
                                    PKVIEWER    hThis,
                                    int32_t     nGroupID,
                                    PKVOXELS    hVoxels)
{
    PKTRACE(Viewer_AddVoxels);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->AddVoxels(nGroupID, hLib, hVoxels);
}

PICOGK_API void Viewer_RemoveVoxels(    PKINSTANCE  hLib,
                                        PKVIEWER    hThis,
                                        PKVOXELS    hVoxels)
{
    PKTRACE(Viewer_RemoveVoxels);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->RemoveVoxels(hLib, hVoxels);
}

PICOGK_API  void Viewer_SetVoxelsMatrix(    PKINSTANCE          hLib,
                                            PKVIEWER            hThis,
                                            PKVOXELS            hVoxels,
                                            const PKMatrix4x4*  pmat)
{
    PKTRACE(Viewer_SetVoxelsMatrix);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->SetVoxelsMatrix(hLib, hVoxels, *pmat);
}


PICOGK_API void Viewer_AddPolyLine( PKINSTANCE hLib,
                                    PKVIEWER    hThis,
                                    int32_t     nGroupID,
                                    PKPOLYLINE  hPolyLine)
{
    PKTRACE(Viewer_AddPolyLine);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->AddPolyLine(nGroupID, hLib, hPolyLine);
}

PICOGK_API void Viewer_RemovePolyLine(  PKINSTANCE hLib,
                                        PKVIEWER    hThis,
                                        PKPOLYLINE  hPolyLine)
{
    PKTRACE(Viewer_RemovePolyLine);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->RemovePolyLine(hLib, hPolyLine);
}

PICOGK_API  void Viewer_SetPolyLineMatrix(  PKINSTANCE          hLib,
                                            PKVIEWER            hThis,
                                            PKPOLYLINE          hPolyLine,
                                            const PKMatrix4x4*  pmat)
{
    PKTRACE(Viewer_SetPolyLineMatrix);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->SetPolyLineMatrix(hLib, hPolyLine, *pmat);
}

PICOGK_API void Viewer_RemoveAllObjects(PKVIEWER hThis)
{
    PKTRACE(Viewer_RemoveAllObjects);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->RemoveAllObjects();
}

PICOGK_API void Viewer_SetGroupVisible( PKVIEWER    hThis,
                                        int32_t     nGroupID,
                                        bool        bVisible)
{
    PKTRACE(Viewer_SetGroupVisible);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->SetGroupVisible(nGroupID, bVisible);
}

PICOGK_API  void Viewer_SetGroupMaterial(   PKVIEWER            hThis,
                                            int32_t             nGroupID,
                                            const PKColorFloat* pclr,
                                            float               fMetallic,
                                            float               fRoughness)
{
    PKTRACE(Viewer_SetGroupMaterial);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->SetGroupMaterial(nGroupID, *pclr, fMetallic, fRoughness);
}

PICOGK_API void Viewer_SetGroupMatrix(  PKVIEWER            hThis,
                                        int32_t             nGroupID,
                                        const PKMatrix4x4*  pmat)
{
    PKTRACE(Viewer_SetGroupMatrix);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->SetGroupMatrix(nGroupID, *pmat);
}

PICOGK_API void Viewer_EnableGroupWarnOverhang( PKVIEWER    hThis,
                                                int32_t     nGroupID,
                                                float       fWarning,
                                                float       fError)
{
    PKTRACE(Viewer_EnableGroupWarnOverhang);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->EnableGroupWarnOverhang(nGroupID, fWarning, fError);
}

PICOGK_API void Viewer_DisableGroupWarnOverhang(    PKVIEWER    hThis,
                                                    int32_t     nGroupID)
{
    PKTRACE(Viewer_DisableGroupWarnOverhang);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->DisableGroupWarnOverhang(nGroupID);
}

PICOGK_API void Viewer_GetBoundingBox(  PKVIEWER hThis,
                                        PKBBox3* poBox)
{
    PKTRACE(Viewer_GetBoundingBox);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    *poBox = poThis->oBBox();
}

PICOGK_API PKGPUTEX Viewer_GpuTex_hCreate(      PKVIEWER    hThis,
                                                int         nWidth,
                                                int         nHeight,
                                                const char* pRgba8)
{
    PKTRACE(Viewer_hCreateGpuTex);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    return poThis->hGpuTexCreate(nWidth, nHeight, pRgba8);
}

PICOGK_API void Viewer_GpuTex_Refresh(          PKVIEWER    hThis,
                                                PKGPUTEX    hTex,
                                                const char* pRgba8)
{
    PKTRACE(Viewer_GpuTex_Refresh);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    return poThis->GpuTexRefresh(hTex, pRgba8);
}

PICOGK_API void Viewer_GpuTex_MarkForCleanup(   PKVIEWER hThis,
                                                PKGPUTEX hTex)
{
    PKTRACE(Viewer_GpuTex_MarkForCleanup);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->GpuTexMarkForCleanup(hTex);
}

PICOGK_API PKQUAD Viewer_Quad_hCreate(  PKVIEWER            hThis,
                                        PKGPUTEX            hTexDefault,
                                        PKColorFloat        clrDefault,
                                        float               fAlpha,
                                        const PKMatrix4x4*  pmat,
                                        bool                bFlipX,
                                        bool                bFlipY,
                                        bool                bDoubleSided)
{
    PKTRACE(Viewer_Quad_hCreate);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    return poThis->hAddQuad(    hTexDefault,
                                clrDefault,
                                fAlpha,
                                *pmat,
                                bFlipX,
                                bFlipY,
                                bDoubleSided);
}

PICOGK_API void Viewer_Quad_Destroy(    PKVIEWER    hThis,
                                        PKQUAD      hQuad)
{
    PKTRACE(Viewer_Quad_Destroy);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->RemoveQuad(hQuad);
}

PICOGK_API void Viewer_Quad_SetMatrix(  PKVIEWER            hThis,
                                        PKQUAD              hQuad,
                                        const PKMatrix4x4*  pmat)
{
    PKTRACE(Viewer_Quad_SetMatrix);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->SetQuadMatrix(hQuad, *pmat);
}

PICOGK_API PKGUI Viewer_SideBar_hCreate(    PKVIEWER        hThis,
                                            bool            bLeft,
                                            int             nMin,
                                            int             nMax,
                                            int             nDef,
                                            PKColorFloat    clrBackground,
                                            PKColorFloat    clrBackgroundHv)
{
    PKTRACE(Viewer_SideBar_hCreate);
    
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    return poThis->hCreateSideBar(  bLeft,
                                    nMin,
                                    nMax,
                                    nDef,
                                    clrBackground,
                                    clrBackgroundHv);
}

PICOGK_API void Viewer_SideBar_Destroy( PKVIEWER hThis,
                                        PKGUI hSideBar)
{
    PKTRACE(Viewer_SideBar_Destroy);

    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->DestroySideBar(hSideBar);
}
