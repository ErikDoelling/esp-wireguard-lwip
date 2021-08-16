#include "ESP-Wireguard.h"

const char *wg_tag = "Wireguard";

bool WireguardClass::begin(std::string m_privKey, uint16_t m_listenPort, netif *m_bindNetif,
                           std::string m_ipaddr, std::string m_netmask, std::string m_gateway)
{
    if (m_privKey.length() != WIREGUARD_PRIVATE_KEY_LEN)
    {
        ESP_LOGE(wg_tag, "Private Key to short");
        return false;
    }

    this->_wg.private_key = m_privKey.c_str(),
    this->_wg.listen_port = m_listenPort;
    this->_wg.bind_netif = m_bindNetif;

    if (!ipaddr_aton(m_ipaddr.c_str(), &this->_ipaddr))
    {
        ESP_LOGE(wg_tag, "IP address wrong");
        return false;
    }
    if (!ipaddr_aton(m_netmask.c_str(), &this->_netmask))
    {
        ESP_LOGE(wg_tag, "Netmask wrong");
        return false;
    }
    if (!ipaddr_aton(m_gateway.c_str(), &this->_gateway))
    {
        ESP_LOGE(wg_tag, "Gateway address wrong");
        return false;
    }
    this->_wg_netif = netif_add(&this->_wg_netif_struct,
                                &this->_ipaddr.u_addr.ip4,
                                &this->_netmask.u_addr.ip4,
                                &this->_gateway.u_addr.ip4,
                                &this->_wg,
                                &wireguardif_init,
                                &ip_input);
    netif_set_default(this->_wg_netif);
    netif_set_hostname(this->_wg_netif, "Wireguard");
}

bool WireguardClass::addPeer(std::string m_pubKey, std::string m_psk,
                             std::string m_allowedIPs, std::string m_allowedMask,
                             std::string m_endpointIP, unsigned short m_endpointPort,
                             unsigned short m_keepalive)
{
    static struct wireguardif_peer peer;

    wireguardif_peer_init(&peer);

    peer.public_key = m_pubKey.c_str();
    peer.preshared_key = (const uint8_t *)m_psk.c_str();
    ipaddr_aton(m_allowedIPs.c_str(), &peer.allowed_ip);
    ipaddr_aton(m_allowedMask.c_str(), &peer.allowed_mask);
    ipaddr_aton(m_endpointIP.c_str(), &peer.endpoint_ip);
    peer.endport_port = m_endpointPort;
    peer.keep_alive = m_keepalive;

    err_t result = wireguardif_add_peer(this->_wg_netif, &peer, &(this->_wg_peer_index));
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
    err_t result = wireguardif_connect(this->_wg_netif, this->_wg_peer_index);
    if (result == ERR_ARG) {
        ESP_LOGE(wg_tag, "Illegal argument");
        return false;
    }
}

bool WireguardClass::disconnect()
{
    err_t result = wireguardif_disconnect(this->_wg_netif, this->_wg_peer_index);
    if (result != ERR_OK) {
        ESP_LOGE(wg_tag, "Diconnect failed");
        return false;
    }
    return true;
}

bool WireguardClass::linkUp()
{
    netif_set_up(this->_wg_netif);
}

bool WireguardClass::linkDown()
{
    netif_set_down(this->_wg_netif);
}