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


#include "backend/opencl/OclConfig.h"
#include "3rdparty/rapidjson/document.h"
#include "backend/common/Tags.h"
#include "backend/opencl/OclConfig_gen.h"
#include "backend/opencl/wrappers/OclLib.h"
#include "base/io/json/Json.h"
#include "base/io/log/Log.h"


namespace pythonxm {


static const char *kCache       = "cache";
static const char *kDevicesHint = "devices-hint";
static const char *kEnabled     = "enabled";
static const char *kLoader      = "loader";

#ifndef PYTHONXM_OS_APPLE
static const char *kAMD         = "AMD";
static const char *kINTEL       = "INTEL";
static const char *kNVIDIA      = "NVIDIA";
static const char *kPlatform    = "platform";
#endif

#ifdef PYTHONXM_FEATURE_ADL
static const char *kAdl         = "adl";
#endif


extern template class Threads<OclThreads>;


}


#ifndef PYTHONXM_OS_APPLE
pythonxm::OclConfig::OclConfig() : m_platformVendor(kAMD) {}
#else
pythonxm::OclConfig::OclConfig() = default;
#endif


pythonxm::OclPlatform pythonxm::OclConfig::platform() const
{
    const auto platforms = OclPlatform::get();
    if (platforms.empty()) {
        return {};
    }

#   ifndef PYTHONXM_OS_APPLE
    if (!m_platformVendor.isEmpty()) {
        String search;
        String vendor = m_platformVendor;
        vendor.toUpper();

        if (vendor == kAMD) {
            search = "Advanced Micro Devices";
        }
        else if (vendor == kNVIDIA) {
            search = kNVIDIA;
        }
        else if (vendor == kINTEL) {
            search = "Intel";
        }
        else {
            search = m_platformVendor;
        }

        for (const auto &platform : platforms) {
            if (platform.vendor().contains(search)) {
                return platform;
            }
        }
    }
    else if (m_platformIndex < platforms.size()) {
        return platforms[m_platformIndex];
    }

    return {};
#   else
    return platforms[0];
#   endif
}


rapidjson::Value pythonxm::OclConfig::toJSON(rapidjson::Document &doc) const
{
    using namespace rapidjson;
    auto &allocator = doc.GetAllocator();

    Value obj(kObjectType);

    obj.AddMember(StringRef(kEnabled),  m_enabled, allocator);
    obj.AddMember(StringRef(kCache),    m_cache, allocator);
    obj.AddMember(StringRef(kLoader),   m_loader.toJSON(), allocator);

#   ifndef PYTHONXM_OS_APPLE
    obj.AddMember(StringRef(kPlatform), m_platformVendor.isEmpty() ? Value(m_platformIndex) : m_platformVendor.toJSON(), allocator);
#   endif

#   ifdef PYTHONXM_FEATURE_ADL
    obj.AddMember(StringRef(kAdl),      m_adl, allocator);
#   endif

    m_threads.toJSON(obj, doc);

    return obj;
}


std::vector<pythonxm::OclLaunchData> pythonxm::OclConfig::get(const Miner *miner, const Algorithm &algorithm, const OclPlatform &platform, const std::vector<OclDevice> &devices) const
{
    std::vector<OclLaunchData> out;
    const auto &threads = m_threads.get(algorithm);

    if (threads.isEmpty()) {
        return out;
    }

    out.reserve(threads.count() * 2);

    for (const auto &thread : threads.data()) {
        if (thread.index() >= devices.size()) {
            LOG_INFO("%s" YELLOW(" skip non-existing device with index ") YELLOW_BOLD("%u"), ocl_tag(), thread.index());
            continue;
        }

        if (thread.threads().size() > 1) {
            for (int64_t affinity : thread.threads()) {
                out.emplace_back(miner, algorithm, *this, platform, thread, devices[thread.index()], affinity);
            }
        }
        else {
            out.emplace_back(miner, algorithm, *this, platform, thread, devices[thread.index()], thread.threads().front());
        }
    }

    return out;
}


void pythonxm::OclConfig::read(const rapidjson::Value &value)
{
    if (value.IsObject()) {
        m_enabled   = Json::getBool(value, kEnabled, m_enabled);
        m_cache     = Json::getBool(value, kCache, m_cache);
        m_loader    = Json::getString(value, kLoader);

#       ifndef PYTHONXM_OS_APPLE
        setPlatform(Json::getValue(value, kPlatform));
#       endif

        setDevicesHint(Json::getString(value, kDevicesHint));

#       ifdef PYTHONXM_FEATURE_ADL
        m_adl = Json::getBool(value, kAdl, m_adl);
#       endif

        m_threads.read(value);

        generate();
    }
    else if (value.IsBool()) {
        m_enabled = value.GetBool();

        generate();
    }
    else {
        m_shouldSave = true;

        generate();
    }
}


void pythonxm::OclConfig::generate()
{
    if (!isEnabled() || m_threads.has("*")) {
        return;
    }

    if (!OclLib::init(loader())) {
        return;
    }

    const auto devices = m_devicesHint.empty() ? platform().devices() : filterDevices(platform().devices(), m_devicesHint);
    if (devices.empty()) {
        return;
    }

    size_t count = 0;

    count += pythonxm::generate<Algorithm::CN>(m_threads, devices);
    count += pythonxm::generate<Algorithm::CN_LITE>(m_threads, devices);
    count += pythonxm::generate<Algorithm::CN_HEAVY>(m_threads, devices);
    count += pythonxm::generate<Algorithm::CN_PICO>(m_threads, devices);
    count += pythonxm::generate<Algorithm::RANDOM_X>(m_threads, devices);
    count += pythonxm::generate<Algorithm::ASTROBWT>(m_threads, devices);
    count += pythonxm::generate<Algorithm::KAWPOW>(m_threads, devices);

    m_shouldSave = count > 0;
}


void pythonxm::OclConfig::setDevicesHint(const char *devicesHint)
{
    if (devicesHint == nullptr) {
        return;
    }

    const auto indexes = String(devicesHint).split(',');
    m_devicesHint.reserve(indexes.size());

    for (const auto &index : indexes) {
        m_devicesHint.push_back(strtoul(index, nullptr, 10));
    }
}


#ifndef PYTHONXM_OS_APPLE
void pythonxm::OclConfig::setPlatform(const rapidjson::Value &platform)
{
    if (platform.IsString()) {
        m_platformVendor = platform.GetString();
    }
    else if (platform.IsUint()) {
        m_platformVendor = nullptr;
        m_platformIndex  = platform.GetUint();
    }
}
#endif
