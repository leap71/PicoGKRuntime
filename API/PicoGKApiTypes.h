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

#ifndef PICOGKGLAPITYPES_H_
#define PICOGKGLAPITYPES_H_

// Check if the "rich PicoGK Types have been included
// if they haven't, define types with the same memory
// layout for export

#ifdef PICOGKTYPES_INCLUDED

#define PKCoord         PicoGK::Coord
#define PKColorFloat    PicoGK::ColorFloat
#define PKVector2       PicoGK::Vector2
#define PKVector3       PicoGK::Vector3
#define PKVector4       PicoGK::Vector4
#define PKMatrix4x4     PicoGK::Matrix4x4
#define PKTransform     PicoGK::Transform
#define PKTriangle      PicoGK::Triangle
#define PKBBox3         PicoGK::BBox3

#else

#include  <cstdint>

#pragma pack(1)
struct PKTriangle
{
    int32_t A;
    int32_t B;
    int32_t C;
};

struct PKCoord
{
    int32_t X;
    int32_t Y;
    int32_t Z;
};

struct PKVector2
{
    float X;
    float Y;
};

struct PKVector3
{
    float X;
    float Y;
    float Z;
};

struct PKVector4
{
    float X;
    float Y;
    float Z;
    float W;
};

struct PKBBox3
{
    PKVector3 vecMin;
    PKVector3 vecMax;
};

struct PKMatrix4x4
{
    PKVector4 vec1;
    PKVector4 vec2;
    PKVector4 vec3;
    PKVector4 vec4;
};

struct PKColorFloat
{
    float R;
    float G;
    float B;
    float A;
};

#pragma pack()

#endif // Definition of basic types

// Implicit rendering callback function

typedef float (*PKPFnfSdf)(const PKVector3* pvecCoord);

// Callback iterating across active voxels

typedef void (*PKFnTraverseActiveS)(    const PKVector3* pvecCoord,
                                        float fValue);

typedef void (*PKFnTraverseActiveV)(    const PKVector3* pvecCoord,
                                        const PKVector3* pvecValue);

// Viewer callbacks

typedef void (*PKFInfo)(                const char*         pszMessage,
                                        bool                bFatalError);

typedef void (*PKPFUpdateRequested)(    void*               poViewer,
                                        const PKVector2*    pvecViewport,
                                        PKColorFloat*       pclrBackground,
                                        PKMatrix4x4*        pmatModelViewProjection,
                                        PKMatrix4x4*        pmatModelTransform,
                                        PKMatrix4x4*        pmatStatic,
                                        PKVector3*          pvecEyePosition,
                                        PKVector3*          pvecEyeStatic);

typedef void (*PKPFKeyPressed)(         void*               poViewer,
                                        int32_t             iKey,
                                        int32_t             iScancode,
                                        int32_t             iAction,
                                        int32_t             iModifiers);

typedef void (*PKPFMouseMoved)(         void*               poViewer,
                                        const PKVector2*    pvecMousePos);

typedef void (*PKPFMouseButton)(        void*               poViewer,
                                        int32_t             iButton,
                                        int32_t             iAction,
                                        int32_t             iModifiers,
                                        const PKVector2*    pvecMousePos);

typedef void (*PKPFScrollWheel)(        void*               poViewer,
                                        const PKVector2*    pvecOffset,
                                        const PKVector2*    pvecMousePos);

typedef void (*PKPFWindowSize)(         void*               poViewer,
                                        const PKVector2*    pvecWindowSize);

#endif
