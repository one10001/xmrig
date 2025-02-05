/* PythonXM
 * Copyright (c) 2018-2020 SChernykh   <https://github.com/SChernykh>
 * Copyright (c) 2016-2020 PythonXM       <https://github.com/pythonxm>, <support@pythonxm.com>
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

#ifndef PYTHONXM_DNS_H
#define PYTHONXM_DNS_H


#include <vector>
#include <uv.h>


#include "base/net/dns/DnsRecord.h"
#include "base/net/tools/Storage.h"
#include "base/tools/Object.h"
#include "base/tools/String.h"


namespace pythonxm {


class IDnsListener;


class Dns
{
public:
    PYTHONXM_DISABLE_COPY_MOVE_DEFAULT(Dns)

    Dns(IDnsListener *listener);
    ~Dns();

    inline bool isEmpty() const       { return m_ipv4.empty() && m_ipv6.empty(); }
    inline const String &host() const { return m_host; }
    inline int status() const         { return m_status; }

    bool resolve(const String &host);
    const char *error() const;
    const DnsRecord &get(DnsRecord::Type prefered = DnsRecord::A) const;
    size_t count(DnsRecord::Type type = DnsRecord::Unknown) const;

private:
    void clear();
    void onResolved(int status, addrinfo *res);

    static void onResolved(uv_getaddrinfo_t *req, int status, addrinfo *res);

    addrinfo m_hints{};
    IDnsListener *m_listener;
    int m_status                    = 0;
    std::vector<DnsRecord> m_ipv4;
    std::vector<DnsRecord> m_ipv6;
    String m_host;
    uintptr_t m_key;
    uv_getaddrinfo_t *m_resolver    = nullptr;

    static Storage<Dns> m_storage;
};


} /* namespace pythonxm */


#endif /* PYTHONXM_DNS_H */
