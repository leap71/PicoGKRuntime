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

#ifndef PICOGKVDBFILE_H_
#define PICOGKVDBFILE_H_

#include <openvdb/openvdb.h>
#include <openvdb/io/Stream.h>

namespace PicoGK
{
class VdbFile
{
public:
    PKSHAREDPTR(VdbFile);
    
    static VdbFile::Ptr roFromFile(std::string strFileName)
    {
        openvdb::io::File oFile(strFileName);
        try
        {
            oFile.open();
            openvdb::GridPtrVecPtr roGrids = oFile.getGrids();
            oFile.close();
            return make_shared<VdbFile>(roGrids);
        }
        
        catch (const openvdb::IoError& e)
        {
            // std::cerr << "OpenVDB I/O Error: " << e.what() << std::endl;
        }
        
        catch (...)
        {
            
        }
        
        return nullptr;
    }
    
    VdbFile()
    {
        // Empty Grids vector
        m_roGrids = std::make_shared<openvdb::GridPtrVec>();
    }
    
    VdbFile(GridPtrVecPtr roGrids)
    {
        m_roGrids = roGrids;
    }
    
    int32_t nAddGrid(   std::string                     strGridName,
                        const openvdb::FloatGrid::Ptr   roGrid)
    {
        openvdb::FloatGrid::Ptr roCopy = deepCopyTypedGrid<FloatGrid>(roGrid);
        roCopy->setName(strGridName);
        m_roGrids->push_back(roCopy);
        return (int32_t) m_roGrids->size()-1;
    }
    
    int32_t nGridCount() const
    {
        return (int32_t) m_roGrids->size();
    }
    
    GridBase::Ptr roGridAt(int32_t nIndex) const
    {
        assert(nIndex < nGridCount());
        return m_roGrids->at(nIndex);
    }
    
    std::string strNameAt(int32_t nIndex) const
    {
        auto roGrid = roGridAt(nIndex);
        return roGrid->getName();
    }
    
    int nTypeAt(int32_t nIndex) const
    {
        auto roGrid = roGridAt(nIndex);
        
        if (roGrid->isType<openvdb::FloatGrid>())
        {
            switch (roGrid->getGridClass())
            {
                case openvdb::GRID_LEVEL_SET:
                    return 0;
                    break;
                    
                default:
                    break;
            }
        }
        
        return -1;
    }
    
    bool bSaveToFile(std::string strFileName)
    {
        std::vector<openvdb::GridBase::Ptr> oGrids;
        for (const auto& roGrid : *m_roGrids)
        {
            oGrids.push_back(roGrid);
        }
        
        try
        {
            openvdb::io::File oFile(strFileName);
            oFile.write(oGrids);
            oFile.close();
            return true;
        }
        
        catch (const openvdb::IoError& e)
        {
            // std::cerr << "OpenVDB I/O Error: " << e.what() << std::endl;
        }
        
        catch (...)
        {
        }
        
        return false;
    }

protected:
    
    openvdb::GridPtrVecPtr m_roGrids;
};
}
#endif
