/* PythonXM
 * Copyright (c) 2018-2019 tevador     <tevador@gmail.com>
 * Copyright (c) 2018-2021 SChernykh   <https://github.com/SChernykh>
 * Copyright (c) 2016-2021 PythonXM       <https://github.com/pythonxm>, <support@pythonxm.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PYTHONXM_RXMSR_H
#define PYTHONXM_RXMSR_H


#include <vector>


namespace pythonxm
{


class CpuThread;
class RxConfig;


class RxMsr
{
public:
    static inline bool isEnabled()      { return m_enabled; }
    static inline bool isInitialized()  { return m_initialized; }

    static bool init(const RxConfig &config, const std::vector<CpuThread> &threads);
    static void destroy();

private:
    static bool m_cacheQoS;
    static bool m_enabled;
    static bool m_initialized;
};


} /* namespace pythonxm */


#endif /* PYTHONXM_RXMSR_H */
