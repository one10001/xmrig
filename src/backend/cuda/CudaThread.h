/* PythonXM
 * Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
 * Copyright 2017-2018 XMR-Stak    <https://github.com/fireice-uk>, <https://github.com/psychocrypt>
 * Copyright 2018-2020 SChernykh   <https://github.com/SChernykh>
 * Copyright 2016-2020 PythonXM       <https://github.com/pythonxm>, <support@pythonxm.com>
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

#ifndef PYTHONXM_CUDATHREAD_H
#define PYTHONXM_CUDATHREAD_H


using nvid_ctx = struct nvid_ctx;


#include "3rdparty/rapidjson/fwd.h"


namespace pythonxm {


class CudaThread
{
public:
    CudaThread() = delete;
    CudaThread(const rapidjson::Value &value);
    CudaThread(uint32_t index, nvid_ctx *ctx);

    inline bool isValid() const                              { return m_blocks > 0 && m_threads > 0; }
    inline int32_t bfactor() const                           { return static_cast<int32_t>(m_bfactor); }
    inline int32_t blocks() const                            { return m_blocks; }
    inline int32_t bsleep() const                            { return static_cast<int32_t>(m_bsleep); }
    inline int32_t datasetHost() const                       { return m_datasetHost; }
    inline int32_t threads() const                           { return m_threads; }
    inline int64_t affinity() const                          { return m_affinity; }
    inline uint32_t index() const                            { return m_index; }

    inline bool operator!=(const CudaThread &other) const    { return !isEqual(other); }
    inline bool operator==(const CudaThread &other) const    { return isEqual(other); }

    bool isEqual(const CudaThread &other) const;
    rapidjson::Value toJSON(rapidjson::Document &doc) const;

private:
    int32_t m_blocks        = 0;
    int32_t m_datasetHost   = -1;
    int32_t m_threads       = 0;
    int64_t m_affinity      = -1;
    uint32_t m_index        = 0;

#   ifdef _WIN32
    uint32_t m_bfactor      = 6;
    uint32_t m_bsleep       = 25;
#   else
    uint32_t m_bfactor      = 0;
    uint32_t m_bsleep       = 0;
#   endif
};


} /* namespace pythonxm */


#endif /* PYTHONXM_CUDATHREAD_H */
