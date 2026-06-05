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

#ifndef PICOGKLIBRARYMGR_H_
#define PICOGKLIBRARYMGR_H_

#include "PicoGKBuild.h"
#include <string>

#include "PicoGKMesh.h"
#include "PicoGKLattice.h"
#include "PicoGKPolyLine.h"
#include "PicoGKVdbVoxels.h"
#include "PicoGKVdbFile.h"
#include "PicoGKVdbField.h"
#include "PicoGKVdbMeta.h"
#include "PicoGKHandleManager.h"
#include "PicoGKTrace.h"

namespace PicoGK
{
    class Library
    {
    public:
        class Instance
        {
        public:
            PKSHAREDPTR(Instance);
            
            Instance(float fVoxelSizeMM)
            {
                m_fVoxelSizeMM = fVoxelSizeMM;
            }
            
            inline float fVoxelSizeMM() const
            {
                return m_fVoxelSizeMM;
            }
            
            inline int64_t nMemUsage() const
            {
                int64_t nResult = 0;
                nResult += m_oMeshes.nMemUsage();
                nResult += m_oLattices.nMemUsage();
                nResult += m_oPolyLines.nMemUsage();
                nResult += m_oVoxels.nMemUsage();
                nResult += m_oVdbFiles.nMemUsage();
                nResult += m_oScalarFields.nMemUsage();
                nResult += m_oVectorFields.nMemUsage();
                nResult += m_oVdbMetas.nMemUsage();
                return nResult;
            }
            
            HandleManager<PicoGK::Mesh>         m_oMeshes       {"Meshes"};
            HandleManager<PicoGK::Lattice>      m_oLattices     {"Lattices"};
            HandleManager<PicoGK::PolyLine>     m_oPolyLines    {"PolyLines"};
            HandleManager<PicoGK::Voxels>       m_oVoxels       {"Voxels"};
            HandleManager<PicoGK::VdbFile>      m_oVdbFiles     {"VdbFiles"};
            HandleManager<PicoGK::ScalarField>  m_oScalarFields {"ScalarFields"};
            HandleManager<PicoGK::VectorField>  m_oVectorFields {"VectorFields"};
            HandleManager<PicoGK::VdbMeta>      m_oVdbMetas     {"VdbMetas"};
            
        protected:
            float m_fVoxelSizeMM  = 0.0f;
        }; // Instance
        
    public:
        inline static Library& oLib()
        {
            static Library  m_oLib;
            return m_oLib;
        }
        
        uint64_t nCreateInstance(float fVoxelSizeMM)
        {
            return m_oInstances.hAdd(std::make_shared<Instance>(fVoxelSizeMM));
        }
        
        bool bIsValid(uint64_t hInstance)
        {
            return m_oInstances.bIsValid(hInstance);
        }
        
        void DestroyInstance(uint64_t hInstance)
        {
            m_oInstances.bDestroy(hInstance);
        }
        
        std::shared_ptr<Instance> roGetInstance(uint64_t hInstance)
        {
            //PKTRACE(roGetInstance);
            return m_oInstances.roGet(hInstance);
        }
        
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
        
    protected:
        HandleManager<Instance>  m_oInstances   {"LibraryInstance"};
        
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

    };
} // namespace PicoGK

#endif // PICOGKLIBRARYMANAGER_H_
