#include "gx-net-server.hpp"
#include "../core/job/gx-cr-job-manager.hpp"
#include "../platform/gx-plt-log.hpp"
#include "gx-net-server-client.hpp"

#include <algorithm>

namespace {
std::unordered_map<std::uint32_t, std::uint8_t> yellow_list;
std::unordered_map<std::uint32_t, std::chrono::steady_clock::time_point> black_list;
}

gearoenix::net::Server::Server(const std::uint16_t p, const std::uint64_t cc, new_client_callback_t&& on_connect)
    : port(p)
    , max_clients(cc)
    , new_client_callback(std::move(on_connect))
    , host([p, cc] {
        const ENetAddress address {
            .host = ENET_HOST_ANY,
            .port = p,
        };
        auto* const h = enet_host_create(&address, cc, 1, 0, 0);
        return h;
    }())
{
    static bool not_initialized = true;
    if (not_initialized) {
        not_initialized = false;

        yellow_list.reserve(1 << 16);
        black_list.reserve(1 << 16);
    }

    clients.reserve(max_clients);
}

std::shared_ptr<gearoenix::net::Server> gearoenix::net::Server::construct(
    const std::uint16_t port, const std::uint64_t max_clients, new_client_callback_t&& on_connect)
{
    std::shared_ptr<Server> server(new Server(port, max_clients, std::move(on_connect)));
    if (!server->host) {
        GX_LOG_E("Failed to create ENet host on port " << port);
        return nullptr;
    }
    server->running = false;
    server->weak_self = server;
    server->create_thread();
    while (!server->running) {
        std::this_thread::yield();
    }
    return server;
}

void gearoenix::net::Server::create_thread()
{
    thread = std::make_shared<std::thread>([this] {
        enet_host_compress_with_range_coder(host);

        auto self = weak_self.lock();
        while (!self) {
            std::this_thread::yield();
            self = weak_self.lock();
        }

        auto weak = std::weak_ptr(self);

        ENetEvent event;

        running = true;
        while (running) {
            self.reset();
            self = weak.lock();
            if (!self) {
                return;
            }

            const auto service_result = enet_host_service(host, &event, 1);

            if (service_result == 0) {
                continue;
            }

            if (service_result < 0) {
                GX_LOG_E("ENet service error, service_result: " << service_result);
                terminate();
                return;
            }

            // blacklist handling
            if (event.peer) {
                if (const auto search = black_list.find(event.peer->address.host); search != black_list.end()) {
                    if (std::chrono::steady_clock::now() - search->second < std::chrono::seconds(180)) {
                        continue;
                    }
                    black_list.erase(search);
                }
            }

            switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                if (!event.peer) {
                    GX_LOG_E("ENet client peer is null");
                    break;
                }

                auto new_client = std::shared_ptr<ServerClient>(new ServerClient(event.peer, std::move(self)));
                clients[new_client->peer] = new_client;

                // I can't send it to Job Manager.
                // I have to make sure of the precedence of client initialisation and the packet receiving.
                // So this is the only way to make sure that the API user is ready for receiving the packets.
                new_client_callback(std::move(new_client));
                break;
            }

            case ENET_EVENT_TYPE_RECEIVE: {
                if (!event.packet) {
                    GX_LOG_E("Packet received with null");
                    break;
                }
                const auto search = clients.find(event.peer);
                if (clients.end() == search) {
                    GX_LOG_E("Received packet from a client that is not inside the clients map!");
                    break;
                }
                auto c = search->second.lock();
                if (!c) {
                    GX_LOG_E("Client deleted during the packet receiving");
                    clients.erase(search);
                    break;
                }
                core::job::send_job_to_pool([packet = event.packet, c = std::move(c)] {
                    std::vector<std::uint8_t> data;
                    data.assign(
                        reinterpret_cast<const std::uint8_t*>(packet->data),
                        reinterpret_cast<const std::uint8_t*>(packet->data) + packet->dataLength);

                    c->received_callback(std::move(data));
                    enet_packet_destroy(packet);
                });
                break;
            }

            case ENET_EVENT_TYPE_DISCONNECT: {
                const auto search = clients.find(event.peer);
                if (clients.end() == search) {
                    break;
                }
                auto c = search->second.lock();
                if (!c) {
                    clients.erase(search);
                    break;
                }
                core::job::send_job_to_pool([c = std::move(c)] {
                    c->disconnected_callback();
                    c->clean();
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
            // Enable the following if you see any latency issue in the future.
            // enet_host_flush(host);
        }
    });
}

void gearoenix::net::Server::terminate()
{
    running = false;
    core::job::send_job_to_pool([t = thread, h = host] {
        t->join();
        if (h) {
            enet_host_destroy(h);
        }
        GX_LOG_D("ENet server stopped");
    });
    thread.reset();
}

gearoenix::net::Server::~Server()
{
    terminate();
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

void gearoenix::net::Server::bad_client(ServerClient& c)
{
    c.disconnected_callback();
    if (2 < ++yellow_list[c.peer->address.host]) {
        black_list[c.peer->address.host] = std::chrono::steady_clock::now();
    }
    clients.erase(c.peer);
    c.clean();
}