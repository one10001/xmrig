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

#ifndef PYTHONXM_BENCHCONFIG_H
#define PYTHONXM_BENCHCONFIG_H


#include "base/crypto/Algorithm.h"
#include "base/tools/String.h"


namespace pythonxm {


class BenchConfig
{
public:
    static const char *kAlgo;
    static const char *kApiHost;
    static const char *kBenchmark;
    static const char *kHash;
    static const char *kId;
    static const char *kSeed;
    static const char *kSize;
    static const char *kSubmit;
    static const char *kToken;
    static const char *kVerify;

#   ifndef PYTHONXM_DEBUG_BENCHMARK_API
    static constexpr bool kApiTLS               = true;
    static constexpr const uint16_t kApiPort    = 443;
#   else
    static constexpr bool kApiTLS               = false;
    static constexpr const uint16_t kApiPort    = 18805;
#   endif

    BenchConfig(uint32_t size, const String &id, const rapidjson::Value &object, bool dmi);

    static BenchConfig *create(const rapidjson::Value &object, bool dmi);

    inline bool isDMI() const                   { return m_dmi; }
    inline bool isSubmit() const                { return m_submit; }
    inline const Algorithm &algorithm() const   { return m_algorithm; }
    inline const String &id() const             { return m_id; }
    inline const String &seed() const           { return m_seed; }
    inline const String &token() const          { return m_token; }
    inline uint32_t size() const                { return m_size; }
    inline uint64_t hash() const                { return m_hash; }

    rapidjson::Value toJSON(rapidjson::Document &doc) const;

private:
    static uint32_t getSize(const char *benchmark);

    Algorithm m_algorithm;
    bool m_dmi;
    bool m_submit;
    String m_id;
    String m_seed;
    String m_token;
    uint32_t m_size;
    uint64_t m_hash;
};


} /* namespace pythonxm */


#endif /* PYTHONXM_BENCHCONFIG_H */
