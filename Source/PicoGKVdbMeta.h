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

#ifndef PICOGKVDBMETA_H_
#define PICOGKVDBMETA_H_

#include <openvdb/openvdb.h>
#include "PicoGKVdbVoxels.h"

namespace PicoGK
{

class VdbMeta
{
public:
    PKSHAREDPTR(VdbMeta);
    
    enum EType
    {
        METATYPE_UNKNOWN = -1,
        METATYPE_STRING  = 0,
        METATYPE_FLOAT,
        METATYPE_VECTOR
    };
    
    VdbMeta(MetaMap::Ptr roMetaMap)
    {
        m_roMetaMap = roMetaMap;
    }
    
    int32_t nCount() const
    {
        return (int32_t) m_roMetaMap->metaCount();
    }
    
    std::string strNameAt(int32_t nIndex)
    {
        int32_t n=0;
        
        for (   auto iter = m_roMetaMap->beginMeta();
                iter != m_roMetaMap->endMeta();
                ++iter)
        {
            if (n==nIndex)
            return iter->first;
                
            n++;
        }
        
        return "";
    }
    
    EType eTypeAt(const std::string& strValueName)
    {
        openvdb::Metadata::Ptr roMeta = (*m_roMetaMap)[strValueName];
        
        if (roMeta == nullptr)
            return METATYPE_UNKNOWN;
        else if (roMeta->typeName() == openvdb::StringMetadata::staticTypeName())
            return METATYPE_STRING;
        else if (roMeta->typeName() == openvdb::FloatMetadata::staticTypeName())
            return METATYPE_FLOAT;
        else if (roMeta->typeName() == openvdb::Vec3SMetadata::staticTypeName())
            return METATYPE_VECTOR;
        
        return METATYPE_UNKNOWN;
    }
    
    bool bGetValueAt(   const std::string& strValueName,
                        std::string* pstrValue)
    {
        openvdb::Metadata::Ptr roMeta = (*m_roMetaMap)[strValueName];
        
        if (roMeta == nullptr)
            return false; // doesn't exist
        
        if (roMeta->typeName() != openvdb::StringMetadata::staticTypeName())
            return false; // wrong type
        
        *pstrValue = static_cast<openvdb::StringMetadata&>(*roMeta).value();
        return true;
    }
    
    bool bGetValueAt(  const std::string& strValueName,
                       float* pfValue)
    {
        openvdb::Metadata::Ptr roMeta = (*m_roMetaMap)[strValueName];
        
        if (roMeta == nullptr)
            return false; // doesn't exist
        
        if (roMeta->typeName() != openvdb::FloatMetadata::staticTypeName())
            return false; // wrong type
        
        *pfValue = static_cast<openvdb::FloatMetadata&>(*roMeta).value();
        return true;
    }
    
    bool bGetValueAt(   const std::string& strValueName,
                        Vector3* pvecValue)
    {
        openvdb::Metadata::Ptr roMeta = (*m_roMetaMap)[strValueName];
        
        if (roMeta == nullptr)
            return false; // doesn't exist
        
        if (roMeta->typeName() != openvdb::Vec3SMetadata::staticTypeName())
            return false; // wrong type
        
        Vec3s vec = static_cast<openvdb::Vec3SMetadata&>(*roMeta).value();
        
        pvecValue->X = vec.x();
        pvecValue->Y = vec.y();
        pvecValue->Z = vec.z();
        return true;
    }
    
    void RemoveAt(const std::string& strValueName)
    {
        m_roMetaMap->removeMeta(strValueName);
    }
    
    void SetValue(  const std::string& strValueName,
                    const std::string& strValue)
    {
        
        m_roMetaMap->insertMeta(    strValueName,
                                    openvdb::StringMetadata(strValue));
    }
    
    void SetValue(  const std::string&  strValueName,
                    float               fValue)
    {
        
        m_roMetaMap->insertMeta(    strValueName,
                                    openvdb::FloatMetadata(fValue));
    }
    
    void SetValue(  const std::string&  strValueName,
                    Vector3             vecValue)
    {
        m_roMetaMap->insertMeta(    strValueName,
                                    openvdb::Vec3SMetadata(
                                        openvdb::Vec3s( vecValue.X,
                                                        vecValue.Y,
                                                        vecValue.Z)));
    }
    
    MetaMap::Ptr m_roMetaMap;
};

}

#endif
