#pragma once

#include "../core/macro/gx-cr-mcr-getter-setter.hpp"

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <span>
#include <string>
#include <thread>
#include <vector>

namespace gearoenix::net {
struct Client final {
    friend struct Manager;

    GX_GET_REFC_PRV(std::string, address);
    GX_GET_CVAL_PRV(std::uint16_t, port);
    GX_GET_CREF_PRV(std::weak_ptr<Client>, weak_self);

    std::function<void()> disconnection_callback;
    std::function<void(std::vector<std::uint8_t>&&)> receive_callback;
    const std::shared_ptr<std::thread> thread;
    std::atomic<bool> running;
    std::mutex data_lock;
    std::vector<std::vector<std::uint8_t>> packets;

    Client(
        std::string&& addr,
        std::uint16_t p,
        std::function<void()>&& disconnect_c,
        std::function<void(std::vector<std::uint8_t>&&)>&& data_c,
        std::function<void(std::shared_ptr<Client>&&)>&& connection_c);
    static void construct(
        std::string&& address,
        std::uint16_t port,
        std::function<void()>&& disconnection_callback,
        std::function<void(std::vector<std::uint8_t>&&)>&& data_callback,
        std::function<void(std::shared_ptr<Client>&&)>&& connection_callback);

public:
    Client(const Client&) = delete;
    ~Client();
    void send(std::vector<std::uint8_t>&& packet);
};
}