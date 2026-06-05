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

#ifndef PICOGKTRACE_H_
#define PICOGKTRACE_H_

//#define PICOGKTRACE_ENABLE true

#include <iostream>
#include <chrono>

#if defined(PICOGKTRACE_ENABLE)
#   define PKTRACE(func) PkTrace trace_##func(#func, PICOGKTRACE_ENABLE)
#else
#   define PKTRACE(func)
#endif

namespace PicoGK
{

class PkTrace
{
public:
    PkTrace(std::string strName, bool bCostlyOnly)
    {
        m_timStart      = std::chrono::high_resolution_clock::now();
        m_strName       = strName;
        m_bCostlyOnly   = bCostlyOnly;
        
        if (!m_bCostlyOnly)
            std::cerr << m_strName << " - Entered\n";
    }
    
    ~PkTrace()
    {
        std::chrono::milliseconds oMS = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_timStart);
        
        if ((!m_bCostlyOnly) || (oMS.count() > 100))
        {
            std::cerr << m_strName << " - Exited ";
            std::cerr << oMS << " elapsed\n";
        }
    }
    
protected:
    std::string m_strName;
    bool        m_bCostlyOnly;
    
    std::chrono::time_point<std::chrono::high_resolution_clock>
                                        m_timStart;
};
    
} // namespace PicoGK

#endif

