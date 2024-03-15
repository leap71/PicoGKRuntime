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

#include "PicoGKTypes.h"
#include "PicoGK.h"

#include "PicoGKLibraryMgr.h"
#include "PicoGKGLViewer.h"

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

PICOGK_API void Library_Init(float fVoxelSizeMM)
{
    Library::oLib().InitLibrary(fVoxelSizeMM);
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

PICOGK_API void Library_VoxelsToMm( const PKVector3* pvecVoxelCoordinate,
                                    PKVector3* pvecMmCoordinate)
{
    VoxelSize oVoxelSize(Library::oLib().fVoxelSizeMM());
    pvecMmCoordinate->X = oVoxelSize.fToMM(pvecVoxelCoordinate->X);
    pvecMmCoordinate->Y = oVoxelSize.fToMM(pvecVoxelCoordinate->Y);
    pvecMmCoordinate->Z = oVoxelSize.fToMM(pvecVoxelCoordinate->Z);
}

PICOGK_API void Library_MmToVoxels( const PKVector3* pvecMmCoordinate,
                                    PKVector3* pvecVoxelCoordinate)
{
    VoxelSize oVoxelSize(Library::oLib().fVoxelSizeMM());
    pvecVoxelCoordinate->X = oVoxelSize.iToVoxels(pvecMmCoordinate->X);
    pvecVoxelCoordinate->Y = oVoxelSize.iToVoxels(pvecMmCoordinate->Y);
    pvecVoxelCoordinate->Z = oVoxelSize.iToVoxels(pvecMmCoordinate->Z);
}

PICOGK_API PKMESH Mesh_hCreate()
{
    return (PKMESH) Library::oLib().proMeshCreate();
}

PICOGK_API PKMESH Mesh_hCreateFromVoxels(PKVOXELS hVoxels)
{
    Voxels::Ptr* proVoxels = (Voxels::Ptr*) hVoxels;
    return (PKMESH) Library::oLib().proMeshCreateFromVoxels(**proVoxels);
}

PICOGK_API bool Mesh_bIsValid(PKMESH hThis)
{
    Mesh::Ptr* proThis = (Mesh::Ptr*) hThis;
    return Library::oLib().bMeshIsValid(proThis);
}

PICOGK_API void Mesh_Destroy(PKMESH hThis)
{
    Mesh::Ptr* proThis = (Mesh::Ptr*) hThis;
    assert(Library::oLib().bMeshIsValid(proThis));

    Library::oLib().MeshDestroy(proThis);
}

PICOGK_API int32_t Mesh_nAddVertex( PKMESH hThis,
                                    const Vector3* pvecVertex)
{
    Mesh::Ptr* proThis = (Mesh::Ptr*) hThis;
    assert(Library::oLib().bMeshIsValid(proThis));
    
    return (*proThis)->nAddVertex(*pvecVertex);
}
    
PICOGK_API void Mesh_GetVertex( PKMESH      hThis,
                                int32_t     nVertex,
                                Vector3*    pvecVertex)
{
    Mesh::Ptr* proThis = (Mesh::Ptr*) hThis;
    assert(Library::oLib().bMeshIsValid(proThis));
    
    (*proThis)->GetVertex(nVertex, pvecVertex);
}

PICOGK_API int32_t Mesh_nVertexCount(PKMESH hThis)
{
    Mesh::Ptr* proThis = (Mesh::Ptr*) hThis;
    assert(Library::oLib().bMeshIsValid(proThis));
    
    return (*proThis)->nVertexCount();
}

PICOGK_API int32_t Mesh_nAddTriangle(   PKMESH hThis,
                                        const Triangle* psTri)
{
    Mesh::Ptr* proThis = (Mesh::Ptr*) hThis;
    assert(Library::oLib().bMeshIsValid(proThis));
    
    return (*proThis)->nAddTriangle(*psTri);
}

PICOGK_API void Mesh_GetTriangle(   PKMESH hThis,
                                    int32_t nTriangle,
                                    Triangle* psTri)
{
    Mesh::Ptr* proThis = (Mesh::Ptr*) hThis;
    assert(Library::oLib().bMeshIsValid(proThis));
    
    return (*proThis)->GetTriangle( nTriangle,
                                    psTri);
}

PICOGK_API void Mesh_GetTriangleV(  PKMESH      hThis,
                                    int32_t     nTriangle,
                                    Vector3*    pvecA,
                                    Vector3*    pvecB,
                                    Vector3*    pvecC)
{
    Mesh::Ptr* proThis = (Mesh::Ptr*) hThis;
    assert(Library::oLib().bMeshIsValid(proThis));
    
    (*proThis)->GetTriangle(    nTriangle,
                                pvecA,
                                pvecB,
                                pvecC);
}

PICOGK_API void Mesh_GetBoundingBox(    PKMESH hThis,
                                        BBox3* poBox)
{
    Mesh::Ptr* proThis = (Mesh::Ptr*) hThis;
    assert(Library::oLib().bMeshIsValid(proThis));
    
    (*proThis)->GetBoundingBox(poBox);
}

PICOGK_API int32_t Mesh_nTriangleCount(PKMESH hThis)
{
    Mesh::Ptr* proThis = (Mesh::Ptr*) hThis;
    assert(Library::oLib().bMeshIsValid(proThis));
    
    return (*proThis)->nTriangleCount();
}

PICOGK_API PKLATTICE Lattice_hCreate()
{
    return (PKLATTICE) Library::oLib().proLatticeCreate();
}

PICOGK_API bool Lattice_bIsValid(PKLATTICE hThis)
{
    Lattice::Ptr* proThis = (Lattice::Ptr*) hThis;
    return Library::oLib().bLatticeIsValid(proThis);
}

PICOGK_API void Lattice_Destroy(PKLATTICE hThis)
{
    Lattice::Ptr* proThis = (Lattice::Ptr*) hThis;
    Library::oLib().LatticeDestroy(proThis);
}

PICOGK_API void Lattice_AddSphere(  PKLATTICE hThis,
                                    const Vector3* vecCenter,
                                    float fRadius)
{
    Lattice::Ptr* proThis = (Lattice::Ptr*) hThis;
    assert(Library::oLib().bLatticeIsValid(proThis));
    
    (*proThis)->AddSphere(  *vecCenter,
                            fRadius);
}

PICOGK_API void Lattice_AddBeam(    PKLATTICE hThis,
                                    const Vector3* pvecA,
                                    const Vector3* pvecB,
                                    float fRadiusA,
                                    float fRadiusB,
                                    bool  bRoundCap)
{
    Lattice::Ptr* proThis = (Lattice::Ptr*) hThis;
    assert(Library::oLib().bLatticeIsValid(proThis));
    
    (*proThis)->AddBeam(    *pvecA,
                            *pvecB,
                            fRadiusA,
                            fRadiusB,
                            bRoundCap);
}

PICOGK_API PKVOXELS Voxels_hCreate()
{
    return (PKVOXELS) Library::oLib().proVoxelsCreate();
}

PICOGK_API PKVOXELS Voxels_hCreateCopy(PKVOXELS hSource)
{
    Voxels::Ptr* proSource = (Voxels::Ptr*) hSource;
    assert(Library::oLib().bVoxelsIsValid(proSource));
    
    return (PKVOXELS) Library::oLib().proVoxelsCreateCopy(**proSource);
}

PICOGK_API bool Voxels_bIsValid(PKVOXELS hThis)
{
    Voxels::Ptr* proThis = (Voxels::Ptr*) hThis;
    return Library::oLib().bVoxelsIsValid(proThis);
}

PICOGK_API void Voxels_Destroy(PKVOXELS hThis)
{
    Voxels::Ptr* proThis = (Voxels::Ptr*) hThis;
    assert(Library::oLib().bVoxelsIsValid(proThis));
    
    Library::oLib().VoxelsDestroy(proThis);
}

PICOGK_API void Voxels_BoolAdd( PKVOXELS hThis,
                                PKVOXELS hOther)
{
    Voxels::Ptr* proThis = (Voxels::Ptr*) hThis;
    assert(Library::oLib().bVoxelsIsValid(proThis));
    
    Voxels::Ptr* proOther = (Voxels::Ptr*) hOther;
    assert(Library::oLib().bVoxelsIsValid(proOther));
    
    (*proThis)->BoolAdd(**proOther);
}

PICOGK_API void Voxels_BoolSubtract( PKVOXELS hThis,
                                     PKVOXELS hOther)
{
    Voxels::Ptr* proThis = (Voxels::Ptr*) hThis;
    assert(Library::oLib().bVoxelsIsValid(proThis));
    
    Voxels::Ptr* proOther = (Voxels::Ptr*) hOther;
    assert(Library::oLib().bVoxelsIsValid(proOther));
    
    (*proThis)->BoolSubtract(**proOther);
}

PICOGK_API void Voxels_BoolIntersect(   PKVOXELS hThis,
                                        PKVOXELS hOther)
{
    Voxels::Ptr* proThis = (Voxels::Ptr*) hThis;
    assert(Library::oLib().bVoxelsIsValid(proThis));
    
    Voxels::Ptr* proOther = (Voxels::Ptr*) hOther;
    assert(Library::oLib().bVoxelsIsValid(proOther));
    
    (*proThis)->BoolIntersect(**proOther);
}

PICOGK_API void Voxels_Offset(  PKVOXELS hThis,
                                float fDist)
{
    Voxels::Ptr* proThis = (Voxels::Ptr*) hThis;
    assert(Library::oLib().bVoxelsIsValid(proThis));
    
    (*proThis)->Offset(fDist, Library::oLib().fVoxelSizeMM());
}

PICOGK_API void Voxels_DoubleOffset(    PKVOXELS hThis,
                                        float fDist1,
                                        float fDist2)
{
    Voxels::Ptr* proThis = (Voxels::Ptr*) hThis;
    assert(Library::oLib().bVoxelsIsValid(proThis));
    
    (*proThis)->DoubleOffset(fDist1, fDist2, Library::oLib().fVoxelSizeMM());
}

PICOGK_API void Voxels_TripleOffset(    PKVOXELS hThis,
                                        float fDist)
{
    Voxels::Ptr* proThis = (Voxels::Ptr*) hThis;
    assert(Library::oLib().bVoxelsIsValid(proThis));
    
    (*proThis)->TripleOffset(fDist, Library::oLib().fVoxelSizeMM());
}

PICOGK_API void Voxels_RenderMesh(  PKVOXELS hThis,
                                    PKMESH hMesh)
{
    Voxels::Ptr* proThis = (Voxels::Ptr*) hThis;
    assert(Library::oLib().bVoxelsIsValid(proThis));
    
    Mesh::Ptr* proMesh = (Mesh::Ptr*) hMesh;
    assert(Library::oLib().bMeshIsValid(proMesh));
    
    (*proThis)->RenderMesh(**proMesh, Library::oLib().fVoxelSizeMM());
}

PICOGK_API void Voxels_RenderImplicit(  PKVOXELS hThis,
                                        const PKBBox3* poBBox,
                                        PKPFnfSdf pfnSDF)
{
    Voxels::Ptr* proThis = (Voxels::Ptr*) hThis;
    assert(Library::oLib().bVoxelsIsValid(proThis));
    
    (*proThis)->RenderImplicit(*poBBox, pfnSDF, Library::oLib().fVoxelSizeMM());
}

PICOGK_API void Voxels_IntersectImplicit(   PKVOXELS hThis,
                                            PKPFnfSdf pfnSDF)
{
    Voxels::Ptr* proThis = (Voxels::Ptr*) hThis;
    assert(Library::oLib().bVoxelsIsValid(proThis));
    
    (*proThis)->IntersectImplicit(pfnSDF, Library::oLib().fVoxelSizeMM());
}

PICOGK_API void Voxels_RenderLattice(   PKVOXELS hThis,
                                        PKLATTICE hLattice)
{
    Voxels::Ptr* proThis = (Voxels::Ptr*) hThis;
    assert(Library::oLib().bVoxelsIsValid(proThis));
    
    Lattice::Ptr* proLattice = (Lattice::Ptr*) hLattice;
    assert(Library::oLib().bLatticeIsValid(proLattice));
    
    (*proThis)->RenderLattice(**proLattice, Library::oLib().fVoxelSizeMM());
}

PICOGK_API void Voxels_ProjectZSlice( PKVOXELS hThis,
                                      float fZStart,
                                      float fZEnd)
{
    Voxels::Ptr* proThis = (Voxels::Ptr*) hThis;
    assert(Library::oLib().bVoxelsIsValid(proThis));
    
    (*proThis)->ProjectZSlice(  fZStart,
                                fZEnd,
                                Library::oLib().fVoxelSizeMM());
}

PICOGK_API bool Voxels_bIsEqual(    PKVOXELS hThis,
                                    PKVOXELS hOther)
{
    Voxels::Ptr* proThis = (Voxels::Ptr*) hThis;
    assert(Library::oLib().bVoxelsIsValid(proThis));
    
    Voxels::Ptr* proOther = (Voxels::Ptr*) hOther;
    assert(Library::oLib().bVoxelsIsValid(proOther));
    
    return (*proThis)->bIsEqual(**proOther);
}

PICOGK_API void Voxels_CalculateProperties( PKVOXELS hThis,
                                            float* pfVolume,
                                            BBox3* poBBox)
{
    Voxels::Ptr* proThis = (Voxels::Ptr*) hThis;
    assert(Library::oLib().bVoxelsIsValid(proThis));
    
    (*proThis)->CalculateProperties(pfVolume, poBBox, Library::oLib().fVoxelSizeMM());
}

PICOGK_API void Voxels_GetSurfaceNormal(    PKVOXELS            hThis,
                                            const PKVector3*    pvecSurfacePoint,
                                            PKVector3*          pvecNormal)
{
    Voxels::Ptr* proThis = (Voxels::Ptr*) hThis;
    assert(Library::oLib().bVoxelsIsValid(proThis));
    
    (*proThis)->GetSurfaceNormal(*pvecSurfacePoint, Library::oLib().fVoxelSizeMM(), pvecNormal);
}

PICOGK_API bool Voxels_bClosestPointOnSurface(  PKVOXELS            hThis,
                                                const PKVector3*    pvecSearch,
                                                PKVector3*          pvecSurfacePoint)
{
    Voxels::Ptr* proThis = (Voxels::Ptr*) hThis;
    assert(Library::oLib().bVoxelsIsValid(proThis));
    
    return (*proThis)->bFindClosestPointOnSurface(  *pvecSearch,
                                                    Library::oLib().fVoxelSizeMM(),
                                                    pvecSurfacePoint);
}

PICOGK_API bool Voxels_bRayCastToSurface(   PKVOXELS            hThis,
                                            const PKVector3*    pvecSearch,
                                            const PKVector3*    pvecDirection,
                                            PKVector3*          pvecSurfacePoint)
{
    Voxels::Ptr* proThis = (Voxels::Ptr*) hThis;
    assert(Library::oLib().bVoxelsIsValid(proThis));
    
    return (*proThis)->bRayCastToSurface(   *pvecSearch,
                                            *pvecDirection,
                                            Library::oLib().fVoxelSizeMM(),
                                            pvecSurfacePoint);
}

PICOGK_API void Voxels_GetVoxelDimensions(  PKVOXELS hThis,
                                            int32_t* pnXOrigin,
                                            int32_t* pnYOrigin,
                                            int32_t* pnZOrigin,
                                            int32_t* pnXSize,
                                            int32_t* pnYSize,
                                            int32_t* pnZSize)
{
    Voxels::Ptr* proThis = (Voxels::Ptr*) hThis;
    assert(Library::oLib().bVoxelsIsValid(proThis));
    
    return (*proThis)->GetVoxelDimensions(  pnXOrigin,
                                            pnYOrigin,
                                            pnZOrigin,
                                            pnXSize,
                                            pnYSize,
                                            pnZSize);
}

PICOGK_API void Voxels_GetSlice(    PKVOXELS    hThis,
                                    int32_t     nZSlice,
                                    float*      pfBuffer,
                                    float*      pfBackgroundValue)
{
    Voxels::Ptr* proThis = (Voxels::Ptr*) hThis;
    assert(Library::oLib().bVoxelsIsValid(proThis));
    
    *pfBackgroundValue = (*proThis)->fBackground();
    return (*proThis)->GetSlice(nZSlice, pfBuffer);
}

PICOGK_API PKPOLYLINE PolyLine_hCreate(const ColorFloat*  pclr)
{
    return Library::oLib().proPolyLineCreate(*pclr);
}

PICOGK_API bool PolyLine_bIsValid(PKPOLYLINE hThis)
{
    PolyLine::Ptr* proThis = (PolyLine::Ptr*) hThis;
    return Library::oLib().bPolyLineIsValid(proThis);
}

PICOGK_API void PolyLine_Destroy(PKPOLYLINE hThis)
{
    PolyLine::Ptr* proThis = (PolyLine::Ptr*) hThis;
    assert(Library::oLib().bPolyLineIsValid(proThis));
    
    Library::oLib().PolyLineDestroy(proThis);
}

PICOGK_API int32_t PolyLine_nAddVertex( PKPOLYLINE hThis,
                                        const Vector3* pvec)
{
    PolyLine::Ptr* proThis = (PolyLine::Ptr*) hThis;
    assert(Library::oLib().bPolyLineIsValid(proThis));
    
    return (*proThis)->nAddVertex(*pvec);
}

PICOGK_API void PolyLine_GetVertex( PKPOLYLINE hThis,
                                    int32_t nIndex,
                                    Vector3* pvec)
{
    PolyLine::Ptr* proThis = (PolyLine::Ptr*) hThis;
    assert(Library::oLib().bPolyLineIsValid(proThis));
    
    (*proThis)->GetVertex(nIndex, pvec);
}

PICOGK_API int32_t PolyLine_nVertexCount(PKPOLYLINE hThis)
{
    PolyLine::Ptr* proThis = (PolyLine::Ptr*) hThis;
    assert(Library::oLib().bPolyLineIsValid(proThis));
    
    return (*proThis)->nVertexCount();
}

PICOGK_API void PolyLine_GetColor(  PKPOLYLINE hThis,
                                    ColorFloat* pclr)
{
    PolyLine::Ptr* proThis = (PolyLine::Ptr*) hThis;
    assert(Library::oLib().bPolyLineIsValid(proThis));
    
    *pclr = (*proThis)->clrLines();
}


PICOGK_API PKVIEWER Viewer_hCreate( const char*             pszWindowTitle,
                                    const Vector2*          pvecSize,
                                    PKFInfo                 pfnInfoCallback,
                                    PKPFUpdateRequested     pfnUpdateCallback,
                                    PKPFKeyPressed          pfnKeyPressedCallback,
                                    PKPFMouseMoved          pfnMouseMoveCallback,
                                    PKPFMouseButton         pfnMouseButtonCallback,
                                    PKPFScrollWheel         pfnScrollWheelCallback,
                                    PKPFWindowSize          pfnWindowSize)
{
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
    Viewer* poThis = (Viewer*) hThis;
    return ViewerManager::oMgr().bIsValid(poThis);
}

PICOGK_API void Viewer_Destroy(PKVIEWER hThis)
{
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
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->RequestScreenShot(pszScreenShotPath);
}

PICOGK_API void Viewer_RequestClose(PKVIEWER hThis)
{
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
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    return poThis->bLoadLightSetup( pDiffTextureDDS,
                                    nDiffTextureSize,
                                    pSpecTextureDDS,
                                    nSpecTextureSize);
}

PICOGK_API void Viewer_AddMesh( PKVIEWER    hThis,
                                int32_t     nGroupID,
                                PKMESH      hMesh)
{
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    Mesh::Ptr* proMesh = (Mesh::Ptr*) hMesh;
    assert(Library::oLib().bMeshIsValid(proMesh));
    
    poThis->AddMesh(nGroupID, proMesh);
}

PICOGK_API void Viewer_RemoveMesh(  PKVIEWER hThis,
                                    PKMESH   hMesh)
{
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    Mesh::Ptr* proMesh = (Mesh::Ptr*) hMesh;
    assert(Library::oLib().bMeshIsValid(proMesh));
    
    poThis->RemoveMesh(proMesh);
}

PICOGK_API void Viewer_AddPolyLine( PKVIEWER    hThis,
                                    int32_t     nGroupID,
                                    PKPOLYLINE  hPolyLine)
{
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    PolyLine::Ptr* proPoly = (PolyLine::Ptr*) hPolyLine;
    assert(Library::oLib().bPolyLineIsValid(proPoly));
    
    poThis->AddPolyLine(    nGroupID,
                            proPoly);
}

PICOGK_API void Viewer_RemovePolyLine(  PKVIEWER    hThis,
                                        PKPOLYLINE  hPolyLine)
{
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    PolyLine::Ptr* proPoly = (PolyLine::Ptr*) hPolyLine;
    assert(Library::oLib().bPolyLineIsValid(proPoly));
    
    poThis->RemovePolyLine(proPoly);
}


PICOGK_API void Viewer_SetGroupVisible( PKVIEWER    hThis,
                                        int32_t     nGroupID,
                                        bool        bVisible)
{
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->SetGroupVisible(nGroupID, bVisible);
}

PICOGK_API void Viewer_SetGroupStatic(  PKVIEWER    hThis,
                                        int32_t     nGroupID,
                                        bool        bStatic)
{
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->SetGroupStatic(nGroupID, bStatic);
}

PICOGK_API void Viewer_SetGroupMaterial(    PKVIEWER            hThis,
                                            int32_t             nGroupID,
                                            const ColorFloat*   pclr,
                                            float               fMetallic,
                                            float               fRoughness)
{
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->SetGroupMaterial(nGroupID, *pclr, fMetallic, fRoughness);
}

PICOGK_API void Viewer_SetGroupMatrix(  PKVIEWER            hThis,
                                        int32_t             nGroupID,
                                        const Matrix4x4*    pmat)
{
    Viewer* poThis = (Viewer*) hThis;
    assert(ViewerManager::oMgr().bIsValid(poThis));
    
    poThis->SetGroupMatrix(nGroupID, *pmat);
}

PICOGK_API PKVDBFILE VdbFile_hCreate()
{
    return (PKVDBFILE) Library::oLib().proVdbFileCreate();
}

PICOGK_API PKVDBFILE VdbFile_hCreateFromFile(const char* pszFileName)
{
    return (PKVDBFILE) Library::oLib().proVdbFileCreateFromFile(pszFileName);
}

PICOGK_API bool VdbFile_bIsValid(PKVDBFILE hThis)
{
    VdbFile::Ptr* proThis = (VdbFile::Ptr*) hThis;
    return Library::oLib().bVdbFileIsValid(proThis);
}

PICOGK_API void VdbFile_Destroy(PKVDBFILE hThis)
{
    VdbFile::Ptr* proThis = (VdbFile::Ptr*) hThis;
    assert(Library::oLib().bVdbFileIsValid(proThis));
    Library::oLib().VdbFileDestroy(proThis);
}

PICOGK_API bool VdbFile_bSaveToFile(    PKVDBFILE       hThis,
                                        const char*     pszFileName)
{
    VdbFile::Ptr* proThis = (VdbFile::Ptr*) hThis;
    assert(Library::oLib().bVdbFileIsValid(proThis));
    
    return (*proThis)->bSaveToFile(pszFileName);
}

PICOGK_API PKVOXELS VdbFile_hGetVoxels( PKVDBFILE   hThis,
                                        int32_t     nIndex)
{
    VdbFile::Ptr* proThis = (VdbFile::Ptr*) hThis;
    assert(Library::oLib().bVdbFileIsValid(proThis));
    
    return (PKVOXELS) Library::oLib().proVdbFileGetVoxels(*proThis, nIndex);
}

PICOGK_API int32_t VdbFile_nAddVoxels(  PKVDBFILE   hThis,
                                        const char* pszFieldName,
                                        PKVOXELS    hVoxels)
{
    VdbFile::Ptr* proThis = (VdbFile::Ptr*) hThis;
    assert(Library::oLib().bVdbFileIsValid(proThis));
    
    Voxels::Ptr* proVoxels = (Voxels::Ptr*) hVoxels;
    assert(Library::oLib().bVoxelsIsValid(proVoxels));
    
    return Library::oLib().nVdbFileAddVoxels(   *proThis,
                                                pszFieldName,
                                                *proVoxels);
}

PICOGK_API PKSCALARFIELD VdbFile_hGetScalarField(   PKVDBFILE hThis,
                                                    int32_t nIndex)
{
    VdbFile::Ptr* proThis = (VdbFile::Ptr*) hThis;
    assert(Library::oLib().bVdbFileIsValid(proThis));
    
    return (PKSCALARFIELD) Library::oLib().proVdbFileGetScalarField(*proThis, nIndex);
}

PICOGK_API int32_t VdbFile_nAddScalarField( PKVDBFILE       hThis,
                                            const char*     pszFieldName,
                                            PKSCALARFIELD   hScalarField)
{
    VdbFile::Ptr* proThis = (VdbFile::Ptr*) hThis;
    assert(Library::oLib().bVdbFileIsValid(proThis));
    
    ScalarField::Ptr* proField = (ScalarField::Ptr*) hScalarField;
    assert(Library::oLib().bScalarFieldIsValid(proField));
    
    return Library::oLib().nVdbFileAddScalarField(  *proThis,
                                                    pszFieldName,
                                                    *proField);
}

PICOGK_API PKVECTORFIELD VdbFile_hGetVectorField(   PKVDBFILE   hThis,
                                                    int32_t     nIndex)
{
    VdbFile::Ptr* proThis = (VdbFile::Ptr*) hThis;
    assert(Library::oLib().bVdbFileIsValid(proThis));
    
    return (PKVECTORFIELD) Library::oLib().proVdbFileGetVectorField(*proThis, nIndex);
}

PICOGK_API int32_t VdbFile_nAddVectorField( PKVDBFILE       hThis,
                                            const char*     pszFieldName,
                                            PKVECTORFIELD   hVectorField)
{
    VdbFile::Ptr* proThis = (VdbFile::Ptr*) hThis;
    assert(Library::oLib().bVdbFileIsValid(proThis));
    
    VectorField::Ptr* proField = (VectorField::Ptr*) hVectorField;
    assert(Library::oLib().bVectorieldIsValid(proField));
    
    return Library::oLib().nVdbFileAddVectorField(  *proThis,
                                                    pszFieldName,
                                                    *proField);
}

PICOGK_API int32_t VdbFile_nFieldCount(PKVDBFILE hThis)
{
    VdbFile::Ptr* proThis = (VdbFile::Ptr*) hThis;
    assert(Library::oLib().bVdbFileIsValid(proThis));
    
    return (*proThis)->nGridCount();
}

PICOGK_API void VdbFile_GetFieldName(   PKVDBFILE   hThis,
                                        int32_t     nIndex,
                                        char        psz[PKINFOSTRINGLEN])
{
    VdbFile::Ptr* proThis = (VdbFile::Ptr*) hThis;
    assert(Library::oLib().bVdbFileIsValid(proThis));
    
    SafeCopyInfoString( (*proThis)->strNameAt(nIndex),
                        psz);
}

PICOGK_API int VdbFile_nFieldType(  PKVDBFILE   hThis,
                                    int32_t     nIndex)
{
    VdbFile::Ptr* proThis = (VdbFile::Ptr*) hThis;
    assert(Library::oLib().bVdbFileIsValid(proThis));
    
    return (*proThis)->nTypeAt(nIndex);
}

PICOGK_API PKSCALARFIELD ScalarField_hCreate()
{
    return (PKSCALARFIELD) Library::oLib().proScalarFieldCreate();
}

PICOGK_API PKSCALARFIELD ScalarField_hCreateCopy(PKSCALARFIELD hSource)
{
    ScalarField::Ptr* proSource = (ScalarField::Ptr*) hSource;
    return (PKSCALARFIELD) Library::oLib().proScalarFieldCreateCopy(**proSource);
}

PICOGK_API bool ScalarField_bIsValid(PKSCALARFIELD hThis)
{
    ScalarField::Ptr* proThis = (ScalarField::Ptr*) hThis;
    return Library::oLib().bScalarFieldIsValid(proThis);
}

PICOGK_API void ScalarField_Destroy(PKSCALARFIELD   hThis)
{
    ScalarField::Ptr* proThis = (ScalarField::Ptr*) hThis;
    assert(Library::oLib().bScalarFieldIsValid(proThis));
    
    Library::oLib().ScalarFieldDestroy(proThis);
}

PICOGK_API PKSCALARFIELD ScalarField_hCreateFromVoxels(PKVOXELS hVoxels)
{
    Voxels::Ptr* proVoxels = (Voxels::Ptr*) hVoxels;
    assert(Library::oLib().bVoxelsIsValid(proVoxels));
    
    ScalarField::Ptr* proField = Library::oLib().proScalarFieldCreateFromVoxels(**proVoxels);
    return (PKVECTORFIELD) proField;
}

PICOGK_API void ScalarField_SetValue(   PKSCALARFIELD       hThis,
                                        const PKVector3*    pvecPosition,
                                        float               fValue)
{
    ScalarField::Ptr* proThis = (ScalarField::Ptr*) hThis;
    assert(Library::oLib().bScalarFieldIsValid(proThis));
    
    (*proThis)->SetValue(   *pvecPosition,
                            Library::oLib().fVoxelSizeMM(),
                            fValue);
}

PICOGK_API bool ScalarField_bGetValue(  PKSCALARFIELD       hThis,
                                        const PKVector3*    pvecPosition,
                                        float*              pfValue)
{
    ScalarField::Ptr* proThis = (ScalarField::Ptr*) hThis;
    assert(Library::oLib().bScalarFieldIsValid(proThis));
    
    return (*proThis)->bGetValue(   *pvecPosition,
                                    Library::oLib().fVoxelSizeMM(),
                                    pfValue);
}

PICOGK_API void ScalarField_RemoveValue(    PKSCALARFIELD       hThis,
                                            const PKVector3*    pvecPosition)
{
    ScalarField::Ptr* proThis = (ScalarField::Ptr*) hThis;
    assert(Library::oLib().bScalarFieldIsValid(proThis));
    
    (*proThis)->RemoveValue( *pvecPosition,
                             Library::oLib().fVoxelSizeMM());
}

PICOGK_API void ScalarField_GetVoxelDimensions( PKSCALARFIELD hThis,
                                                int32_t* pnXOrigin,
                                                int32_t* pnYOrigin,
                                                int32_t* pnZOrigin,
                                                int32_t* pnXSize,
                                                int32_t* pnYSize,
                                                int32_t* pnZSize)
{
    ScalarField::Ptr* proThis = (ScalarField::Ptr*) hThis;
    assert(Library::oLib().bScalarFieldIsValid(proThis));
    
    return (*proThis)->GetVoxelDimensions(  pnXOrigin,
                                            pnYOrigin,
                                            pnZOrigin,
                                            pnXSize,
                                            pnYSize,
                                            pnZSize);
}

PICOGK_API void ScalarField_GetSlice(   PKSCALARFIELD   hThis,
                                        int32_t     nZSlice,
                                        float*      pfBuffer)
{
    ScalarField::Ptr* proThis = (ScalarField::Ptr*) hThis;
    assert(Library::oLib().bScalarFieldIsValid(proThis));
    return (*proThis)->GetSlice(nZSlice, pfBuffer);
}

PICOGK_API void ScalarField_TraverseActive( PKSCALARFIELD hThis,
                                            PKFnTraverseActiveS pfnCallback)
{
    ScalarField::Ptr* proThis = (ScalarField::Ptr*) hThis;
    assert(Library::oLib().bScalarFieldIsValid(proThis));
    
    (*proThis)->TraverseActive(pfnCallback, Library::oLib().fVoxelSizeMM());
}

PICOGK_API PKVECTORFIELD VectorField_hCreate()
{
    return (PKVECTORFIELD) Library::oLib().proVectorFieldCreate();
}

PICOGK_API PKVECTORFIELD VectorField_hCreateCopy(PKVECTORFIELD hSource)
{
    VectorField::Ptr* proSource = (VectorField::Ptr*) hSource;
    return (PKVECTORFIELD) Library::oLib().proVectorFieldCreateCopy(**proSource);
}

PICOGK_API bool VectorField_bIsValid(PKVECTORFIELD hThis)
{
    VectorField::Ptr* proThis = (VectorField::Ptr*) hThis;
    return Library::oLib().bVectorFieldIsValid(proThis);
}

PICOGK_API void VectorField_Destroy(PKVECTORFIELD hThis)
{
    VectorField::Ptr* proThis = (VectorField::Ptr*) hThis;
    assert(Library::oLib().bVectorFieldIsValid(proThis));
    
    Library::oLib().VectorFieldDestroy(proThis);
}

PICOGK_API PKVECTORFIELD VectorField_hCreateFromVoxels(PKVOXELS hVoxels)
{
    Voxels::Ptr* proVoxels = (Voxels::Ptr*) hVoxels;
    assert(Library::oLib().bVoxelsIsValid(proVoxels));
    
    VectorField::Ptr* proField = Library::oLib().proVectorFieldCreate();
    (*proField)->AddGradientFieldFrom(*proVoxels);
    
    return (PKVECTORFIELD) proField;
}

PICOGK_API void VectorField_SetValue(   PKVECTORFIELD       hThis,
                                        const PKVector3*    pvecPosition,
                                        const PKVector3*    pvecValue)
{
    VectorField::Ptr* proThis = (VectorField::Ptr*) hThis;
    assert(Library::oLib().bVectorFieldIsValid(proThis));
    
    (*proThis)->SetValue(   *pvecPosition,
                            Library::oLib().fVoxelSizeMM(),
                            *pvecValue);
}

PICOGK_API bool VectorField_bGetValue(  PKSCALARFIELD       hThis,
                                        const PKVector3*    pvecPosition,
                                        PKVector3*          pvecValue)
{
    VectorField::Ptr* proThis = (VectorField::Ptr*) hThis;
    assert(Library::oLib().bVectorFieldIsValid(proThis));
    
    return (*proThis)->bGetValue(   *pvecPosition,
                                    Library::oLib().fVoxelSizeMM(),
                                    pvecValue);
}

PICOGK_API void VectorField_RemoveValue(    PKVECTORFIELD       hThis,
                                            const PKVector3*    pvecPosition)
{
    VectorField::Ptr* proThis = (VectorField::Ptr*) hThis;
    assert(Library::oLib().bVectorFieldIsValid(proThis));
    
    (*proThis)->RemoveValue( *pvecPosition,
                             Library::oLib().fVoxelSizeMM());
}

PICOGK_API void VectorField_TraverseActive( PKVECTORFIELD hThis,
                                            PKFnTraverseActiveV pfnCallback)
{
    VectorField::Ptr* proThis = (VectorField::Ptr*) hThis;
    assert(Library::oLib().bVectorFieldIsValid(proThis));
    
    (*proThis)->TraverseActive(pfnCallback, Library::oLib().fVoxelSizeMM());
}

PICOGK_API PKMETADATA Metadata_hFromVoxels(PKVOXELS hField)
{
    Voxels::Ptr* proField = (Voxels::Ptr*) hField;
    assert(Library::oLib().bVoxelsIsValid(proField));
    
    Library::oLib().proVdbMetaFromField((*proField)->roVdbGrid());
}

PICOGK_API PKMETADATA Metadata_hFromScalarField(PKSCALARFIELD hField)
{
    ScalarField::Ptr* proField = (ScalarField::Ptr*) hField;
    assert(Library::oLib().bScalarFieldIsValid(proField));
    
    Library::oLib().proVdbMetaFromField((*proField)->roVdbGrid());
}

PICOGK_API PKMETADATA Metadata_hFromVectorField(PKVECTORFIELD hField)
{
    VectorField::Ptr* proField = (VectorField::Ptr*) hField;
    assert(Library::oLib().bVectorFieldIsValid(proField));
    
    Library::oLib().proVdbMetaFromField((*proField)->roVdbGrid());
}

PICOGK_API void Metadata_Destroy(PKMETADATA hThis)
{
    VdbMeta::Ptr* proThis = (VdbMeta::Ptr*) hThis;
    assert(Library::oLib().bVdbMetaIsValid(proThis));
    
    Library::oLib().VdbMetaDestroy(proThis);
}

PICOGK_API int32_t Metadata_nCount(PKMETADATA hThis)
{
    VdbMeta::Ptr* proThis = (VdbMeta::Ptr*) hThis;
    assert(Library::oLib().bVdbMetaIsValid(proThis));
    
    return (*proThis)->nCount();
}

PICOGK_API int32_t Metadata_nNameLengthAt(  PKMETADATA  hThis,
                                            int32_t     nIndex)
{
    VdbMeta::Ptr* proThis = (VdbMeta::Ptr*) hThis;
    assert(Library::oLib().bVdbMetaIsValid(proThis));
    
    std::string strName = (*proThis)->strNameAt(nIndex);
    return (int32_t) strName.length();
}

PICOGK_API bool Metadata_bGetNameAt(        PKMETADATA  hThis,
                                            int32_t     nIndex,
                                            char*       psz,
                                            int32_t     nMaxStringLen)
{
    VdbMeta::Ptr* proThis = (VdbMeta::Ptr*) hThis;
    assert(Library::oLib().bVdbMetaIsValid(proThis));
    
    if (nIndex >= (*proThis)->nCount())
        return false;
    
    std::string s = (*proThis)->strNameAt(nIndex);
    
#ifdef _WINDOWS
    strncpy_s(psz, nMaxStringLen-1, s.c_str(), s.length());
#else
    strncpy(psz, s.c_str(), nMaxStringLen-1);
#endif
    psz[nMaxStringLen-1] = 0;
    
    return true;
}

PICOGK_API int32_t Metadata_nTypeAt(    PKMETADATA  hThis,
                                        const char* psz)
{
    VdbMeta::Ptr* proThis = (VdbMeta::Ptr*) hThis;
    assert(Library::oLib().bVdbMetaIsValid(proThis));
    
    return (int32_t) (*proThis)->eTypeAt(psz);
}

PICOGK_API int32_t Metadata_nStringLengthAt(    PKMETADATA          hThis,
                                                const char*         psz)
{
    VdbMeta::Ptr* proThis = (VdbMeta::Ptr*) hThis;
    assert(Library::oLib().bVdbMetaIsValid(proThis));
    
    std::string str;
    if (!(*proThis)->bGetValueAt(psz, &str))
        return 0;
    
    return (int32_t) str.length();
}

PICOGK_API bool Metadata_bGetStringAt(  PKMETADATA      hThis,
                                        const char*     psz,
                                        char*           pszValue,
                                        int32_t         nMaxStringLen)
{
    VdbMeta::Ptr* proThis = (VdbMeta::Ptr*) hThis;
    assert(Library::oLib().bVdbMetaIsValid(proThis));
    
    std::string s;
    if (!(*proThis)->bGetValueAt(psz, &s))
        return false;
    
#ifdef _WINDOWS
    strncpy_s(pszValue, nMaxStringLen-1, s.c_str(), s.length());
#else
    strncpy(pszValue, s.c_str(), nMaxStringLen-1);
#endif
    pszValue[nMaxStringLen-1] = 0;
    
    return true;
}

PICOGK_API bool Metadata_bGetFloatAt(   PKMETADATA      hThis,
                                        const char*     psz,
                                        float*          pfValue)
{
    VdbMeta::Ptr* proThis = (VdbMeta::Ptr*) hThis;
    assert(Library::oLib().bVdbMetaIsValid(proThis));
    
    return (*proThis)->bGetValueAt(psz, pfValue);
}

PICOGK_API bool Metadata_bGetVectorAt(  PKMETADATA      hThis,
                                        const char*     psz,
                                        PKVector3*      pvecValue)
{
    VdbMeta::Ptr* proThis = (VdbMeta::Ptr*) hThis;
    assert(Library::oLib().bVdbMetaIsValid(proThis));
    
    return (*proThis)->bGetValueAt(psz, pvecValue);
}

PICOGK_API void Metadata_SetStringValue(    PKMETADATA     hThis,
                                            const char*    pszFieldName,
                                            const char*    pszValue)
{
    VdbMeta::Ptr* proThis = (VdbMeta::Ptr*) hThis;
    assert(Library::oLib().bVdbMetaIsValid(proThis));
    
    return (*proThis)->SetValue(pszFieldName, pszValue);
}

PICOGK_API void Metadata_SetFloatValue( PKMETADATA      hThis,
                                        const char*     pszFieldName,
                                        float           fValue)
{
    VdbMeta::Ptr* proThis = (VdbMeta::Ptr*) hThis;
    assert(Library::oLib().bVdbMetaIsValid(proThis));
    
    return (*proThis)->SetValue(pszFieldName, fValue);
}

PICOGK_API void Metadata_SetVectorValue(    PKMETADATA          hThis,
                                            const char*         pszFieldName,
                                            const PKVector3*    pvecValue)
{
    VdbMeta::Ptr* proThis = (VdbMeta::Ptr*) hThis;
    assert(Library::oLib().bVdbMetaIsValid(proThis));
    
    (*proThis)->SetValue(pszFieldName, *pvecValue);
}

PICOGK_API void MetaData_RemoveValue(   PKMETADATA  hThis,
                                        const char* pszFieldName)
{
    VdbMeta::Ptr* proThis = (VdbMeta::Ptr*) hThis;
    assert(Library::oLib().bVdbMetaIsValid(proThis));
    
    (*proThis)->RemoveAt(pszFieldName);
}

