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

#ifndef PICOGKGL_H_
#define PICOGKGL_H_

#include "PicoGKApiTypes.h"

#ifdef __cplusplus
    #define PICOGK_EXTC extern "C"
#else
    #define PICOGK_EXTC
#endif

#ifdef PICOGK_BUILD_LIBRARY // set in compiler when building a library
    #if defined _WINDOWS
        #define PICOGK_API PICOGK_EXTC __declspec(dllexport)
    #else
        #define PICOGK_API PICOGK_EXTC __attribute__ ((visibility ("default")))
    #endif
#else // we are using the library
#if defined _WINDOWS
    #define PICOGK_API PICOGK_EXTC __declspec(dllimport)
#else
    #define PICOGK_API PICOGK_EXTC
#endif
#endif

#include "PicoGKApiTypes.h"

// Library information

#define PKINFOSTRINGLEN 255

PICOGK_API void         Library_Init(               float fVoxelSizeMM);

PICOGK_API void         Library_GetName(            char psz[PKINFOSTRINGLEN]);

PICOGK_API void         Library_GetVersion(         char psz[PKINFOSTRINGLEN]);

PICOGK_API void         Library_GetBuildInfo(       char psz[PKINFOSTRINGLEN]);

#define PKHANDLE    void*
#define PKMESH      PKHANDLE
#define PKLATTICE   PKHANDLE
#define PKPOLYLINE  PKHANDLE
#define PKVOXELS    PKHANDLE
#define PKVIEWER    PKHANDLE
#define PKVDBFILE   PKHANDLE

// MESH

PICOGK_API PKMESH       Mesh_hCreate();

PICOGK_API PKMESH       Mesh_hCreateFromVoxels(     PKVOXELS                hVoxels);

PICOGK_API bool         Mesh_bIsValid(              PKMESH                  hThis);

PICOGK_API void         Mesh_Destroy(               PKMESH                  hThis);

PICOGK_API int32_t      Mesh_nAddVertex(            PKMESH                  hThis,
                                                    const PKVector3*        pvecVertex);

PICOGK_API int32_t      Mesh_nVertexCount(          PKMESH                  hThis);

PICOGK_API void         Mesh_GetVertex(             PKMESH                  hThis,
                                                    int32_t                 nVertex,
                                                    PKVector3*              pvecVertex);

PICOGK_API int32_t      Mesh_nAddTriangle(          PKMESH hThis,
                                                    const PKTriangle*       psTri);

PICOGK_API int32_t      Mesh_nTriangleCount(        PKMESH                  hThis);

PICOGK_API void         Mesh_GetTriangle(           PKMESH                  hThis,
                                                    int32_t                 nTriangle,
                                                    PKTriangle*             psTri);

PICOGK_API void         Mesh_GetTriangleV(          PKMESH                  hThis,
                                                    int32_t                 nTriangle,
                                                    PKVector3*              pvecA,
                                                    PKVector3*              pvecB,
                                                    PKVector3*              pvecC);

PICOGK_API void         Mesh_GetBoundingBox(        PKMESH                  hThis,
                                                    PKBBox3*                poBox);

// LATTICE

PICOGK_API PKLATTICE    Lattice_hCreate();


PICOGK_API bool         Lattice_bIsValid(           PKLATTICE               hThis);


PICOGK_API void         Lattice_Destroy(            PKLATTICE               hThis);


PICOGK_API void         Lattice_AddSphere(          PKLATTICE               hThis,
                                                    const PKVector3*        vecCenter,
                                                    float                   fRadius);


PICOGK_API void         Lattice_AddBeam(            PKLATTICE               hThis,
                                                    const                   PKVector3* pvecA,
                                                    const                   PKVector3* pvecB,
                                                    float                   fRadiusA,
                                                    float                   fRadiusB,
                                                    bool                    bRoundCap);

// VOXELS

PICOGK_API PKVOXELS     Voxels_hCreate();

PICOGK_API PKVOXELS     Voxels_hCreateCopy(         PKVOXELS                hSource);

PICOGK_API bool         Voxels_bIsValid(            PKVOXELS                hThis);

PICOGK_API void         Voxels_Destroy(             PKVOXELS                hThis);

PICOGK_API void         Voxels_BoolAdd(             PKVOXELS                hThis,
                                                    PKVOXELS                hOther);

PICOGK_API void         Voxels_BoolSubtract(        PKVOXELS                hThis,
                                                    PKVOXELS                hOther);

PICOGK_API void         Voxels_BoolIntersect(       PKVOXELS                hThis,
                                                    PKVOXELS                hOther);

PICOGK_API void         Voxels_Offset(              PKVOXELS                hThis,
                                                    float                   fDist);

PICOGK_API void         Voxels_DoubleOffset(        PKVOXELS                hThis,
                                                    float                   fDist1,
                                                    float                   fDist2);

PICOGK_API void         Voxels_TripleOffset(        PKVOXELS                hThis,
                                                    float                   fDist);

PICOGK_API void         Voxels_RenderMesh(          PKVOXELS                hThis,
                                                    PKMESH                  hMesh);

PICOGK_API void         Voxels_RenderImplicit(      PKVOXELS                hThis,
                                                    const PKBBox3*          poBBox,
                                                    PKPFnfSdf               pfnSDF);

PICOGK_API void         Voxels_IntersectImplicit(   PKVOXELS                hThis,
                                                    PKPFnfSdf               pfnSDF);

PICOGK_API void         Voxels_RenderLattice(       PKVOXELS                hThis,
                                                    PKLATTICE               hLattice);

PICOGK_API void         Voxels_ProjectZSlice(       PKVOXELS                hThis,
                                                    float                   fStartZ,
                                                    float                   fEndZ);

PICOGK_API bool         Voxels_bIsInside(           PKVOXELS                hThis,
                                                    const PKVector3*        pvecTestPoint);

PICOGK_API bool         Voxels_bIsEqual(            PKVOXELS                hThis,
                                                    PKVOXELS                hOther);

PICOGK_API void         Voxels_CalculateProperties( PKVOXELS                hThis,
                                                    float*                  pfVolume,
                                                    PKBBox3*                poBBox);

PICOGK_API void         Voxels_GetSurfaceNormal(        PKVOXELS            hThis,
                                                        const PKVector3*    pvecSurfacePoint,
                                                        PKVector3*          pvecNormal);

PICOGK_API bool         Voxels_bClosestPointOnSurface(  PKVOXELS            hThis,
                                                        const PKVector3*    pvecSearch,
                                                        PKVector3*          pvecSurfacePoint);

PICOGK_API bool         Voxels_bRayCastToSurface(       PKVOXELS            hThis,
                                                        const PKVector3*    pvecSearch,
                                                        const PKVector3*    pvecDirection,
                                                        PKVector3*          pvecSurfacePoint);

PICOGK_API void         Voxels_GetVoxelDimensions(      PKVOXELS            hThis,
                                                        int32_t*            pnXSize,
                                                        int32_t*            pnYSize,
                                                        int32_t*            pnZSize);

PICOGK_API void         Voxels_GetSlice(                PKVOXELS            hThis,
                                                        int32_t             nZSlice,
                                                        float*              pfBuffer);

// POLYLINE

PICOGK_API PKPOLYLINE   PolyLine_hCreate(           const PKColorFloat*     pclr);

PICOGK_API bool         PolyLine_bIsValid(          PKHANDLE                hThis);

PICOGK_API void         PolyLine_Destroy(           PKHANDLE                hThis);

PICOGK_API int32_t      PolyLine_nAddVertex(        PKPOLYLINE              hThis,
                                                    const PKVector3*        pvec);

PICOGK_API int32_t      PolyLine_nVertexCount(      PKPOLYLINE              hThis);

PICOGK_API void         PolyLine_GetVertex(         PKPOLYLINE              hThis,
                                                    int32_t                 nIndex,
                                                    PKVector3*              pvec);

PICOGK_API void         PolyLine_GetColor(          PKPOLYLINE              hThis,
                                                    PKColorFloat*           pclr);

// VIEWER

PICOGK_API PKVIEWER     Viewer_hCreate(             const char*             pszWindowTitle,
                                                    const PKVector2*        pvecSize,
                                                    PKFInfo                 pfnInfoCallback,
                                                    PKPFUpdateRequested     pfnUpdateCallback,
                                                    PKPFKeyPressed          pfnKeyPressedCallback,
                                                    PKPFMouseMoved          pfnMouseMoveCallback,
                                                    PKPFMouseButton         pfnMouseButtonCallback,
                                                    PKPFScrollWheel         pfnScrollWheelCallback,
                                                    PKPFWindowSize          pfnWindowSize);

PICOGK_API  bool        Viewer_bIsValid(            PKVIEWER                hThis);

PICOGK_API  void        Viewer_Destroy(             PKVIEWER                hThis);

PICOGK_API  void        Viewer_RequestUpdate(       PKVIEWER                hThis);

PICOGK_API  bool        Viewer_bPoll(               PKVIEWER                hThis);

PICOGK_API  void        Viewer_RequestScreenShot(   PKVIEWER                hThis,
                                                    const char*             pszScreenShotPath);

PICOGK_API  void        Viewer_RequestClose(        PKVIEWER                hThis);

PICOGK_API bool         Viewer_bLoadLightSetup(     PKVIEWER                hThis,
                                                    const char*             pDiffTextureDDS,
                                                    int32_t                 nDiffTextureSize,
                                                    const char*             pSpecTextureDDS,
                                                    int32_t                 nSpecTextureSize);

PICOGK_API  void        Viewer_AddMesh(             PKVIEWER                hThis,
                                                    int32_t                 nGroupID,
                                                    PKMESH                  hMesh);

PICOGK_API  void        Viewer_RemoveMesh(          PKVIEWER                hThis,
                                                    PKMESH                  hMesh);

PICOGK_API  void        Viewer_AddPolyLine(         PKVIEWER                hThis,
                                                    int32_t                 nGroupID,
                                                    PKPOLYLINE              hPolyLine);

PICOGK_API  void        Viewer_RemovePolyLine(      PKVIEWER                hThis,
                                                    PKPOLYLINE              hPolyLine);

PICOGK_API  void        Viewer_SetGroupVisible(     PKVIEWER                hThis,
                                                    int32_t                 nGroupID,
                                                    bool                    bVisible);

PICOGK_API  void        Viewer_SetGroupStatic(      PKVIEWER                hThis,
                                                    int32_t                 nGroupID,
                                                    bool                    bStatic);

PICOGK_API  void        Viewer_SetGroupMaterial(    PKVIEWER                hThis,
                                                    int32_t                 nGroupID,
                                                    const PKColorFloat*     pclr,
                                                    float                   fMetallic,
                                                    float                   fRoughness);

PICOGK_API void         Viewer_SetGroupMatrix(      PKVIEWER                hThis,
                                                    int32_t                 nGroupID,
                                                    const PKMatrix4x4*      pmat);

PICOGK_API PKVDBFILE    VdbFile_hCreate();

PICOGK_API PKVDBFILE    VdbFile_hCreateFromFile(    const char*             pszFileName);

PICOGK_API bool         VdbFile_bIsValid(           PKVDBFILE                hThis);

PICOGK_API void         VdbFile_Destroy(            PKVDBFILE                hThis);

PICOGK_API bool         VdbFile_bSaveToFile(        PKVDBFILE               hVdbFile,
                                                    const char*             pszFileName);

PICOGK_API PKVOXELS     VdbFile_hGetVoxels(         PKVDBFILE               hVdbFile,
                                                    int32_t                 nIndex);

PICOGK_API int32_t      VdbFile_nAddVoxels(         PKVDBFILE               hVdbFile,
                                                    const char*             pszFieldName,
                                                    PKVOXELS                hVoxels);

PICOGK_API int32_t      VdbFile_nFieldCount(        PKVDBFILE               hVdbFile);

PICOGK_API void         VdbFile_GetFieldName(       PKVDBFILE               hVdbFile,
                                                    int32_t                 nIndex,
                                                    char psz[PKINFOSTRINGLEN]);

PICOGK_API int32_t      VdbFile_nFieldType(         PKVDBFILE               hVdbFile,
                                                    int32_t                 nIndex);

#endif
 

