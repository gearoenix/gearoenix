#include "gx-net-client.hpp"

#include "../core/macro/gx-cr-mcr-assert.hpp"
#include "../platform/gx-plt-log.hpp"

#include <enet/enet.h>

gearoenix::net::Client::Client(
    std::string&& addr,
    const std::uint16_t p,
    std::function<void()>&& disconnect_c,
    std::function<void(std::vector<std::byte>&&)>&& data_c,
    std::function<void(std::shared_ptr<Client>&&)>&& connection_c)
    : address(std::move(addr))
    , port(p)
    , disconnection_callback(std::move(disconnect_c))
    , receive_callback(std::move(data_c))
    , thread([this, connection_callback = std::make_optional(std::move(connection_c))]() mutable {
        auto self = weak_self.lock();
        while (!self) {
            std::this_thread::yield();
            self = weak_self.lock();
        }
        running = true;

        auto* const host = enet_host_create(nullptr, 1, 1, 0, 0);
        if (!host) {
            GX_LOG_D("ENet client connection failed.");
            (*connection_callback)(nullptr);
            return;
        }

        enet_host_compress_with_range_coder(host);

        ENetAddress enet_address;
        if (enet_address_set_host(&enet_address, address.c_str()) != 0) {
            GX_LOG_D("Failed to resolve host address: " << address << " port: " << port);
            enet_host_destroy(host);
            (*connection_callback)(nullptr);
            return;
        }

        enet_address.port = port;
        auto* const peer = enet_host_connect(host, &enet_address, 1, 0);
        if (!peer) {
            GX_LOG_D("Failed to initiate connection peer: " << address << " port: " << port);
            enet_host_destroy(host);
            (*connection_callback)(nullptr);
            return;
        }

        auto local_data = decltype(data){};
        while (running) {
            {
                std::lock_guard _(data_lock);
                std::swap(local_data, data);
            }

            if (!local_data.empty()) {
                auto* const enet_packet = enet_packet_create(local_data.data(), local_data.size(), ENET_PACKET_FLAG_RELIABLE);
                GX_ASSERT_D(enet_packet);
                if (0 != enet_peer_send(peer, 0, enet_packet)) {
                    GX_LOG_E("Failed to send the data: " << address << " port: " << port);
                    disconnection_callback();
                    enet_peer_disconnect_now(peer, 0);
                    enet_host_destroy(host);
                    return;
                }
                local_data.clear();
            }

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
                    disconnection_callback();
                    enet_peer_disconnect_now(peer, 0);
                    enet_host_destroy(host);
                    return;

                case ENET_EVENT_TYPE_RECEIVE:
                    if (event.packet) {
                        std::vector received_data(
                            reinterpret_cast<const std::byte*>(event.packet->data),
                            reinterpret_cast<const std::byte*>(event.packet->data) + event.packet->dataLength);
                        enet_packet_destroy(event.packet);
                        receive_callback(std::move(received_data));
                    }
                    GX_ASSERT_D(event.packet);
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

        enet_peer_disconnect_now(peer, 0);
        enet_host_destroy(host);
    })
{
}

void gearoenix::net::Client::construct(
    std::string&& address,
    const std::uint16_t port,
    std::function<void()>&& disconnection_callback,
    std::function<void(std::vector<std::byte>&&)>&& data_callback,
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
    thread.join();
}

void gearoenix::net::Client::send(const std::span<const std::byte> d)
{
    const std::lock_guard _(data_lock);
    data.insert(data.end(), d.begin(), d.end());
}