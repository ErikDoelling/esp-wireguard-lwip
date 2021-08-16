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
 * 3. Neither the name of "Floorsense Ltd", "Agile Workspace Ltd" nor the names of
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
 * Author: Erik Dölling <doelling@zesys.de>
 */


#include "esp_wireguard.h"
#include <wireguard.h>
#include <wireguardif.h>
#include <lwip/netif.h>
#include <lwip/ip.h>

#define ESP_WG_DEBUG

#ifdef ESP_WG_DEBUG 
// wg_tag
#include "esp_log.h"
#include "esp_system.h"
const char *wg_tag = "ESP32-wireguard";
#endif
// static wg_netif struct
static struct netif wg_netif_struct;
// netif pointer
struct netif *wg_netif = NULL;
// wg_peer_index
uint8_t wg_peer_index = WIREGUARDIF_INVALID_INDEX;

/**
 * @brief Setup the Wireguard NetIf and connect to peer
 */
void esp_setup_wg()
{
	struct wireguardif_init_data wg = {0};
#ifdef ESP_WG_DEBUG 
	ESP_LOGI(wg_tag, "Start Wireguard");
#endif
	const ip_addr_t ipaddr = IPADDR4_INIT_BYTES(10, 88, 0, 2);
	const ip_addr_t netmask = IPADDR4_INIT_BYTES(255, 255, 255, 0);
	// const ip_addr_t gateway = IPADDR4_INIT_BYTES(10, 88, 0, 1);
	const ip_addr_t gateway = IPADDR4_INIT_BYTES(0, 0, 0, 0);

	err_t result;

	wg.private_key = "sPQOk2NwQEzbDaPYb4gysvVQ5Zx/XABVYBPgEDQez2g=";
	wg.listen_port = 52984;
	wg.bind_netif = NULL; // netif_find("st1");
	wg_netif = netif_add(&wg_netif_struct, &ipaddr.u_addr.ip4, &netmask.u_addr.ip4, &gateway.u_addr.ip4, &wg, &wireguardif_init, &ip_input);
	netif_set_up(wg_netif);
	netif_set_hostname(wg_netif, "IoT1-Wireguard");
	
	netif_set_default(wg_netif);

	static struct wireguardif_peer peer;
	wireguardif_peer_init(&peer);
	peer.public_key = "YRdLPHo19IiPusx7drhpr4J/lgyl2O6Lp1A+ZG8evmE=";
	peer.preshared_key = NULL;
	// peer.preshared_key = (const uint8_t*) "gxEn2YcCrJp1uBZzdaM0G8L3vzTNfHH4zSSs1RY/OVs=";
	// peer.preshared_key = "FvnMrgj38gHErT5CsSLfJnU5gZ0DRTHvRbvvJWWSog4=";

	// Allow all IPs through tunnel
	peer.allowed_ip = (ip_addr_t) IPADDR4_INIT_BYTES(0, 0, 0, 0);
	peer.allowed_mask = (ip_addr_t) IPADDR4_INIT_BYTES(0, 0, 0, 0);

	peer.endpoint_ip = (ip_addr_t) IPADDR4_INIT_BYTES(172, 16, 17, 1);
	peer.endport_port = 52984;
	// If we know the endpoint's address can add here

	// Register the new WireGuard peer with the netwok interface
	wireguardif_add_peer(wg_netif, &peer, &wg_peer_index);
	wireguardif_disconnect(wg_netif, wg_peer_index);
	if ((wg_peer_index != WIREGUARDIF_INVALID_INDEX) && !ip_addr_isany(&peer.endpoint_ip))
	{
		result = wireguardif_connect(wg_netif, wg_peer_index);
		if (result != ERR_OK)
		{
#ifdef ESP_WG_DEBUG 
			ESP_LOGI(wg_tag,"Connected to Peer");
#endif
			wireguardif_disconnect(wg_netif, wg_peer_index);
        }
	}
}


