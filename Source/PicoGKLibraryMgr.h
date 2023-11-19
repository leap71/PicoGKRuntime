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

#ifndef PICOGKLIBRARYMGR_H_
#define PICOGKLIBRARYMGR_H_

#include "PicoGKBuild.h"
#include <string>
#include <map>

#include "PicoGKMesh.h"
#include "PicoGKLattice.h"
#include "PicoGKPolyLine.h"
#include "PicoGKVdbVoxels.h"

namespace PicoGK
{
class Library
{
    
public:
    
    inline static Library& oLib()
    {
        static Library  m_oLib;
        return m_oLib;
    }
    
    void InitLibrary(float fVoxelSizeMM)
    {
        assert(m_fVoxelSizeMM == 0.0f); // set only once
        m_fVoxelSizeMM = fVoxelSizeMM;
    }
    
    inline float fVoxelSizeMM() const
    {
        if (m_fVoxelSizeMM == 0.0f)
        {
            assert(false); // VoxelSize needs to be set at beginning
            return 1.0f;
        }
        
        return m_fVoxelSizeMM;
    }
    
public:
    
    std::string strName() const
    {
#ifdef DEBUG_BUILD
        return "PicoGK Core Library (Debug Version)";
#else
        return "PicoGK Core Library";
#endif
    }
    
    std::string strLibraryName() const
    {
        return PICOGK_LIB_NAME;
    }
    
    std::string strVersion() const
    {
        return PICOGK_LIB_VERSION;
    }
    
    std::string strBuildInfo() const
    {
        return std::string(PICOGK_BUILD " " PICOGK_LIB_NAME);
    }
    
public: // Mesh Functions
    
    Mesh::Ptr* proMeshCreate()
    {
        
        Mesh::Ptr   roMesh  = std::make_shared<Mesh>();
        Mesh::Ptr*  proMesh = new Mesh::Ptr(roMesh);
        m_oMeshes[proMesh]  = proMesh;
        return proMesh;
    }
    
    Mesh::Ptr* proMeshCreateFromVoxels(const Voxels& oVoxels)
    {
        Mesh::Ptr   roMesh  = oVoxels.roAsMesh(fVoxelSizeMM());
        Mesh::Ptr*  proMesh = new Mesh::Ptr(roMesh);
        m_oMeshes[proMesh]  = proMesh;
        return proMesh;
    }
    
    bool bMeshFind(const Mesh::Ptr* proMesh) const
    {
        return (m_oMeshes.find(proMesh) != m_oMeshes.end());
    }
    
    bool bMeshIsValid(const Mesh::Ptr* proMesh)
    {
        if (proMesh == nullptr)
            return false;
        
        return bMeshFind(proMesh);
    }
        
    void MeshDestroy(Mesh::Ptr* proMesh)
    {
        auto it = m_oMeshes.find(proMesh);
        
        if (it != m_oMeshes.end())
        {
            m_oMeshes.erase(it);
            delete proMesh; // free reference to shared pointer
            return;
        }
        
        // not found, trying to free an element that doesn't exist
        assert(false);
    }
    
public: // Lattice functions
    
    Lattice::Ptr* proLatticeCreate()
    {
        
        Lattice::Ptr roLattice = std::make_shared<Lattice>();
        Lattice::Ptr* proLattice = new Lattice::Ptr(roLattice);
        m_oLattices[proLattice] = proLattice;
        return proLattice;
    }
    
    bool bLatticeFind(const Lattice::Ptr* proLattice) const
    {
        return (m_oLattices.find(proLattice) != m_oLattices.end());
    }
    
    bool bLatticeIsValid(const Lattice::Ptr* proLattice)
    {
        if (proLattice == nullptr)
            return false;
        
        return bLatticeFind(proLattice);
    }
        
    void LatticeDestroy(Lattice::Ptr* proLattice)
    {
        auto it = m_oLattices.find(proLattice);
        
        if (it != m_oLattices.end())
        {
            m_oLattices.erase(it);
            delete proLattice; // free reference to shared pointer
            return;
        }
        
        // not found, trying to free an element that doesn't exist
        assert(false);
    }
    
public: // PolyLine functions
    
    PolyLine::Ptr* proPolyLineCreate(const ColorFloat& clr)
    {
        PolyLine::Ptr   roPolyLine      = std::make_shared<PolyLine>(clr);
        PolyLine::Ptr*  proPolyLine     = new PolyLine::Ptr(roPolyLine);
        m_oPolyLines[proPolyLine]       = proPolyLine;
        return proPolyLine;
    }
    
    bool bPolyLineFind(const PolyLine::Ptr* proPolyLine) const
    {
        return (m_oPolyLines.find(proPolyLine) != m_oPolyLines.end());
    }
    
    bool bPolyLineIsValid(const PolyLine::Ptr* proPolyLine)
    {
        if (proPolyLine == nullptr)
            return false;
        
        return bPolyLineFind(proPolyLine);
    }
        
    void PolyLineDestroy(PolyLine::Ptr* proPolyLine)
    {
        auto it = m_oPolyLines.find(proPolyLine);
        
        if (it != m_oPolyLines.end())
        {
            m_oPolyLines.erase(it);
            delete proPolyLine; // free reference to shared pointer
            return;
        }
        
        // not found, trying to free an element that doesn't exist
        assert(false);
    }
    
public: // Voxels functions
    
    Voxels::Ptr* proVoxelsCreate()
    {
        Voxels::Ptr   roVoxels = std::make_shared<Voxels>();
        Voxels::Ptr* proVoxels = new Voxels::Ptr(roVoxels);
        m_oVoxels[proVoxels]   = proVoxels;
        return proVoxels;
    }
    
    Voxels::Ptr* proVoxelsCreateCopy(const Voxels& oSource)
    {
        Voxels::Ptr   roNew = std::make_shared<Voxels>(oSource);
        Voxels::Ptr* proNew = new Voxels::Ptr(roNew);
        m_oVoxels[proNew]   = proNew;
        return proNew;
    }
    
    bool bVoxelsFind(const Voxels::Ptr* proVoxels) const
    {
        return (m_oVoxels.find(proVoxels) != m_oVoxels.end());
    }
    
    bool bVoxelsIsValid(const Voxels::Ptr* proVoxels)
    {
        if (proVoxels == nullptr)
            return false;
        
        return bVoxelsFind(proVoxels);
    }
        
    void VoxelsDestroy(Voxels::Ptr* proVoxels)
    {
        auto it = m_oVoxels.find(proVoxels);
        
        if (it != m_oVoxels.end())
        {
            m_oVoxels.erase(it);
            delete proVoxels; // free reference to shared pointer
            return;
        }
        
        // not found, trying to free an element that doesn't exist
        assert(false);
    }
    
public:

    Library(const Library&)                 = delete;
    Library& operator = (const Library&)    = delete;
    
private:
    Library()
    {
        
    }
    
    ~Library()
    {
     
    }
    
protected:
    float                           m_fVoxelSizeMM  = 0.0f;
    
    std::map<const Mesh::Ptr*,      Mesh::Ptr*>     m_oMeshes;
    std::map<const Lattice::Ptr*,   Lattice::Ptr*>  m_oLattices;
    std::map<const PolyLine::Ptr*,  PolyLine::Ptr*> m_oPolyLines;
    std::map<const Voxels::Ptr*,    Voxels::Ptr*>   m_oVoxels;

};

} // namespace PicoGK

#endif // PICOGKLIBRARYMANAGER_H_
