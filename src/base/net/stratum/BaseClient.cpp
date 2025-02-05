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


#include "base/net/stratum/BaseClient.h"
#include "3rdparty/rapidjson/document.h"
#include "base/io/Env.h"
#include "base/io/log/Log.h"
#include "base/io/log/Tags.h"
#include "base/kernel/interfaces/IClientListener.h"
#include "base/net/stratum/SubmitResult.h"


namespace pythonxm {


int64_t BaseClient::m_sequence = 1;


} /* namespace pythonxm */


pythonxm::BaseClient::BaseClient(int id, IClientListener *listener) :
    m_listener(listener),
    m_id(id)
{
}


void pythonxm::BaseClient::setPool(const Pool &pool)
{
    if (!pool.isValid()) {
        return;
    }

    m_pool      = pool;
    m_user      = Env::expand(pool.user());
    m_password  = Env::expand(pool.password());
    m_rigId     = Env::expand(pool.rigId());
    m_tag       = std::string(Tags::network()) + " " CYAN_BOLD_S + m_pool.url().data() + CLEAR;
}


bool pythonxm::BaseClient::handleResponse(int64_t id, const rapidjson::Value &result, const rapidjson::Value &error)
{
    if (id == 1) {
        return false;
    }

    auto it = m_callbacks.find(id);
    if (it != m_callbacks.end()) {
        const uint64_t elapsed = Chrono::steadyMSecs() - it->second.ts;

        if (error.IsObject()) {
            it->second.callback(error, false, elapsed);
        }
        else {
            it->second.callback(result, true, elapsed);
        }

        m_callbacks.erase(it);

        return true;
    }

    return false;
}


bool pythonxm::BaseClient::handleSubmitResponse(int64_t id, const char *error)
{
    auto it = m_results.find(id);
    if (it != m_results.end()) {
        it->second.done();
        m_listener->onResultAccepted(this, it->second, error);
        m_results.erase(it);

        return true;
    }

    return false;
}
