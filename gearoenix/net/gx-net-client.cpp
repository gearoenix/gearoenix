#include "gx-net-client.hpp"
#include "../core/job/gx-cr-job-manager.hpp"
#include "../core/macro/gx-cr-mcr-assert.hpp"
#include "../platform/gx-plt-log.hpp"

#include <enet/enet.h>

#include <optional>

gearoenix::net::Client::Client(
    std::string&& addr,
    const std::uint16_t p,
    std::function<void()>&& disconnect_c,
    std::function<void(std::vector<std::uint8_t>&&)>&& data_c,
    std::function<void(std::shared_ptr<Client>&&)>&& connection_c)
    : address(std::move(addr))
    , port(p)
    , disconnection_callback(std::move(disconnect_c))
    , receive_callback(std::move(data_c))
    , thread(new std::thread([this, connection_callback = std::make_optional(std::move(connection_c))]() mutable {
        auto self = weak_self.lock();
        while (!self) {
            std::this_thread::yield();
            self = weak_self.lock();
        }
        auto weak = std::weak_ptr(self);
        running = true;

        std::function<void()> terminate = [&] {
            if (connection_callback.has_value()) {
                (*connection_callback)(nullptr);
            }
            if (self) {
                disconnection_callback();
                disconnection_callback = [] { };
                receive_callback = [](auto&&) { };
                self.reset();
            }
        };

        auto* const host = enet_host_create(nullptr, 1, 1, 0, 0);
        if (!host) {
            GX_LOG_D("ENet client connection failed.");
            terminate();
            return;
        }
        terminate = [h = host, t = std::move(terminate)] {
            enet_host_destroy(h);
            t();
        };

        enet_host_compress_with_range_coder(host);

        ENetAddress enet_address;
        if (enet_address_set_host(&enet_address, address.c_str()) != 0) {
            GX_LOG_D("Failed to resolve host address: " << address << " port: " << port);
            terminate();
            return;
        }

        enet_address.port = port;
        auto* const peer = enet_host_connect(host, &enet_address, 1, 0);
        if (!peer) {
            GX_LOG_D("Failed to initiate connection peer: " << address << " port: " << port);
            terminate();
            return;
        }
        terminate = [p = peer, t = std::move(terminate)] {
            enet_peer_disconnect_now(p, 0);
            t();
        };

        auto local_packets = decltype(packets) {};
        while (true) {
            if (!connection_callback.has_value()) {
                // as long as we are waiting for connection, we shouldn't release self.
                self.reset(); // freeing if no other keeps it
                self = weak.lock(); // we need a live pointer.
                if (!self) {
                    terminate();
                    return;
                }
            }

            if (!running) {
                terminate();
                return;
            }

            {
                std::lock_guard _(data_lock);
                std::swap(local_packets, packets);
            }

            for (const auto& local_packet: local_packets) {
                if (local_packet.empty()) {
                    continue;
                }
                auto* const enet_packet = enet_packet_create(local_packet.data(), local_packet.size(), ENET_PACKET_FLAG_RELIABLE);
                GX_ASSERT_D(enet_packet);
                if (0 != enet_peer_send(peer, 0, enet_packet)) {
                    GX_LOG_E("Failed to send the data: " << address << " port: " << port);
                    terminate();
                    return;
                }
            }
            local_packets.clear();

            ENetEvent event;
            if (const auto service_result = enet_host_service(host, &event, 1); service_result > 0) {
                switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    if (connection_callback.has_value()) {
                        (*connection_callback)(std::move(self));
                        connection_callback = std::nullopt;
                    } else {
                        GX_LOG_D("Error: ENet double client connection!");
                    }
                    break;

                case ENET_EVENT_TYPE_DISCONNECT:
                    terminate();
                    return;

                case ENET_EVENT_TYPE_RECEIVE:
                    if (event.packet) {
                        GX_ASSERT_D(!connection_callback.has_value());
                        std::vector received_data(
                            reinterpret_cast<const std::uint8_t*>(event.packet->data),
                            reinterpret_cast<const std::uint8_t*>(event.packet->data) + event.packet->dataLength);
                        enet_packet_destroy(event.packet);
                        receive_callback(std::move(received_data));
                    }
                    GX_ASSERT_D(event.packet); // just to check if it is possible
                    break;

                case ENET_EVENT_TYPE_NONE:
                    break;
                }
            } else if (service_result < 0) {
                GX_LOG_D("ENet service error, address: " << address << " port: " << port);
                break;
            }

            enet_host_flush(host);
        }

        terminate();
    }))
{
}

void gearoenix::net::Client::construct(
    std::string&& address,
    const std::uint16_t port,
    std::function<void()>&& disconnection_callback,
    std::function<void(std::vector<std::uint8_t>&&)>&& data_callback,
    std::function<void(std::shared_ptr<Client>&&)>&& connection_callback)
{
    const std::shared_ptr<Client> result(new Client(
        std::move(address), port,
        std::move(disconnection_callback),
        std::move(data_callback),
        std::move(connection_callback)));
    result->running = false;
    result->weak_self = result;
    do {
        std::this_thread::yield();
    } while (!result->running);
}

gearoenix::net::Client::~Client()
{
    running = false;
    core::job::send_job_to_pool([t = thread] { t->join(); });
}

void gearoenix::net::Client::send(std::vector<std::uint8_t>&& packet)
{
    const std::lock_guard _(data_lock);
    packets.push_back(std::move(packet));
}