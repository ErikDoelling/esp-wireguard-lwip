/*
 * Copyright (c) 2021 Erik Dölling (www.zesys.de)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *  list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this
 *  list of conditions and the following disclaimer in the documentation and/or
 *  other materials provided with the distribution.
 *
 * 3. Neither the name of "ZeSys e.V." nor the names of
 *  its contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Special thanks to Daniel Hope <daniel.hope@smartalock.com>
 * Author: Erik Dölling <doelling@zesys.de>
 */

#ifndef __ESP_WIREGUARD_H__
#define __ESP_WIREGUARD_H__

#include <memory>
#include <cinttypes>
#include <string>
#include <vector>
extern "C"
{
#include "wireguard.h"
#include "wireguardif.h"
#include "lwip/netif.h"
#include "lwip/ip.h"

#ifdef ESP_IDF_VERSION_MAJOR
#include "esp_event.h"
#include "esp_log.h"
#endif
}

typedef enum
{
    WG_CONNECTION_LOST = 2,
    WG_CONNECTED = 3
} wg_status_t;

class WireguardClass
{
private:
    struct wireguardif_init_data _wg {0};
    static netif _wg_netif_struct;
    netif *_wg_netif{NULL};
    uint8_t _wg_peer_index{WIREGUARDIF_INVALID_INDEX};
    bool _default_netif{true};

    ip_addr_t _ipaddr;
    ip_addr_t _netmask;
    ip_addr_t _gateway;

public:
    bool begin(std::string m_privKey, uint16_t m_listenPort, netif *m_bindNetif,
               std::string m_ipaddr, std::string m_netmask, std::string m_gateway);
    bool connect();
    bool disconnect();
    bool addPeer(std::string m_pubKey, std::string m_psk,
                 std::string m_allowedIPs, std::string m_allowedMask,
                 std::string m_endpointIP, unsigned short m_endpointPort,
                 unsigned short m_keepalive);
    bool removePeer(std::string m_pubKey);
    bool linkUp();
    bool linkDown();
};



extern WireguardClass Wireguard;

#endif // __ESP_WIREGUARD_H__
