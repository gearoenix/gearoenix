#include "gx-net-server.hpp"
#include "../core/job/gx-cr-job-manager.hpp"
#include "../platform/gx-plt-log.hpp"
#include "gx-net-server-client.hpp"

#include <algorithm>
#include <bit>
#include <optional>

gearoenix::net::Server::Server(const std::uint16_t p, const std::uint64_t cc, new_client_callback_t&& on_connect)
    : port(p)
    , max_clients(cc)
    , new_client_callback(std::move(on_connect))
    , host([p, cc] {
        const ENetAddress address {
            .host = ENET_HOST_ANY,
            .port = p,
        };
        return enet_host_create(&address, cc, 1, 0, 0);
    }())
    , thread(new std::thread([this] {
        if (!host) {
            GX_LOG_E("Failed to create ENet host on port " << port);
            return;
        }
        enet_host_compress_with_range_coder(host);

        auto self = weak_self.lock();
        while (!self) {
            std::this_thread::yield();
            self = weak_self.lock();
        }
        running = true;

        auto weak = std::weak_ptr(self);
        self.reset();

        ENetEvent event;
        while (true) {
            self = weak.lock();
            if (!self) {
                return;
            }

            if (!running) {
                return;
            }

            const auto service_result = enet_host_service(host, &event, 1);

            if (service_result == 0) {
                continue;
            }

            if (service_result < 0) {
                GX_LOG_E("ENet service error, service_result: " << service_result);
                return;
            }

            switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                if (!event.peer) {
                    GX_LOG_E("ENet client peer is null");
                    return;
                }

                auto new_client = std::shared_ptr<ServerClient>(new ServerClient(event.peer, std::move(self)));

                {
                    const std::lock_guard _(clients_lock);
                    clients[new_client->peer] = new_client;
                }

                // I can't send it to Job Manager.
                // I have to make sure of the precedence of client initialisation and the packet receiving.
                // So this is the only way to make sure that the API user is ready for receiving the packets.
                new_client_callback(std::move(new_client));
                break;
            }

            case ENET_EVENT_TYPE_RECEIVE: {
                core::job::send_job_to_pool([this, self = std::move(self), packet = event.packet, peer = event.peer] {
                    if (!packet) {
                        GX_LOG_E("Packet received with null");
                        return;
                    }
                    const auto client = [&] {
                        const std::lock_guard _(clients_lock);
                        if (const auto it = clients.find(peer); it != clients.end()) {
                            auto c = it->second.lock();
                            if (!c) {
                                GX_LOG_E("Client deleted during the packet receiving");
                                clients.erase(it);
                            }
                            return c;
                        }
                        GX_LOG_E("Received packet from a client that is not inside the clients map!");
                        return std::shared_ptr<ServerClient> {};
                    }();
                    if (client) {
                        std::vector<std::byte> data;
                        data.assign(
                            reinterpret_cast<const std::byte*>(packet->data),
                            reinterpret_cast<const std::byte*>(packet->data) + packet->dataLength);

                            client->received_callback(std::move(data));

                    }
                    enet_packet_destroy(packet);
                });
                break;
            }

            case ENET_EVENT_TYPE_DISCONNECT: {
                core::job::send_job_to_pool([this, self = std::move(self), peer = event.peer] {
                    const auto client = [&] {
                        const std::lock_guard _(clients_lock);
                        if (const auto it = clients.find(peer); it != clients.end()) {
                            auto c = it->second.lock();
                            clients.erase(it);
                            return c;
                        }
                        return std::shared_ptr<ServerClient> {};
                    }();
                    if (client) {
                        client->disconnected_callback();
                    }
                });
                break;
            }

            case ENET_EVENT_TYPE_NONE:
                GX_LOG_D("None event in server while result of enet_host_service is positive.");
                break;

            default:
                GX_LOG_D("Unknown ENet event type: " << event.type);
                break;
            }

            self.reset();
            enet_host_flush(host);
        }
    }))
{
}

std::shared_ptr<gearoenix::net::Server> gearoenix::net::Server::construct(
    const std::uint16_t port, const std::uint64_t max_clients, new_client_callback_t&& on_connect)
{
    std::shared_ptr<Server> server(new Server(port, max_clients, std::move(on_connect)));
    server->running = false;
    server->weak_self = server;
    while (!server->running) {
        std::this_thread::yield();
    }
    return server;
}

gearoenix::net::Server::~Server()
{
    running = false;
    core::job::send_job_to_pool([t = thread, h = host] {
        t->join();
        enet_host_destroy(h);
        GX_LOG_D("ENet server stopped");
    });
}

void gearoenix::net::Server::broadcast(const std::span<const std::byte> data) const
{
    if (data.empty()) {
        return;
    }

    auto* const packet = enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE);
    if (!packet) {
        GX_LOG_E("Failed to create ENet packet for broadcast.");
        return;
    }

    enet_host_broadcast(host, 0, packet);
}