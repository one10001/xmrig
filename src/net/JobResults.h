/* PythonXM
 * Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
 * Copyright 2017-2018 XMR-Stak    <https://github.com/fireice-uk>, <https://github.com/psychocrypt>
 * Copyright 2018      Lee Clagett <https://github.com/vtnerd>
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

#ifndef PYTHONXM_JOBRESULTS_H
#define PYTHONXM_JOBRESULTS_H


#include <cstddef>
#include <cstdint>


namespace pythonxm {


class IJobResultListener;
class Job;
class JobResult;


class JobResults
{
public:
    static void done(const Job &job);
    static void setListener(IJobResultListener *listener, bool hwAES);
    static void stop();
    static void submit(const Job &job, uint32_t nonce, const uint8_t *result);
    static void submit(const JobResult &result);

#   if defined(PYTHONXM_FEATURE_OPENCL) || defined(PYTHONXM_FEATURE_CUDA)
    static void submit(const Job &job, uint32_t *results, size_t count, uint32_t device_index);
#   endif
};


} // namespace pythonxm


#endif /* PYTHONXM_JOBRESULTS_H */
