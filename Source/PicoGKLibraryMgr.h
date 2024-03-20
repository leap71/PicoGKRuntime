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

#ifndef PICOGKLIBRARYMGR_H_
#define PICOGKLIBRARYMGR_H_

#include "PicoGKBuild.h"
#include <string>
#include <map>

#include "PicoGKMesh.h"
#include "PicoGKLattice.h"
#include "PicoGKPolyLine.h"
#include "PicoGKVdbVoxels.h"
#include "PicoGKVdbFile.h"
#include "PicoGKVdbField.h"
#include "PicoGKVdbMeta.h"

#define PK_IMPLEMENT_STANDARD_LIB_FUNCTIONS(ClassName)                  \
                                                                        \
ClassName::Ptr* pro##ClassName##Create()                                \
{                                                                       \
    ClassName::Ptr  ro          =  std::make_shared<ClassName>();       \
    ClassName::Ptr* pro         = new ClassName::Ptr(ro);               \
    m_o##ClassName##List[pro]  = pro;                                   \
    return pro;                                                         \
}                                                                       \
                                                                        \
ClassName::Ptr* pro##ClassName##CreateCopy(const ClassName& oSource)    \
{                                                                       \
    ClassName::Ptr  ro          =  std::make_shared<ClassName>(oSource);\
    ClassName::Ptr* pro         = new ClassName::Ptr(ro);               \
    m_o##ClassName##List[pro]  = pro;                                   \
    return pro;                                                         \
}                                                                       \
                                                                        \
bool b##ClassName##Find(const ClassName::Ptr* pro) const                \
{                                                                       \
    return (m_o##ClassName##List.find(pro)                              \
        != m_o##ClassName##List.end());                                 \
}                                                                       \
                                                                        \
bool b##ClassName##IsValid(const ClassName::Ptr* pro)                   \
{                                                                       \
    if (pro == nullptr)                                                 \
        return false;                                                   \
                                                                        \
    return b##ClassName##Find(pro);                                     \
}                                                                       \
                                                                        \
void ClassName##Destroy(ClassName::Ptr* pro)                            \
{                                                                       \
    auto it = m_o##ClassName##List.find(pro);                           \
                                                                        \
    if (it != m_o##ClassName##List.end())                               \
    {                                                                   \
        m_o##ClassName##List.erase(it);                                 \
        delete pro;                                                     \
        return;                                                         \
    }                                                                   \
                                                                        \
                                                                        \
    assert(false);                                                      \
}                                                                       

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
    PK_IMPLEMENT_STANDARD_LIB_FUNCTIONS(Mesh)
    
    Mesh::Ptr* proMeshCreateFromVoxels(const Voxels& oVoxels)
    {
        Mesh::Ptr   roMesh      = oVoxels.roAsMesh(fVoxelSizeMM());
        Mesh::Ptr*  proMesh     = new Mesh::Ptr(roMesh);
        m_oMeshList[proMesh]    = proMesh;
        return proMesh;
    }
    
public: // Lattice functions
    PK_IMPLEMENT_STANDARD_LIB_FUNCTIONS(Lattice)
    
public: // PolyLine functions
    
    PolyLine::Ptr* proPolyLineCreate(const ColorFloat& clr)
    {
        PolyLine::Ptr   roPolyLine      = std::make_shared<PolyLine>(clr);
        PolyLine::Ptr*  proPolyLine     = new PolyLine::Ptr(roPolyLine);
        m_oPolyLineList[proPolyLine]    = proPolyLine;
        return proPolyLine;
    }
    
    bool bPolyLineFind(const PolyLine::Ptr* proPolyLine) const
    {
        return (m_oPolyLineList.find(proPolyLine)
                != m_oPolyLineList.end());
    }
    
    bool bPolyLineIsValid(const PolyLine::Ptr* proPolyLine)
    {
        if (proPolyLine == nullptr)
            return false;
        
        return bPolyLineFind(proPolyLine);
    }
        
    void PolyLineDestroy(PolyLine::Ptr* proPolyLine)
    {
        auto it = m_oPolyLineList.find(proPolyLine);
        
        if (it != m_oPolyLineList.end())
        {
            m_oPolyLineList.erase(it);
            delete proPolyLine; // free reference to shared pointer
            return;
        }
        
        // not found, trying to free an element that doesn't exist
        assert(false);
    }
    
public: // Voxels functions
    PK_IMPLEMENT_STANDARD_LIB_FUNCTIONS(Voxels)
    
public: // VdbFile functions
    PK_IMPLEMENT_STANDARD_LIB_FUNCTIONS(VdbFile)
    
    VdbFile::Ptr* proVdbFileCreateFromFile(std::string strFileName)
    {
        VdbFile::Ptr roVdbFile = VdbFile::roFromFile(strFileName);
        if (roVdbFile == nullptr)
            return nullptr;
        
        VdbFile::Ptr*   proVdbFile  = new VdbFile::Ptr(roVdbFile);
        m_oVdbFileList[proVdbFile]  = proVdbFile;
        return proVdbFile;
    }
    
    bool bVdbSaveToFile(std::string strFileName)
    {
        VdbFile::Ptr roVdbFile = VdbFile::roFromFile(strFileName);
        if (roVdbFile == nullptr)
            return false;
        
        return roVdbFile->bSaveToFile(strFileName);
    }
    
    Voxels::Ptr* proVdbFileGetVoxels(   VdbFile::Ptr roVdbFile,
                                        int32_t nIndex)
    {
        GridBase::Ptr roGrid = roVdbFile->roGridAt(nIndex);
        
        if (!roGrid->isType<FloatGrid>())
            return nullptr;
        
        if (roGrid->getGridClass() != GRID_LEVEL_SET)
            return nullptr; // not a voxel field
        
        Voxels::Ptr roVoxels = std::make_shared<Voxels>(gridPtrCast<FloatGrid>(roGrid));
        
        Voxels::Ptr* proVoxels      = new Voxels::Ptr(roVoxels);
        m_oVoxelsList[proVoxels]    = proVoxels;
        return proVoxels;

    }
    
    int32_t nVdbFileAddVoxels(  VdbFile::Ptr roVdbFile,
                                std::string strName,
                                Voxels::Ptr roVoxels)
    {
        return roVdbFile->nAddGrid(strName, roVoxels->roVdbGrid());
    }
    
    int32_t nVdbFileAddScalarField( VdbFile::Ptr roVdbFile,
                                    std::string strName,
                                    ScalarField::Ptr roField)
    {
        return roVdbFile->nAddGrid(strName, roField->roVdbGrid());
    }
    
    ScalarField::Ptr* proVdbFileGetScalarField( VdbFile::Ptr roVdbFile,
                                                int32_t nIndex)
    {
        GridBase::Ptr roGrid = roVdbFile->roGridAt(nIndex);
        
        if (!roGrid->isType<FloatGrid>())
            return nullptr;
        
        // We treat all float grids as scalar fields, if loaded through this function
        // PicoGK stores scalar fields as fog volumes
        
        ScalarField::Ptr roField = std::make_shared<ScalarField>(gridPtrCast<FloatGrid>(roGrid));
        
        ScalarField::Ptr* proField      = new ScalarField::Ptr(roField);
        m_oScalarFieldList[proField]    = proField;
        return proField;
    }
    
    int32_t nVdbFileAddVectorField( VdbFile::Ptr roVdbFile,
                                    std::string strName,
                                    VectorField::Ptr roField)
    {
        return roVdbFile->nAddGrid(strName, roField->roVdbGrid());
    }
    
    VectorField::Ptr* proVdbFileGetVectorField( VdbFile::Ptr roVdbFile,
                                                int32_t nIndex)
    {
        GridBase::Ptr roGrid = roVdbFile->roGridAt(nIndex);
        
        if (!roGrid->isType<Vec3SGrid>())
            return nullptr;
        
        VectorField::Ptr roField        = std::make_shared<VectorField>(gridPtrCast<Vec3SGrid>(roGrid));
        
        VectorField::Ptr* proField      = new VectorField::Ptr(roField);
        m_oVectorFieldList[proField]    = proField;
        return proField;
    }
    
public: // ScalarField functions
    PK_IMPLEMENT_STANDARD_LIB_FUNCTIONS(ScalarField)
    
    ScalarField::Ptr* proScalarFieldCreateFromVoxels(const Voxels& oVoxels)
    {
        ScalarField::Ptr roField = std::make_shared<ScalarField>(oVoxels);
        
        ScalarField::Ptr* proField      = new ScalarField::Ptr(roField);
        m_oScalarFieldList[proField]    = proField;
        return proField;
    }
    
public: // VectorField
    PK_IMPLEMENT_STANDARD_LIB_FUNCTIONS(VectorField)
    
public:
    VdbMeta::Ptr* proVdbMetaFromField(MetaMap::Ptr roMetaMap)
    {
        VdbMeta::Ptr roField = std::make_shared<VdbMeta>(roMetaMap);
        
        VdbMeta::Ptr* proField      = new VdbMeta::Ptr(roField);
        m_oVdbMetaList[proField]    = proField;
        return proField;
    }
    
    bool bVdbMetaFind(const VdbMeta::Ptr* pro) const
    {
        return (m_oVdbMetaList.find(pro)
            != m_oVdbMetaList.end());
    }
    
    bool bVdbMetaIsValid(const VdbMeta::Ptr* pro)
    {
        if (pro == nullptr)
            return false;

        return bVdbMetaFind(pro);
    }

    void VdbMetaDestroy(VdbMeta::Ptr* pro)
    {
        auto it = m_oVdbMetaList.find(pro);
        
        if (it != m_oVdbMetaList.end())
        {
            m_oVdbMetaList.erase(it);
            delete pro;
            return;
        }

        assert(false);
    }
    
public:

    Library(const Library&)                 = delete;
    Library& operator = (const Library&)    = delete;
    
private:
    Library()
    {
        openvdb::initialize();
    }
    
    ~Library()
    {
     
    }
    
protected:
    float                               m_fVoxelSizeMM  = 0.0f;
    
    std::map<const Mesh::Ptr*,          Mesh::Ptr*>         m_oMeshList;
    std::map<const Lattice::Ptr*,       Lattice::Ptr*>      m_oLatticeList;
    std::map<const PolyLine::Ptr*,      PolyLine::Ptr*>     m_oPolyLineList;
    std::map<const Voxels::Ptr*,        Voxels::Ptr*>       m_oVoxelsList;
    std::map<const VdbFile::Ptr*,       VdbFile::Ptr*>      m_oVdbFileList;
    std::map<const ScalarField::Ptr*,   ScalarField::Ptr*>  m_oScalarFieldList;
    std::map<const VectorField::Ptr*,   VectorField::Ptr*>  m_oVectorFieldList;
    std::map<const VdbMeta::Ptr*,       VdbMeta::Ptr*>      m_oVdbMetaList;
};

} // namespace PicoGK

#endif // PICOGKLIBRARYMANAGER_H_
