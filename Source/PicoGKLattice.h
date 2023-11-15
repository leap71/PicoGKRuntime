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

#ifndef PICOGKLATTICE_H_
#define PICOGKLATTICE_H_

#include "PicoGKTypes.h"
#include <memory>
#include <algorithm>
#include <vector>

namespace PicoGK
{

class LatticeSphere
{
public:
    PKSHAREDPTR(LatticeSphere);
    
    LatticeSphere(  const Vector3& vecCenter,
                    float fRadius)
    : m_vecC(vecCenter)
    {
        m_fRad = fRadius;
        
        m_oBBox.Include(m_vecC);
        m_oBBox.Grow(m_fRad);
    }
    
    inline float fSdValue(const Vector3& vecSample) const
    {
        return (vecSample - m_vecC).fLength() - m_fRad;
    }
    
    inline Vector3 vecMin() const           {return m_oBBox.vecMin;}
    
    inline Vector3 vecMax() const           {return m_oBBox.vecMax;}
    
    inline const BBox3& oBBox() const       {return m_oBBox;}
    
protected:
    
    Vector3 m_vecC;
    float   m_fRad;
    BBox3   m_oBBox;
};


class LatticeBeam
{
public:
    PKSHAREDPTR(LatticeBeam);
    
    LatticeBeam(    const Vector3& vecA,
                    const Vector3& vecB,
                    float fRadiusA,
                    float fRadiusB,
                    bool bRoundCap)
    :   m_vecS(vecA),
        m_vecE(vecB),
        m_fRadS(fRadiusA),
        m_fRadE(fRadiusB),
        m_bRoundCap(bRoundCap)
    {
        m_oBBox.Include(m_vecS);
        m_oBBox.Include(m_vecE);
        m_oBBox.Grow(std::max<float>(m_fRadS, m_fRadE));
    }
    
    inline float fSdValue(const Vector3& vecSample) const
    {
        if (m_bRoundCap)
            return fSdvRoundCone(vecSample);
        
        return fSdvFlatCone(vecSample);
    }
    
    inline Vector3 vecMin() const           {return m_oBBox.vecMin;}
    
    inline Vector3 vecMax() const           {return m_oBBox.vecMax;}
    
    inline const BBox3& oBBox() const       {return m_oBBox;}
    
protected:
    
    float fSdvRoundCone(const Vector3& p) const
    {
        Vector3 b = m_vecE;
        Vector3 a = m_vecS;
        float r1 = m_fRadS;
        float r2 = m_fRadE;
        
        // sampling independent computations (only depend on shape)
        Vector3 ba = b - a;
        float l2 = ba.fDot(ba);
        float rr = r1 - r2;
        float a2 = l2 - rr * rr;
        float il2 = 1.0f / l2;
        
        // sampling dependant computations
        Vector3 pa = p - a;
        float y = pa.fDot(ba);
        float z = y - l2;
        float x2 = (pa * l2 - ba * y).fDot((pa * l2 - ba * y));
        float y2 = y * y * l2;
        float z2 = z * z * l2;
        
        // single square root!
        float k = Math::iSign(rr) * rr * rr * x2;
        
        if (Math::iSign(z) * a2 * z2 > k)
            return sqrtf(x2 + z2) * il2 - r2;
        
        if (Math::iSign(y) * a2 * y2 < k)
            return sqrtf(x2 + y2) * il2 - r1;
        
        return (sqrtf(x2 * a2 * il2) + y * rr) * il2 - r1;
    }
    
    float fSdvFlatCone(const Vector3& p) const
    {
        Vector3 b = m_vecE;
        Vector3 a = m_vecS;
        float ra = m_fRadS;
        float rb = m_fRadE;
        
        float rba = rb - ra;
        float baba = (b - a).fDot(b - a);
        float papa = (p - a).fDot(p - a);
        float paba = (p - a).fDot(b - a) / baba;
        float x = std::sqrt(papa - paba * paba * baba);
        float cax = std::max(0.0f, (float) (x - ((paba < 0.5f) ? ra : rb)));
        float cay = std::abs(paba - 0.5f) - 0.5f;
        float k = rba * rba + baba;
        float f = Math::fClamp((rba * (x - ra) + paba * baba) / k, 0.0f, 1.0f);
        float cbx = x - ra - f * rba;
        float cby = paba - f;
        float s = (cbx < 0.0 && cay < 0.0) ? -1.0f : 1.0f;
        return s * sqrtf(std::min(  cax * cax + cay * cay * baba,
                                    cbx * cbx + cby * cby * baba));
    }
    
    Vector3 m_vecS;
    Vector3 m_vecE;
    float   m_fRadS;
    float   m_fRadE;
    bool    m_bRoundCap;
    BBox3   m_oBBox;
};

class Lattice
{
public:
    PKSHAREDPTR(Lattice);
    
    void AddSphere( Vector3 vecCenter,
                    float fRadius)
    {
        LatticeSphere::Ptr roSphere = std::make_shared<LatticeSphere>(vecCenter, fRadius);
        m_oBBox.Include(roSphere->oBBox());
        m_oSpheres.push_back(roSphere);
    }
 
    void AddBeam(   Vector3 vecS,
                    Vector3 vecE,
                    float fRadS,
                    float fRadE,
                    bool bRoundCap)
    {
        if (bRoundCap && ((vecS-vecE).fLength() < std::numeric_limits<float>::epsilon()))
        {
            // this is a sphere...
            AddSphere(vecS, std::max(fRadS, fRadE));
            return;
        }
        
        LatticeBeam::Ptr roBeam = std::make_shared<LatticeBeam>(    vecS,
                                                                    vecE,
                                                                    fRadS,
                                                                    fRadE,
                                                                    bRoundCap);
        m_oBBox.Include(roBeam->oBBox());
        m_oBeams.push_back(roBeam);
    }
    
    inline const BBox3& oBBox() const
    {
        return m_oBBox;
    }
    
    inline const std::vector<LatticeBeam::Ptr>& oBeams() const
    {
        return m_oBeams;
    }
    
    inline const std::vector<LatticeSphere::Ptr>& oSpheres() const
    {
        return m_oSpheres;
    }
    
protected:
    std::vector<LatticeBeam::Ptr>   m_oBeams;
    std::vector<LatticeSphere::Ptr> m_oSpheres;
    BBox3                           m_oBBox;
};

} //

#endif
