#include "gx-net-server-client.hpp"
#include "../platform/gx-plt-log.hpp"
#include "gx-net-server.hpp"

gearoenix::net::ServerClient::ServerClient(ENetPeer* const peer, std::shared_ptr<Server>&& server)
    : peer(peer)
    , server(std::move(server))
{
}

gearoenix::net::ServerClient::~ServerClient() { enet_peer_disconnect(peer, 0); }

bool gearoenix::net::ServerClient::send(const std::span<const std::uint8_t> data) const
{
    if (data.empty()) {
        return false;
    }

    auto* const packet = enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE);
    if (!packet) {
        GX_LOG_E("Failed to create ENet packet for sending.");
        return false;
    }

    return enet_peer_send(peer, 0, packet) == 0;
}

gearoenix::net::uniform_ip_t gearoenix::net::ServerClient::get_ip() const
{
    uniform_ip_t ip {};

    const auto ipv4 = peer->address.host;

    ip[0] = (ipv4 >> 24) & 0xFF;
    ip[1] = (ipv4 >> 16) & 0xFF;
    ip[2] = (ipv4 >> 8) & 0xFF;
    ip[3] = ipv4 & 0xFF;

    return ip;
}

void gearoenix::net::ServerClient::clean()
{
    disconnected_callback = [] { };
    received_callback = [](auto&&) { };
}