/* PythonXM
 * Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
 * Copyright 2017-2018 XMR-Stak    <https://github.com/fireice-uk>, <https://github.com/psychocrypt>
 * Copyright 2018-2019 SChernykh   <https://github.com/SChernykh>
 * Copyright 2016-2019 PythonXM       <https://github.com/pythonxm>, <support@pythonxm.com>
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

#ifndef PYTHONXM_NVMLLIB_H
#define PYTHONXM_NVMLLIB_H


#include "backend/cuda/wrappers/CudaDevice.h"
#include "backend/cuda/wrappers/NvmlHealth.h"


namespace pythonxm {


class NvmlLib
{
public:
    static bool init(const char *fileName = nullptr);
    static const char *lastError() noexcept;
    static void close();

    static bool assign(std::vector<CudaDevice> &devices);
    static NvmlHealth health(nvmlDevice_t device);

    static inline bool isInitialized() noexcept         { return m_initialized; }
    static inline bool isReady() noexcept               { return m_ready; }
    static inline const char *driverVersion() noexcept  { return m_driverVersion; }
    static inline const char *version() noexcept        { return m_nvmlVersion; }

private:
    static bool dlopen();
    static bool load();

    static bool m_initialized;
    static bool m_ready;
    static char m_driverVersion[80];
    static char m_nvmlVersion[80];
    static String m_loader;
};


} // namespace pythonxm


#endif /* PYTHONXM_NVMLLIB_H */
