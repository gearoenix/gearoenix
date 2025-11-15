#include "gx-net-server-client.hpp"
#include "gx-net-server.hpp"
#include "../platform/gx-plt-log.hpp"

gearoenix::net::ServerClient::ServerClient(ENetPeer* const peer, std::shared_ptr<Server>&& server)
    : peer(peer)
    , server(std::move(server))
{
}

gearoenix::net::ServerClient::~ServerClient()
{
    enet_peer_disconnect(peer, 0);
}

bool gearoenix::net::ServerClient::send(const std::span<const std::byte> data) const
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