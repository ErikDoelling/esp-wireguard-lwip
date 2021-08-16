#include "ESP-Wireguard.h"

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

const char *wg_tag = "Wireguard";

static struct netif _wg_netif_struct = {0};
static struct netif *_wg_netif = NULL;
static uint8_t _wg_peer_index = WIREGUARDIF_INVALID_INDEX;

bool WireguardClass::begin(const char* m_privKey, uint16_t m_listenPort, netif *m_bindNetif,
                           const char* m_ipaddr, const char* m_netmask, const char* m_gateway)
{
    struct wireguardif_init_data wg;
    this->_wg.private_key = m_privKey,
    this->_wg.listen_port = m_listenPort;
    this->_wg.bind_netif = m_bindNetif;

    if (!ipaddr_aton(m_ipaddr, &this->_ipaddr))
    {
        ESP_LOGE(wg_tag, "IP address wrong");
        return false;
    }
    if (!ipaddr_aton(m_netmask, &this->_netmask))
    {
        ESP_LOGE(wg_tag, "Netmask wrong");
        return false;
    }
    if (!ipaddr_aton(m_gateway, &this->_gateway))
    {
        ESP_LOGE(wg_tag, "Gateway address wrong");
        return false;
    }
    _wg_netif = netif_add(&_wg_netif_struct,
                                &this->_ipaddr.u_addr.ip4,
                                &this->_netmask.u_addr.ip4,
                                &this->_gateway.u_addr.ip4,
                                &wg,
                                &wireguardif_init,
                                &ip_input);
    netif_set_up(_wg_netif);
    // netif_set_hostname(_wg_netif, "Wireguard");
    return true;
}

bool WireguardClass::addPeer(const char* m_pubKey, const char* m_psk,
                             const char* m_allowedIPs, const char* m_allowedMask,
                             const char* m_endpointIP, unsigned int m_endpointPort,
                             unsigned int m_keepalive)
{
    struct wireguardif_peer peer;

    wireguardif_peer_init(&peer);

    peer.public_key = m_pubKey;
    peer.preshared_key = (const uint8_t *)m_psk;
    ipaddr_aton(m_allowedIPs, &peer.allowed_ip);
    ipaddr_aton(m_allowedMask, &peer.allowed_mask);
    ipaddr_aton(m_endpointIP, &peer.endpoint_ip);
    peer.endport_port = m_endpointPort;
    if (m_keepalive) {
        peer.keep_alive = m_keepalive;
    }

    err_t result = wireguardif_add_peer(_wg_netif, &peer, &_wg_peer_index);
    switch (result)
    {
    case ERR_ARG:
        ESP_LOGE(wg_tag, "Illegal argument");
        return false;
        break;
    case ERR_MEM:
        ESP_LOGE(wg_tag, " Out of memory");
        return false;
        break;
    default:

        return true;
    }
}

bool WireguardClass::connect()
{
    err_t result = wireguardif_connect(_wg_netif, _wg_peer_index);
    if (result == ERR_ARG) {
        ESP_LOGE(wg_tag, "Illegal argument");
        return false;
    }
    netif_set_default(_wg_netif);
    return true;
}

bool WireguardClass::disconnect()
{
    err_t result = wireguardif_disconnect(_wg_netif, _wg_peer_index);
    if (result != ERR_OK) {
        ESP_LOGE(wg_tag, "Diconnect failed");
        return false;
    }
    return true;
}

void WireguardClass::linkUp()
{
    netif_set_up(_wg_netif);
}

void WireguardClass::linkDown()
{
    netif_set_down(_wg_netif);
}

WireguardClass Wireguard;
