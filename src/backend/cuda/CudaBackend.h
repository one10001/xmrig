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

#ifndef PYTHONXM_CUDABACKEND_H
#define PYTHONXM_CUDABACKEND_H


#include <utility>


#include "backend/common/interfaces/IBackend.h"
#include "base/tools/Object.h"


namespace pythonxm {


class Controller;
class CudaBackendPrivate;
class Miner;


class CudaBackend : public IBackend
{
public:
    PYTHONXM_DISABLE_COPY_MOVE_DEFAULT(CudaBackend)

    CudaBackend(Controller *controller);

    ~CudaBackend() override;

protected:
    bool isEnabled() const override;
    bool isEnabled(const Algorithm &algorithm) const override;
    const Hashrate *hashrate() const override;
    const String &profileName() const override;
    const String &type() const override;
    void execCommand(char command) override;
    void prepare(const Job &nextJob) override;
    void printHashrate(bool details) override;
    void printHealth() override;
    void setJob(const Job &job) override;
    void start(IWorker *worker, bool ready) override;
    void stop() override;
    bool tick(uint64_t ticks) override;

#   ifdef PYTHONXM_FEATURE_API
    rapidjson::Value toJSON(rapidjson::Document &doc) const override;
    void handleRequest(IApiRequest &request) override;
#   endif

#   ifdef PYTHONXM_FEATURE_BENCHMARK
    inline Benchmark *benchmark() const override        { return nullptr; }
    inline void printBenchProgress() const override     {}
#   endif

private:
    CudaBackendPrivate *d_ptr;
};


} /* namespace pythonxm */


#endif /* PYTHONXM_CUDABACKEND_H */
