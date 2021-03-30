/* PythonXM
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

#ifndef PYTHONXM_PLATFORM_H
#define PYTHONXM_PLATFORM_H


#include <cstdint>


#include "base/tools/String.h"


namespace pythonxm {


class Platform
{
public:
    static inline bool trySetThreadAffinity(int64_t cpu_id)
    {
        if (cpu_id < 0) {
            return false;
        }

        return setThreadAffinity(static_cast<uint64_t>(cpu_id));
    }

    static bool setThreadAffinity(uint64_t cpu_id);
    static void init(const char *userAgent);
    static void setProcessPriority(int priority);
    static void setThreadPriority(int priority);

    static inline bool isUserActive(uint64_t ms)    { return idleTime() < ms; }
    static inline const String &userAgent()         { return m_userAgent; }

    static bool isOnBatteryPower();
    static uint64_t idleTime();

private:
    static char *createUserAgent();

    static String m_userAgent;
};


} // namespace pythonxm


#endif /* PYTHONXM_PLATFORM_H */
