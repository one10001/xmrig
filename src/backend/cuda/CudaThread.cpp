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


#include "backend/cuda/CudaThread.h"
#include "3rdparty/rapidjson/document.h"
#include "backend/cuda/wrappers/CudaLib.h"
#include "base/io/json/Json.h"


#include <algorithm>


namespace pythonxm {

static const char *kAffinity    = "affinity";
static const char *kBFactor     = "bfactor";
static const char *kBlocks      = "blocks";
static const char *kBSleep      = "bsleep";
static const char *kIndex       = "index";
static const char *kThreads     = "threads";
static const char *kDatasetHost = "dataset_host";

} // namespace pythonxm


pythonxm::CudaThread::CudaThread(const rapidjson::Value &value)
{
    if (!value.IsObject()) {
        return;
    }

    m_index     = Json::getUint(value, kIndex);
    m_threads   = Json::getInt(value, kThreads);
    m_blocks    = Json::getInt(value, kBlocks);
    m_bfactor   = std::min(Json::getUint(value, kBFactor, m_bfactor), 12u);
    m_bsleep    = Json::getUint(value, kBSleep, m_bsleep);
    m_affinity  = Json::getUint64(value, kAffinity, m_affinity);

    if (Json::getValue(value, kDatasetHost).IsInt()) {
        m_datasetHost = Json::getInt(value, kDatasetHost, m_datasetHost) != 0;
    }
    else {
        m_datasetHost = Json::getBool(value, kDatasetHost);
    }
}


pythonxm::CudaThread::CudaThread(uint32_t index, nvid_ctx *ctx) :
    m_blocks(CudaLib::deviceInt(ctx, CudaLib::DeviceBlocks)),
    m_datasetHost(CudaLib::deviceInt(ctx, CudaLib::DeviceDatasetHost)),
    m_threads(CudaLib::deviceInt(ctx, CudaLib::DeviceThreads)),
    m_index(index),
    m_bfactor(CudaLib::deviceUint(ctx, CudaLib::DeviceBFactor)),
    m_bsleep(CudaLib::deviceUint(ctx, CudaLib::DeviceBSleep))
{

}


bool pythonxm::CudaThread::isEqual(const CudaThread &other) const
{
    return m_blocks      == other.m_blocks &&
           m_threads     == other.m_threads &&
           m_affinity    == other.m_affinity &&
           m_index       == other.m_index &&
           m_bfactor     == other.m_bfactor &&
           m_bsleep      == other.m_bsleep &&
           m_datasetHost == other.m_datasetHost;
}


rapidjson::Value pythonxm::CudaThread::toJSON(rapidjson::Document &doc) const
{
    using namespace rapidjson;
    auto &allocator = doc.GetAllocator();

    Value out(kObjectType);

    out.AddMember(StringRef(kIndex),        index(), allocator);
    out.AddMember(StringRef(kThreads),      threads(), allocator);
    out.AddMember(StringRef(kBlocks),       blocks(), allocator);
    out.AddMember(StringRef(kBFactor),      bfactor(), allocator);
    out.AddMember(StringRef(kBSleep),       bsleep(), allocator);
    out.AddMember(StringRef(kAffinity),     affinity(), allocator);

    if (m_datasetHost >= 0) {
        out.AddMember(StringRef(kDatasetHost), m_datasetHost > 0, allocator);
    }

    return out;
}
