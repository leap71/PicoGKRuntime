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

PICOGK_API void         Library_GetName(                    char psz[PKINFOSTRINGLEN]);

PICOGK_API void         Library_GetVersion(                 char psz[PKINFOSTRINGLEN]);

PICOGK_API void         Library_GetBuildInfo(               char psz[PKINFOSTRINGLEN]);


#define PKHANDLE        uint64_t
#define PKINSTANCE      PKHANDLE
#define PKMESH          PKHANDLE
#define PKLATTICE       PKHANDLE
#define PKPOLYLINE      PKHANDLE
#define PKVOXELS        PKHANDLE
#define PKVDBFILE       PKHANDLE
#define PKSCALARFIELD   PKHANDLE
#define PKVECTORFIELD   PKHANDLE
#define PKMETADATA      PKHANDLE
#define PKFILEINFO      PKHANDLE

#define PKVIEWER        void*
#define PKGPUTEX        PKHANDLE
#define PKQUAD          PKHANDLE
#define PKGUI           PKHANDLE

// LIBRARY INSTANCE

PICOGK_API PKINSTANCE       Library_hCreateInstance(        float fVoxelSizeMM);

PICOGK_API void             Library_DestroyInstance(        PKINSTANCE hThis);

PICOGK_API int64_t          Library_nTotalMemUsage(         PKINSTANCE hThis);

PICOGK_API int64_t          Library_nMeshesMemUsage(        PKINSTANCE hThis);
PICOGK_API int64_t          Library_nLatticesMemUsage(      PKINSTANCE hThis);
PICOGK_API int64_t          Library_nPolyLinesMemUsage(     PKINSTANCE hThis);
PICOGK_API int64_t          Library_nVoxelsMemUsage(        PKINSTANCE hThis);
PICOGK_API int64_t          Library_nVdbFilesMemUsage(      PKINSTANCE hThis);
PICOGK_API int64_t          Library_nScalarFieldsMemUsage(  PKINSTANCE hThis);
PICOGK_API int64_t          Library_nVectorFieldsMemUsage(  PKINSTANCE hThis);
PICOGK_API int64_t          Library_nVdbMetasMemUsage(      PKINSTANCE hThis);

PICOGK_API int64_t          Library_nMeshesAllocated(       PKINSTANCE hThis);
PICOGK_API int64_t          Library_nLatticesAllocated(     PKINSTANCE hThis);
PICOGK_API int64_t          Library_nPolyLinesAllocated(    PKINSTANCE hThis);
PICOGK_API int64_t          Library_nVoxelsAllocated(       PKINSTANCE hThis);
PICOGK_API int64_t          Library_nVdbFilesAllocated(     PKINSTANCE hThis);
PICOGK_API int64_t          Library_nScalarFieldsAllocated( PKINSTANCE hThis);
PICOGK_API int64_t          Library_nVectorFieldsAllocated( PKINSTANCE hThis);
PICOGK_API int64_t          Library_nVdbMetasAllocated(     PKINSTANCE hThis);

PICOGK_API void             Library_VoxelsToMm(             PKINSTANCE hThis,
                                                            const PKVector3* pvecVoxelCoordinate,
                                                            PKVector3* pvecMmCoordinate);

PICOGK_API void             Library_MmToVoxels(             PKINSTANCE hThis,
                                                            const PKVector3* pvecMmCoordinate,
                                                            PKVector3* pvecVoxelCoordinate);

// MESH

PICOGK_API PKMESH           Mesh_hCreate(                   PKINSTANCE          hInstance);

PICOGK_API PKMESH           Mesh_hCreateFromVoxels(         PKINSTANCE          hInstance,
                                                            PKVOXELS            hVoxels);

PICOGK_API bool             Mesh_bIsValid(                  PKINSTANCE          hInstance,
                                                            PKMESH              hThis);

PICOGK_API void             Mesh_Destroy(                   PKINSTANCE          hInstance,
                                                            PKMESH              hThis);

PICOGK_API int64_t          Mesh_nMemUsage(                 PKINSTANCE          hInstance,
                                                            PKMESH              hThis);

PICOGK_API int32_t          Mesh_nAddVertex(                PKINSTANCE          hInstance,
                                                            PKMESH              hThis,
                                                            const PKVector3*    pvecVertex);

PICOGK_API int32_t          Mesh_nVertexCount(              PKINSTANCE          hInstance,
                                                            PKMESH              hThis);

PICOGK_API void             Mesh_GetVertex(                 PKINSTANCE          hInstance,
                                                            PKMESH              hThis,
                                                            int32_t             nVertex,
                                                            PKVector3*          pvecVertex);

PICOGK_API int32_t          Mesh_nAddTriangle(              PKINSTANCE          hInstance,
                                                            PKMESH              hThis,
                                                            const PKTriangle*   psTri);

PICOGK_API int32_t          Mesh_nTriangleCount(            PKINSTANCE          hInstance,
                                                            PKMESH              hThis);

PICOGK_API void             Mesh_GetTriangle(               PKINSTANCE          hInstance,
                                                            PKMESH              hThis,
                                                            int32_t             nTriangle,
                                                            PKTriangle*         psTri);

PICOGK_API void             Mesh_GetTriangleV(              PKINSTANCE          hInstance,
                                                            PKMESH              hThis,
                                                            int32_t             nTriangle,
                                                            PKVector3*          pvecA,
                                                            PKVector3*          pvecB,
                                                            PKVector3*          pvecC);

PICOGK_API void             Mesh_GetBoundingBox(            PKINSTANCE          hInstance,
                                                            PKMESH              hThis,
                                                            PKBBox3*            poBox);

// LATTICE

PICOGK_API PKLATTICE        Lattice_hCreate(                PKINSTANCE          hInstance);

PICOGK_API int64_t          Lattice_nMemUsage(              PKINSTANCE          hInstance,
                                                            PKLATTICE           hThis);

PICOGK_API bool             Lattice_bIsValid(               PKINSTANCE          hInstance,
                                                            PKLATTICE           hThis);


PICOGK_API void             Lattice_Destroy(                PKINSTANCE          hInstance,
                                                            PKLATTICE           hThis);


PICOGK_API void             Lattice_AddSphere(              PKINSTANCE          hInstance,
                                                            PKLATTICE           hThis,
                                                            const PKVector3*    vecCenter,
                                                            float               fRadius);


PICOGK_API void             Lattice_AddBeam(                PKINSTANCE          hInstance,
                                                            PKLATTICE           hThis,
                                                            const               PKVector3* pvecA,
                                                            const               PKVector3* pvecB,
                                                            float               fRadiusA,
                                                            float               fRadiusB,
                                                            bool                bRoundCap);

// VOXELS

PICOGK_API PKVOXELS         Voxels_hCreate(                 PKINSTANCE          hInstance);

PICOGK_API PKVOXELS         Voxels_hCreateCopy(             PKINSTANCE          hInstance,
                                                            PKVOXELS            hSource);

PICOGK_API PKVOXELS         Voxels_hCreateSphere(           PKINSTANCE          hInstance,
                                                            const PKVector3*    pvecCenter,
                                                            float               fRadius);

PICOGK_API PKVOXELS         Voxels_hCreateCapsule(          PKINSTANCE          hInstance,
                                                            const PKVector3*    pvecStart,
                                                            const PKVector3*    pvecStop,
                                                            float               fRadius1,
                                                            float               fRadius2);

PICOGK_API PKVOXELS         Voxels_hCreateMeshShell(        PKINSTANCE          hLib,
                                                            PKMESH              hMesh,
                                                            float               fRadius);

PICOGK_API bool             Voxels_bIsValid(                PKINSTANCE          hInstance,
                                                            PKVOXELS            hThis);

PICOGK_API void             Voxels_Destroy(                 PKINSTANCE          hInstance,
                                                            PKVOXELS            hThis);

PICOGK_API bool             Voxels_bDiagnose(               PKINSTANCE          hInstance,
                                                            PKVOXELS            hThis,
                                                            char psz[PKINFOSTRINGLEN]);

PICOGK_API bool             Voxels_bIsEmpty(                PKINSTANCE          hInstance,
                                                            PKVOXELS            hThis);

PICOGK_API int64_t          Voxels_nMemUsage(               PKINSTANCE          hInstance,
                                                            PKVOXELS            hThis);

PICOGK_API float            Voxels_fVoxelSize(              PKINSTANCE          hInstance,
                                                            PKVOXELS            hVoxels);

PICOGK_API void             Voxels_BoolAdd(                 PKINSTANCE          hInstance,
                                                            PKVOXELS            hThis,
                                                            PKVOXELS            hOther);

PICOGK_API void             Voxels_BoolSubtract(            PKINSTANCE          hInstance,
                                                            PKVOXELS            hThis,
                                                            PKVOXELS            hOther);

PICOGK_API void             Voxels_BoolIntersect(           PKINSTANCE          hInstance,
                                                            PKVOXELS            hThis,
                                                            PKVOXELS            hOther);

PICOGK_API void             Voxels_Offset(                  PKINSTANCE          hInstance,
                                                            PKVOXELS            hThis,
                                                            float               fDist);

PICOGK_API void             Voxels_DoubleOffset(            PKINSTANCE          hInstance,
                                                            PKVOXELS            hThis,
                                                            float               fDist1,
                                                            float               fDist2);

PICOGK_API void             Voxels_TripleOffset(            PKINSTANCE          hInstance,
                                                            PKVOXELS            hThis,
                                                            float               fDist);


PICOGK_API void             Voxels_RenderMesh(              PKINSTANCE          hInstance,
                                                            PKVOXELS            hThis,
                                                            PKMESH              hMesh);

PICOGK_API void             Voxels_RenderImplicit(          PKINSTANCE          hInstance,
                                                            PKVOXELS            hThis,
                                                            const PKBBox3*      poBBox,
                                                            PKPFnfSdf           pfnSDF);

PICOGK_API void             Voxels_IntersectImplicit(       PKINSTANCE          hInstance,
                                                            PKVOXELS            hThis,
                                                            PKPFnfSdf           pfnSDF);

PICOGK_API void             Voxels_RenderLattice(           PKINSTANCE          hInstance,
                                                            PKVOXELS            hThis,
                                                            PKLATTICE           hLattice);

PICOGK_API void             Voxels_ProjectZSlice(           PKINSTANCE          hInstance,
                                                            PKVOXELS            hThis,
                                                            float               fStartZ,
                                                            float               fEndZ);

PICOGK_API bool             Voxels_bIsInside(               PKINSTANCE          hInstance,
                                                            PKVOXELS            hThis,
                                                            const PKVector3*    pvecTestPoint);

PICOGK_API bool             Voxels_bIsEqual(                PKINSTANCE          hInstance,
                                                            PKVOXELS            hThis,
                                                            PKVOXELS            hOther);

PICOGK_API float            Voxels_fCalculateVolume(        PKINSTANCE          hInstance,
                                                            PKVOXELS            hThis);

PICOGK_API void             Voxels_GetSurfaceNormal(        PKINSTANCE          hInstance,
                                                            PKVOXELS            hThis,
                                                            const PKVector3*    pvecSurfacePoint,
                                                            PKVector3*          pvecNormal);

PICOGK_API bool             Voxels_bClosestPointOnSurface(  PKINSTANCE          hInstance,
                                                            PKVOXELS            hThis,
                                                            const PKVector3*    pvecSearch,
                                                            PKVector3*          pvecSurfacePoint);

PICOGK_API bool             Voxels_bRayCastToSurface(       PKINSTANCE          hInstance,
                                                            PKVOXELS            hThis,
                                                            const PKVector3*    pvecSearch,
                                                            const PKVector3*    pvecDirection,
                                                            PKVector3*          pvecSurfacePoint);

PICOGK_API void             Voxels_GetVoxelDimensions(      PKINSTANCE          hInstance,
                                                            PKVOXELS            hThis,
                                                            int32_t*            pnXOrigin,
                                                            int32_t*            pnYOrigin,
                                                            int32_t*            pnZOrigin,
                                                            int32_t*            pnXSize,
                                                            int32_t*            pnYSize,
                                                            int32_t*            pnZSize);

PICOGK_API void             Voxels_GetXSlice(               PKINSTANCE          hInstance,
                                                            PKVOXELS            hThis,
                                                            int32_t             nXSlice,
                                                            float*              pfBuffer,
                                                            float*              pfBackgroundValue);

PICOGK_API void             Voxels_GetYSlice(               PKINSTANCE          hInstance,
                                                            PKVOXELS            hThis,
                                                            int32_t             nYSlice,
                                                            float*              pfBuffer,
                                                            float*              pfBackgroundValue);

PICOGK_API void             Voxels_GetZSlice(               PKINSTANCE          hInstance,
                                                            PKVOXELS            hThis,
                                                            int32_t             nZSlice,
                                                            float*              pfBuffer,
                                                            float*              pfBackgroundValue);

PICOGK_API void Voxels_GetInterpolatedZSlice(               PKINSTANCE hLib,
                                                            PKVOXELS    hThis,
                                                            float       fZSlice,
                                                            float*      pfBuffer,
                                                            float*      pfBackgroundValue);

// POLYLINE

PICOGK_API PKPOLYLINE       PolyLine_hCreate(               PKINSTANCE          hInstance,
                                                            const PKColorFloat* pclr);

PICOGK_API bool             PolyLine_bIsValid(              PKINSTANCE          hInstance,
                                                            PKPOLYLINE          hThis);

PICOGK_API void             PolyLine_Destroy(               PKINSTANCE          hInstance,
                                                            PKPOLYLINE          hThis);

PICOGK_API int64_t          PolyLine_nMemUsage(             PKINSTANCE          hInstance,
                                                            PKPOLYLINE          hThis);

PICOGK_API int32_t          PolyLine_nAddVertex(            PKINSTANCE          hInstance,
                                                            PKPOLYLINE          hThis,
                                                            const PKVector3*    pvec);

PICOGK_API int32_t          PolyLine_nVertexCount(          PKINSTANCE          hInstance,
                                                            PKPOLYLINE          hThis);

PICOGK_API void             PolyLine_GetVertex(             PKINSTANCE          hInstance,
                                                            PKPOLYLINE          hThis,
                                                            int32_t             nIndex,
                                                            PKVector3*          pvec);

PICOGK_API void             PolyLine_GetColor(              PKINSTANCE          hInstance,
                                                            PKPOLYLINE          hThis,
                                                            PKColorFloat*       pclr);

PICOGK_API void             PolyLine_GetBoundingBox(        PKINSTANCE          hLib,
                                                            PKPOLYLINE          hThis,
                                                            PKBBox3*            poBox);

// VDB FILE

PICOGK_API PKVDBFILE        VdbFile_hCreate(                PKINSTANCE          hInstance);

PICOGK_API PKVDBFILE        VdbFile_hCreateFromFile(        PKINSTANCE          hInstance,
                                                            const char*         pszFileName);

PICOGK_API bool             VdbFile_bIsValid(               PKINSTANCE          hInstance,
                                                            PKVDBFILE           hThis);

PICOGK_API void             VdbFile_Destroy(                PKINSTANCE          hInstance,
                                                            PKVDBFILE           hThis);

PICOGK_API int64_t          VdbFile_nMemUsage(              PKINSTANCE          hInstance,
                                                            PKVDBFILE           hThis);

PICOGK_API bool             VdbFile_bSaveToFile(            PKINSTANCE          hInstance,
                                                            PKVDBFILE           hVdbFile,
                                                            const char*         pszFileName);

PICOGK_API PKVOXELS         VdbFile_hGetVoxels(             PKINSTANCE          hInstance,
                                                            PKVDBFILE           hVdbFile,
                                                            int32_t             nIndex);

PICOGK_API int32_t          VdbFile_nAddVoxels(             PKINSTANCE          hInstance,
                                                            PKVDBFILE           hVdbFile,
                                                            const char*         pszFieldName,
                                                            PKVOXELS            hVoxels);

PICOGK_API PKSCALARFIELD    VdbFile_hGetScalarField(        PKINSTANCE          hInstance,
                                                            PKVDBFILE           hVdbFile,
                                                            int32_t             nIndex);

PICOGK_API int32_t          VdbFile_nAddScalarField(        PKINSTANCE          hInstance,
                                                            PKVDBFILE           hVdbFile,
                                                            const char*         pszFieldName,
                                                            PKSCALARFIELD       hScalarField);

PICOGK_API PKVECTORFIELD    VdbFile_hGetVectorField(        PKINSTANCE          hInstance,
                                                            PKVDBFILE           hVdbFile,
                                                            int32_t             nIndex);

PICOGK_API int32_t          VdbFile_nAddVectorField(        PKINSTANCE          hInstance,
                                                            PKVDBFILE           hVdbFile,
                                                            const char*         pszFieldName,
                                                            PKVECTORFIELD       hVectorField);

PICOGK_API int32_t          VdbFile_nFieldCount(            PKINSTANCE          hInstance,
                                                            PKVDBFILE           hVdbFile);

PICOGK_API void             VdbFile_GetFieldName(           PKINSTANCE          hInstance,
                                                            PKVDBFILE           hVdbFile,
                                                            int32_t             nIndex,
                                                            char psz[PKINFOSTRINGLEN]);

PICOGK_API int32_t          VdbFile_nFieldType(             PKINSTANCE          hInstance,
                                                            PKVDBFILE           hVdbFile,
                                                            int32_t             nIndex);

PICOGK_API PKSCALARFIELD    ScalarField_hCreate(            PKINSTANCE          hInstance);

PICOGK_API PKSCALARFIELD    ScalarField_hCreateCopy(        PKINSTANCE          hInstance,
                                                            PKSCALARFIELD       hSource);

PICOGK_API PKSCALARFIELD    ScalarField_hCreateFromVoxels(  PKINSTANCE          hInstance,
                                                            PKVOXELS            hVoxels);

PICOGK_API PKSCALARFIELD    ScalarField_hBuildFromVoxels(   PKINSTANCE          hInstance,
                                                            PKVOXELS            hVoxels,
                                                            float               fScalarValue,
                                                            float               fSdThreshold);

PICOGK_API bool             ScalarField_bIsValid(           PKINSTANCE          hInstance,
                                                            PKSCALARFIELD       hThis);

PICOGK_API void             ScalarField_Destroy(            PKINSTANCE          hInstance,
                                                            PKSCALARFIELD       hThis);

PICOGK_API int64_t          ScalarField_nMemUsage(          PKINSTANCE          hInstance,
                                                            PKSCALARFIELD       hThis);

PICOGK_API void             ScalarField_SetValue(           PKINSTANCE          hInstance,
                                                            PKSCALARFIELD       hThis,
                                                            const PKVector3*    pvecPosition,
                                                            float               fValue);

PICOGK_API bool             ScalarField_bGetValue(          PKINSTANCE          hInstance,
                                                            PKSCALARFIELD       hThis,
                                                            const PKVector3*    pvecPosition,
                                                            float*              pfValue);

PICOGK_API void             ScalarField_RemoveValue(        PKINSTANCE          hInstance,
                                                            PKVECTORFIELD       hThis,
                                                            const PKVector3*    pvecPosition);

PICOGK_API void             ScalarField_GetVoxelDimensions( PKINSTANCE          hInstance,
                                                            PKSCALARFIELD       hThis,
                                                            int32_t*            pnXOrigin,
                                                            int32_t*            pnYOrigin,
                                                            int32_t*            pnZOrigin,
                                                            int32_t*            pnXSize,
                                                            int32_t*            pnYSize,
                                                            int32_t*            pnZSize);

PICOGK_API void             ScalarField_GetSlice(           PKINSTANCE          hInstance,
                                                            PKSCALARFIELD       hThis,
                                                            int32_t             nZSlice,
                                                            float*              pfBuffer);

PICOGK_API void             ScalarField_TraverseActive(     PKINSTANCE          hInstance,
                                                            PKSCALARFIELD       hThis,
                                                            PKFnTraverseActiveS pfnCallback);

PICOGK_API PKVECTORFIELD    VectorField_hCreate(            PKINSTANCE          hInstance);

PICOGK_API PKVECTORFIELD    VectorField_hCreateCopy(        PKINSTANCE          hInstance,
                                                            PKVECTORFIELD       hSource);

PICOGK_API PKVECTORFIELD    VectorField_hCreateFromVoxels(  PKINSTANCE          hInstance,
                                                            PKVOXELS            hVoxels);

PICOGK_API PKVECTORFIELD    VectorField_hBuildFromVoxels(   PKINSTANCE          hInstance,
                                                            PKVOXELS            hVoxels,
                                                            const PKVector3*    vecValue,
                                                            float               fSdThreshold);

PICOGK_API bool             VectorField_bIsValid(           PKINSTANCE          hInstance,
                                                            PKVECTORFIELD       hThis);

PICOGK_API void             VectorField_Destroy(            PKINSTANCE          hInstance,
                                                            PKVECTORFIELD       hThis);

PICOGK_API int64_t          VectorField_nMemUsage(          PKINSTANCE          hInstance,
                                                            PKVECTORFIELD       hThis);

PICOGK_API void             VectorField_SetValue(           PKINSTANCE          hInstance,
                                                            PKVECTORFIELD       hThis,
                                                            const PKVector3*    pvecPosition,
                                                            const PKVector3*    pvecValue);

PICOGK_API bool             VectorField_bGetValue(          PKINSTANCE          hInstance,
                                                            PKVECTORFIELD       hThis,
                                                            const PKVector3*    pvecPosition,
                                                            PKVector3*          pvecValue);

PICOGK_API void             VectorField_RemoveValue(        PKINSTANCE          hInstance,
                                                            PKVECTORFIELD       hThis,
                                                            const PKVector3*    pvecPosition);

PICOGK_API void             VectorField_TraverseActive(     PKINSTANCE          hInstance,
                                                            PKVECTORFIELD       hThis,
                                                            PKFnTraverseActiveV pfnCallback);

PICOGK_API PKMETADATA       Metadata_hFromVoxels(           PKINSTANCE          hInstance,
                                                            PKVOXELS            hVoxels);

PICOGK_API PKMETADATA       Metadata_hFromScalarField(      PKINSTANCE          hInstance,
                                                            PKSCALARFIELD       hScalarField);

PICOGK_API PKMETADATA       Metadata_hFromVectorField(      PKINSTANCE          hInstance,
                                                            PKVECTORFIELD       hVectorField);

PICOGK_API void             Metadata_Destroy(               PKINSTANCE          hInstance,
                                                            PKMETADATA          hThis);

PICOGK_API int32_t          Metadata_nCount(                PKINSTANCE          hInstance,
                                                            PKMETADATA          hThis);

PICOGK_API int32_t          Metadata_nNameLengthAt(         PKINSTANCE          hInstance,
                                                            PKMETADATA          hThis,
                                                            int32_t             nIndex);

PICOGK_API bool             Metadata_bGetNameAt(            PKINSTANCE          hInstance,
                                                            PKMETADATA          hThis,
                                                            int32_t             nIndex,
                                                            char*               psz,
                                                            int32_t             nMaxStringLen);

PICOGK_API int32_t          Metadata_nTypeAt(               PKINSTANCE          hInstance,
                                                            PKMETADATA          hThis,
                                                            const char*         psz);

PICOGK_API int32_t          Metadata_nStringLengthAt(       PKINSTANCE          hInstance,
                                                            PKMETADATA          hThis,
                                                            const char*         psz);

PICOGK_API bool             Metadata_bGetStringAt(          PKINSTANCE          hInstance,
                                                            PKMETADATA          hThis,
                                                            const char*         psz,
                                                            char*               pszValue,
                                                            int32_t             nMaxStringLen);

PICOGK_API bool             Metadata_bGetFloatAt(           PKINSTANCE          hInstance,
                                                            PKMETADATA          hThis,
                                                            const char*         psz,
                                                            float*              pfValue);

PICOGK_API bool             Metadata_bGetVectorAt(          PKINSTANCE          hInstance,
                                                            PKMETADATA          hThis,
                                                            const char*         psz,
                                                            PKVector3*          pvecValue);

PICOGK_API void             Metadata_SetStringValue(        PKINSTANCE          hInstance,
                                                            PKMETADATA          hThis,
                                                            const char*         pszFieldName,
                                                            const char*         pszValue);

PICOGK_API void             Metadata_SetFloatValue(         PKINSTANCE          hInstance,
                                                            PKMETADATA          hThis,
                                                            const char*         pszFieldName,
                                                            float               fValue);

PICOGK_API void             Metadata_SetVectorValue(        PKINSTANCE          hInstance,
                                                            PKMETADATA          hThis,
                                                            const char*         pszFieldName,
                                                            const PKVector3*    pvecValue);

PICOGK_API void             MetaData_RemoveValue(           PKINSTANCE          hInstance,
                                                            PKMETADATA          hThis,
                                                            const char*         pszFieldName);

// VIEWER

PICOGK_API PKVIEWER         Viewer_hCreate(                 const char*         pszWindowTitle,
                                                            const PKVector2*    pvecSize,
                                                            PKFInfo             pfnInfoCallback,
                                                            PKPFUpdateRequested pfnUpdateCallback,
                                                            PKPFKeyPressed      pfnKeyPressedCallback,
                                                            PKPFMouseMoved      pfnMouseMoveCallback,
                                                            PKPFMouseButton     pfnMouseButtonCallback,
                                                            PKPFScrollWheel     pfnScrollWheelCallback,
                                                            PKPFWindowSize      pfnWindowSize);

PICOGK_API  bool            Viewer_bIsValid(                PKVIEWER            hThis);

PICOGK_API  void            Viewer_Destroy(                 PKVIEWER            hThis);

PICOGK_API  void            Viewer_RequestUpdate(           PKVIEWER            hThis);

PICOGK_API  bool            Viewer_bPoll(                   PKVIEWER            hThis);

PICOGK_API  void            Viewer_RequestScreenShot(       PKVIEWER            hThis,
                                                            const char*         pszScreenShotPath);

PICOGK_API void             Viewer_EnableExperimental(      PKVIEWER            hThis,
                                                            bool                bEnable);

PICOGK_API  void            Viewer_RequestClose(            PKVIEWER            hThis);

PICOGK_API bool             Viewer_bLoadLightSetup(         PKVIEWER            hThis,
                                                            const char*         pDiffTextureDDS,
                                                            int32_t             nDiffTextureSize,
                                                            const char*         pSpecTextureDDS,
                                                            int32_t             nSpecTextureSize);

PICOGK_API  void            Viewer_AddMesh(                 PKINSTANCE          hLib,
                                                            PKVIEWER            hThis,
                                                            int32_t             nGroupID,
                                                            PKMESH              hMesh);

PICOGK_API  void            Viewer_RemoveMesh(              PKINSTANCE          hLib,
                                                            PKVIEWER            hThis,
                                                            PKMESH              hMesh);

PICOGK_API  void            Viewer_SetMeshMatrix(           PKINSTANCE          hLib,
                                                            PKVIEWER            hThis,
                                                            PKMESH              hMesh,
                                                            const PKMatrix4x4*  pmat);

PICOGK_API  void            Viewer_AddVoxels(               PKINSTANCE          hLib,
                                                            PKVIEWER            hThis,
                                                            int32_t             nGroupID,
                                                            PKVOXELS            hVoxels);

PICOGK_API  void            Viewer_RemoveVoxels(            PKINSTANCE          hLib,
                                                            PKVIEWER            hThis,
                                                            PKVOXELS            hVoxels);

PICOGK_API  void            Viewer_SetVoxelsMatrix(         PKINSTANCE          hLib,
                                                            PKVIEWER            hThis,
                                                            PKVOXELS            hVoxels,
                                                            const PKMatrix4x4*  pmat);

PICOGK_API  void            Viewer_AddPolyLine(             PKINSTANCE          hLib,
                                                            PKVIEWER            hThis,
                                                            int32_t             nGroupID,
                                                            PKPOLYLINE          hPolyLine);

PICOGK_API  void            Viewer_RemovePolyLine(          PKINSTANCE          hLib,
                                                            PKVIEWER            hThis,
                                                            PKPOLYLINE          hPolyLine);

PICOGK_API  void            Viewer_SetPolyLineMatrix(       PKINSTANCE          hLib,
                                                            PKVIEWER            hThis,
                                                            PKPOLYLINE          hPolyLine,
                                                            const PKMatrix4x4*  pmat);

PICOGK_API  void            Viewer_RemoveAllObjects(        PKVIEWER            hThis);

PICOGK_API  void            Viewer_SetGroupVisible(         PKVIEWER            hThis,
                                                            int32_t             nGroupID,
                                                            bool                bVisible);

PICOGK_API  void            Viewer_SetGroupMaterial(        PKVIEWER            hThis,
                                                            int32_t             nGroupID,
                                                            const PKColorFloat* pclr,
                                                            float               fMetallic,
                                                            float               fRoughness);

PICOGK_API void             Viewer_SetGroupMatrix(          PKVIEWER            hThis,
                                                            int32_t             nGroupID,
                                                            const PKMatrix4x4*  pmat);

PICOGK_API void             Viewer_EnableGroupWarnOverhang( PKVIEWER    hThis,
                                                            int32_t     nGroupID,
                                                            float       fWarning,
                                                            float       fError);

PICOGK_API void             Viewer_DisableGroupWarnOverhang(PKVIEWER    hThis,
                                                            int32_t     nGroupID);

PICOGK_API void             Viewer_GetBoundingBox(          PKVIEWER            hThis,
                                                            PKBBox3*            poBox);

PICOGK_API PKGPUTEX         Viewer_GpuTex_hCreate(          PKVIEWER            hThis,
                                                            int                 nWidth,
                                                            int                 nHeight,
                                                            const char*         pRgba8);

PICOGK_API void             Viewer_GpuTex_Refresh(          PKVIEWER            hThis,
                                                            PKGPUTEX            hTex,
                                                            const char*         pRgba8);

PICOGK_API void             Viewer_GpuTex_MarkForCleanup(   PKVIEWER            hThis,
                                                            PKGPUTEX);

PICOGK_API PKQUAD           Viewer_Quad_hCreate(            PKVIEWER            hThis,
                                                            PKGPUTEX            hTexDefault,
                                                            PKColorFloat        clrDefault,
                                                            float               fAlpha,
                                                            const PKMatrix4x4*  pmat,
                                                            bool                bFlipX,
                                                            bool                bFlipY,
                                                            bool                bDoubleSided);

PICOGK_API void             Viewer_Quad_Destroy(            PKVIEWER    hThis,
                                                            PKQUAD      hQuad);

PICOGK_API void             Viewer_Quad_SetMatrix(          PKVIEWER            hThis,
                                                            PKQUAD              hQuad,
                                                            const PKMatrix4x4*  pmat);

PICOGK_API PKGUI            Viewer_SideBar_hCreate(         PKVIEWER            hThis,
                                                            bool                bLeft,
                                                            int                 nMin,
                                                            int                 nMax,
                                                            int                 nDef,
                                                            PKColorFloat       clrBackground,
                                                            PKColorFloat       clrBackgroundHv);

PICOGK_API void             Viewer_SideBar_Destroy(         PKVIEWER            hThis,
                                                            PKGUI               hSideBar);

#endif
 

