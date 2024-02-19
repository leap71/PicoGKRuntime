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

#ifndef PICOGKVDBFIELD_H_
#define PICOGKVDBFIELD_H_

#include <openvdb/openvdb.h>
#include "PicoGKTypes.h"

using namespace openvdb;

namespace PicoGK
{

template<class TFieldType>
class Field
{
public:
    
    Field()
    {
        m_roGrid = TFieldType::create();
    };
    
    Field(TFieldType::Ptr roGrid)
    {
        m_roGrid = roGrid;
    };
    
    Field(const Field& oSource)
    {
        m_roGrid = deepCopyTypedGrid<TFieldType>(oSource.m_roGrid);
    };
    
    TFieldType::Ptr roVdbGrid() const {return m_roGrid;}
    
protected:
    TFieldType::Ptr             m_roGrid;
};

class ScalarField : public Field<FloatGrid>
{
public:
    typedef std::shared_ptr<ScalarField> Ptr;
    
    ScalarField()
    : Field()
    {
        m_roGrid->setGridClass(GRID_FOG_VOLUME);
    }
    
    ScalarField(const ScalarField& roSource)
    : Field(roSource)
    {
        m_roGrid->setGridClass(GRID_FOG_VOLUME);
    }
    
    ScalarField(const Voxels& roSource)
    : ScalarField(roSource.roVdbGrid())
    {
    }
    
    ScalarField(const FloatGrid::Ptr roSource)
    : Field(roSource)
    {
        m_roGrid->setGridClass(GRID_FOG_VOLUME);
    }
    
    void SetValue(  Vector3     vecPos,
                    VoxelSize   oVoxelSize,
                    float       fValue)
    {
        auto oAccess = m_roGrid->getAccessor();
        
        openvdb::Coord xyz(     oVoxelSize.iToVoxels(vecPos.X),
                                oVoxelSize.iToVoxels(vecPos.Y),
                                oVoxelSize.iToVoxels(vecPos.Z));
        
        oAccess.setValue(xyz, fValue);
    }
    
    bool bGetValue( Vector3 vecPos,
                    VoxelSize oVoxelSize,
                    float* pfValue)
    {
        auto oAccess = m_roGrid->getConstAccessor();
    
        openvdb::Coord xyz( oVoxelSize.iToVoxels(vecPos.X),
                            oVoxelSize.iToVoxels(vecPos.Y),
                            oVoxelSize.iToVoxels(vecPos.Z));
        
        if (!oAccess.isValueOn(xyz))
            return false;
        
        *pfValue = oAccess.getValue(xyz);
        return true;
    }
};

class VectorField : public Field<Vec3SGrid>
{
public:
    typedef std::shared_ptr<VectorField> Ptr;
    
    VectorField()
    : Field()
    {
    }
    
    VectorField(Vec3SGrid::Ptr roField)
    : Field(roField)
    {
    }
    
    void AddGradientFieldFrom(Voxels::Ptr roVoxels)
    {
        auto oField     = m_roGrid->getAccessor();
        auto oVoxels    = roVoxels->roVdbGrid()->getConstAccessor();
        CoordBBox oBBox = roVoxels->roVdbGrid()->evalActiveVoxelBoundingBox();
        
        math::GradStencil oStencil(*roVoxels->roVdbGrid());
        
        for (auto x=oBBox.min().x(); x<=oBBox.max().x(); x++)
        for (auto y=oBBox.min().y(); y<=oBBox.max().y(); y++)
        for (auto z=oBBox.min().z(); z<=oBBox.max().z(); z++)
        {
            openvdb::Coord xyz(x,y,z);
            
            if (oVoxels.isValueOn(xyz))
            {
                oStencil.moveTo(xyz);
                auto vecGradient = oStencil.gradient();
                oField.setValue(xyz, vecGradient);
            }
        }
    }
    
    void SetValue(  Vector3 vecPos,
                    VoxelSize oVoxelSize,
                    Vector3 vecValue)
    {
        auto oAccess = m_roGrid->getAccessor();
        
        Vec3s vec(vecValue.X, vecValue.Y, vecValue.Z);
        openvdb::Coord xyz( oVoxelSize.iToVoxels(vecPos.X),
                            oVoxelSize.iToVoxels(vecPos.Y),
                            oVoxelSize.iToVoxels(vecPos.Z));
        
        oAccess.setValue(xyz, vec);
    }
    
    bool bGetValue( Vector3 vecPos,
                    VoxelSize oVoxelSize,
                    Vector3* pvecValue)
    {
        auto oAccess = m_roGrid->getConstAccessor();
    
        openvdb::Coord xyz( oVoxelSize.iToVoxels(vecPos.X),
                            oVoxelSize.iToVoxels(vecPos.Y),
                            oVoxelSize.iToVoxels(vecPos.Z));
        
        if (!oAccess.isValueOn(xyz))
            return false;
        
        Vec3s vec = oAccess.getValue(xyz);
        
        pvecValue->X = vec.x();
        pvecValue->Y = vec.y();
        pvecValue->Z = vec.z();
        return true;
    }
};

} // PicoGK namespace

#endif // PICOGKVDBFIELD_H_
