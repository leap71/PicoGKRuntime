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

#ifndef PICOGKTYPES_H_
#define PICOGKTYPES_H_

#define PICOGKTYPES_INCLUDED
// Used in PicoGKApiTypes, to avoid redefinition of basic types

#include <limits>
#include <algorithm>
#include <cmath>

#define PKINIT(var)                 m_##var(var)
#define PKSHAREDPTR(class)          typedef std::shared_ptr<class> Ptr;

#define PKUNUSED(x)                 ( (void)(x) )

#define PKSTRINGIFY(x)              #x
#define PKEXPAND_AND_STRINGIFY(x)   PKSTRINGIFY(x)

namespace PicoGK
{

#pragma pack(1)

struct ColorFloat
{
    float R;
    float G;
    float B;
    float A;
};

struct Coord
{
public:
    Coord(  int32_t _x,
            int32_t _y,
            int32_t _z)
    {
        X = _x;
        Y = _y;
        Z = _z;
    }
    
    Coord()
    {
#ifdef _DEBUG
        X = std::numeric_limits<int32_t>::min();
        Y = std::numeric_limits<int32_t>::min();
        Z = std::numeric_limits<int32_t>::min();
#endif
    }
    
    Coord(const Coord& oSrc)
    {
        X = oSrc.X;
        Y = oSrc.Y;
        Z = oSrc.Z;
    }
    
    bool operator==(const Coord& xyz) const
    {
        return (    (xyz.X == X) &&
                    (xyz.Y == Y) &&
                    (xyz.Z == Z));
    }
    
    union
    {
        struct
        {
            int32_t X;
            int32_t Y;
            int32_t Z;
        };
        
        struct
        {
            int32_t A;
            int32_t B;
            int32_t C;
        };
    };
    
};

typedef Coord Triangle;

struct Vector2
{
    float X;
    float Y;
};

struct Vector3
{
    Vector3(    float fX,
                float fY,
                float fZ)
    {
        X = fX;
        Y = fY;
        Z = fZ;
    }
    
    Vector3(const Vector3& vec) : Vector3(vec.X, vec.Y, vec.Z)
    {
    }

    Vector3(    int32_t iX,
                int32_t iY,
                int32_t iZ)
    {
        X = (float) iX;
        Y = (float) iY;
        Z = (float) iZ;
    }
    
    inline bool bAlmostEqual(   const Vector3& sOp,
                                float fEpsilon = std::numeric_limits<float>::epsilon() * 4.0f) const
    {
       return ( (std::abs(X - sOp.X) <= fEpsilon) &&
                (std::abs(Y - sOp.Y) <= fEpsilon) &&
                (std::abs(Z - sOp.Z) <= fEpsilon));
    }
    
    inline Vector3 vecCross(const Vector3& sOp) const
    {
        return Vector3( Y * sOp.Z - Z * sOp.Y,
                        Z * sOp.X - X * sOp.Z,
                        X * sOp.Y - Y * sOp.X);
    }
    
    inline Vector3 operator-(const Vector3& sOp) const
    {
        return Vector3(X - sOp.X, Y - sOp.Y, Z - sOp.Z);
    }
    
    inline Vector3 operator+(const Vector3& sOp) const
    {
        return Vector3(X + sOp.X, Y + sOp.Y, Z + sOp.Z);
    }
    
    inline Vector3 operator *(float f) const
    {
        return Vector3( X * f,
                        Y * f,
                        Z * f);
    }
    
    inline Vector3 operator *=(float f)
    {
        X *= f;
        Y *= f;
        Z *= f;
        return *this;
    }
    
    inline Vector3 operator /(float f) const
    {
        return Vector3( X / f,
                        Y / f,
                        Z / f);
    }
    
    inline Vector3 operator /=(float f)
    {
        X /= f;
        Y /= f;
        Z /= f;
        return *this;
    }
    
    inline float fDot(const Vector3& vec) const
    {
        float f = 0.0f;
           
        for(int n=0;n<3;n++)
            f += v[n] * vec.v[n];
        
        return f;
    }
    
    inline float fLength() const
    {
        return std::sqrt(fLengthSquared());
    }
    
    inline float fLengthSquared() const
    {
        return  X * X +
                Y * Y +
                Z * Z;
    }
    
    inline void Normalize()
    {
        float length = fLength();
        
        if (length <= 0.0f)
            return;

        X /= length;
        Y /= length;
        Z /= length;
    }
    
    union
    {
        struct
        {
            float X;
            float Y;
            float Z;
        };
        
        float v[3];
    };
};

struct Vector4
{
    float X;
    float Y;
    float Z;
    float W;
    
    Vector4(    float fX,
                float fY,
                float fZ,
                float fW)
    : X(fX), Y(fY), Z(fZ), W(fW)
    {
        
    }
};

struct Matrix4x4
{
    union
    {
        struct
        {
            Vector4 vec1;
            Vector4 vec2;
            Vector4 vec3;
            Vector4 vec4;
        };
        
        float   m[16];
    };
        
    Matrix4x4()
    : vec1(Vector4(1.0f, 0.0f, 0.0f, 0.0f)),
      vec2(Vector4(0.0f, 1.0f, 0.0f, 0.0f)),
      vec3(Vector4(0.0f, 0.0f, 1.0f, 0.0f)),
      vec4(Vector4(0.0f, 0.0f, 0.0f, 1.0f))
    {
            
    }
    
    void operator*=(const Matrix4x4& other)
    {
        Matrix4x4 result;

        for (int col = 0; col < 4; ++col)
        {
            for (int row = 0; row < 4; ++row)
            {
                float sum = 0.0f;
                for (int i = 0; i < 4; ++i)
                {
                    sum += m[row + i * 4] * other.m[col * 4 + i];
                }
                result.m[col * 4 + row] = sum;
            }
        }

        *this = result; // Update the current matrix with the result
    }
};

struct BBox3
{
    inline BBox3()
    :   vecMin( std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max()),
    
        vecMax( std::numeric_limits<float>::lowest(),
                std::numeric_limits<float>::lowest(),
                std::numeric_limits<float>::lowest())
    {
        
    }
    
    inline bool bIsEmpty() const
    {
        return vecMin.X == std::numeric_limits<float>::max();
    }
    
    inline void Include(const Vector3& vec)
    {
        for (int n=0;n<3;n++)
        {
            vecMin.v[n] = std::min<float>(vec.v[n], vecMin.v[n]);
            vecMax.v[n] = std::max<float>(vec.v[n], vecMax.v[n]);
        }
    }
    
    inline void Include(const BBox3& oBB)
    {
        for (int n=0;n<3;n++)
        {
            vecMin.v[n] = std::min<float>(oBB.vecMin.v[n], vecMin.v[n]);
            vecMax.v[n] = std::min<float>(oBB.vecMax.v[n], vecMax.v[n]);
        }
    }
    
    inline void Grow(float f)
    {
        vecMin = vecMin - Vector3(f,f,f);
        vecMax = vecMax + Vector3(f,f,f);
    }

    Vector3   vecMin;
    Vector3   vecMax;
};

class VoxelSize
{
public:
    VoxelSize(float f)
    {
        m_fVoxelSizeMM = f;
    }
    
    operator float()
    {
        return m_fVoxelSizeMM;
    }
    
    Coord xyzToVoxels(const Vector3& vecMM)
    {
        Coord xyzVoxels(    iToVoxels(vecMM.X),
                            iToVoxels(vecMM.Y),
                            iToVoxels(vecMM.Z));
        
        return xyzVoxels;
    }

    inline int32_t iToVoxels(float fMM)
    {
        return (int) 0.5f + fToVoxels(fMM);
    }
    
    inline float fToVoxels(float fMM)
    {
        return fMM / m_fVoxelSizeMM;
    }

    Vector3 vecToMM(const Coord& xyzVoxels)
    {
        Vector3 vecMM(  fToMM(xyzVoxels.X),
                        fToMM(xyzVoxels.Y),
                        fToMM(xyzVoxels.Z));
        
        return vecMM;
    }
    
    Vector3 vecToVoxels(const Vector3& vecMM)
    {
        Vector3 vecVox( fToVoxels(vecMM.X),
                        fToVoxels(vecMM.Y),
                        fToVoxels(vecMM.Z));
        
        return vecVox;
    }

    float fToMM(int iVoxels)
    {
        return iVoxels * m_fVoxelSizeMM;
    }
    
    float m_fVoxelSizeMM;
};

#pragma pack()

class Math
{
public:
    inline static int32_t iSign(float f)
    {
        return (0.0f < f) - (f < 0.0f);
    }
    
    inline static float fClamp(float fValue, float fMinValue, float fMaxValue)
    {
        fValue = std::min(fValue, fMaxValue);
        fValue = std::max(fValue, fMinValue);
        return fValue;
    }
};
    
} // namespace PicoGK

#endif

