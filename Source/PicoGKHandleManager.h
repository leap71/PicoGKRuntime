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

#ifndef PicoGKHandleManager_h
#define PicoGKHandleManager_h
#include <shared_mutex>
#include <unordered_map>
#include <memory>
#include <cstdint>

namespace PicoGK
{
/// A generic handle‐manager for T, using integer handles.
/// Thread-safe: all operations lock internally.
template
<
    typename T,
    typename Handle = uint64_t
>
class HandleManager 
{
public:
    HandleManager(std::string strName)
    {
        m_strName = strName;
    }
    /// Add a new object, returns a unique handle.
    inline Handle hAdd(std::shared_ptr<T> obj) 
    {
        std::unique_lock lk(m_mtx);

        m_hCurrent++;
        m_map.emplace(m_hCurrent, std::move(obj));
        return m_hCurrent;
    }

    /// Retrieve the object for a handle. 
    /// Throws exception if invalid.
    inline std::shared_ptr<T> roGet(Handle h) const
    {
        std::shared_lock lk(m_mtx);

        auto it = m_map.find(h);

        if (it == m_map.end())
        {
            throw std::out_of_range(m_strName + ": Invalid Handle: " + std::to_string(h));
        }

        return it->second;
    }

    /// Remove (and destroy) the object for a handle. 
    /// Returns true if existed.
    inline bool bDestroy(Handle h) 
    {
        std::unique_lock lk(m_mtx);
        return m_map.erase(h) > 0;
    }

    /// Check if a handle is valid (exists)
    inline bool bIsValid(Handle h) const 
    {
        std::unique_lock lk(m_mtx);
        return m_map.count(h) != 0;
    }

    /// Returns the number of currently allocated objects 
    inline size_t nAllocatedCount() const
    {
        std::shared_lock lk(m_mtx);
        return m_map.size();
    }
    
    inline int64_t nMemUsage() const
    {
        std::shared_lock lk(m_mtx);
        
        int64_t nBytes = 0;
        for (const auto& pair : m_map)
            nBytes += pair.second->nMemUsage();
        
        return nBytes;
    }

protected:
    std::string                                     m_strName;
    mutable std::shared_mutex                       m_mtx;
    Handle                                          m_hCurrent = 0;
    std::unordered_map<Handle, std::shared_ptr<T>>  m_map;
};

} // namespace PicoGL

#endif // PicoGKHandleManager_h
