/* PythonXM
 * Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
 * Copyright 2017-2019 XMR-Stak    <https://github.com/fireice-uk>, <https://github.com/psychocrypt>
 * Copyright 2018      Lee Clagett <https://github.com/vtnerd>
 * Copyright 2018-2019 tevador     <tevador@gmail.com>
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


#include "crypto/rx/RxQueue.h"
#include "backend/common/interfaces/IRxListener.h"
#include "base/io/Async.h"
#include "base/io/log/Log.h"
#include "base/io/log/Tags.h"
#include "base/tools/Cvt.h"
#include "crypto/rx/RxBasicStorage.h"


#ifdef PYTHONXM_FEATURE_HWLOC
#   include "crypto/rx/RxNUMAStorage.h"
#endif


pythonxm::RxQueue::RxQueue(IRxListener *listener) :
    m_listener(listener)
{
    m_async  = std::make_shared<Async>(this);
    m_thread = std::thread(&RxQueue::backgroundInit, this);
}


pythonxm::RxQueue::~RxQueue()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_state = STATE_SHUTDOWN;
    lock.unlock();

    m_cv.notify_one();

    m_thread.join();

    delete m_storage;
}


pythonxm::RxDataset *pythonxm::RxQueue::dataset(const Job &job, uint32_t nodeId)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (isReadyUnsafe(job)) {
        return m_storage->dataset(job, nodeId);
    }

    return nullptr;
}


pythonxm::HugePagesInfo pythonxm::RxQueue::hugePages()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_storage && m_state == STATE_IDLE ? m_storage->hugePages() : HugePagesInfo();
}


template<typename T>
bool pythonxm::RxQueue::isReady(const T &seed)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return isReadyUnsafe(seed);
}


void pythonxm::RxQueue::enqueue(const RxSeed &seed, const std::vector<uint32_t> &nodeset, uint32_t threads, bool hugePages, bool oneGbPages, RxConfig::Mode mode, int priority)
{
    std::unique_lock<std::mutex> lock(m_mutex);

    if (!m_storage) {
#       ifdef PYTHONXM_FEATURE_HWLOC
        if (!nodeset.empty()) {
            m_storage = new RxNUMAStorage(nodeset);
        }
        else
#       endif
        {
            m_storage = new RxBasicStorage();
        }
    }

    if (m_state == STATE_PENDING && m_seed == seed) {
        return;
    }

    m_queue.emplace_back(seed, nodeset, threads, hugePages, oneGbPages, mode, priority);
    m_seed  = seed;
    m_state = STATE_PENDING;

    lock.unlock();

    m_cv.notify_one();
}


template<typename T>
bool pythonxm::RxQueue::isReadyUnsafe(const T &seed) const
{
    return m_storage != nullptr && m_storage->isAllocated() && m_state == STATE_IDLE && m_seed == seed;
}


void pythonxm::RxQueue::backgroundInit()
{
    while (m_state != STATE_SHUTDOWN) {
        std::unique_lock<std::mutex> lock(m_mutex);

        if (m_state == STATE_IDLE) {
            m_cv.wait(lock, [this]{ return m_state != STATE_IDLE; });
        }

        if (m_state != STATE_PENDING) {
            continue;
        }

        const auto item = m_queue.back();
        m_queue.clear();

        lock.unlock();

        LOG_INFO("%s" MAGENTA_BOLD("init dataset%s") " algo " WHITE_BOLD("%s (") CYAN_BOLD("%u") WHITE_BOLD(" threads)") BLACK_BOLD(" seed %s..."),
                 Tags::randomx(),
                 item.nodeset.size() > 1 ? "s" : "",
                 item.seed.algorithm().shortName(),
                 item.threads,
                 Cvt::toHex(item.seed.data().data(), 8).data()
                 );

        m_storage->init(item.seed, item.threads, item.hugePages, item.oneGbPages, item.mode, item.priority);

        lock.lock();

        if (m_state == STATE_SHUTDOWN || !m_queue.empty()) {
            continue;
        }

        m_state = STATE_IDLE;
        m_async->send();
    }
}


void pythonxm::RxQueue::onReady()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    const bool ready = m_listener && m_state == STATE_IDLE;
    lock.unlock();

    if (ready) {
        m_listener->onDatasetReady();
    }
}


namespace pythonxm {


template bool RxQueue::isReady(const Job &);
template bool RxQueue::isReady(const RxSeed &);


} // namespace pythonxm
