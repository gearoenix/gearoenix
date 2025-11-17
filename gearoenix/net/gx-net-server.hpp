#pragma once

#include <atomic>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <span>
#include <thread>

#include <enet/enet.h>

namespace gearoenix::net {
struct ServerClient;
struct Server final {
    friend struct Manager;

    typedef std::function<void(std::shared_ptr<ServerClient>&&)> new_client_callback_t;

    const std::uint16_t port;
    const std::uint64_t max_clients;

private:
    const new_client_callback_t new_client_callback;
    ENetHost* const host;
    std::atomic<bool> running;
    std::shared_ptr<std::thread> thread;
    std::mutex clients_lock;
    std::map<ENetPeer*, std::weak_ptr<ServerClient>> clients;
    std::weak_ptr<Server> weak_self;

    Server(std::uint16_t p, std::uint64_t cc, new_client_callback_t&& on_connect);
    [[nodiscard]] static std::shared_ptr<Server> construct(std::uint16_t port, std::uint64_t max_clients, new_client_callback_t&& on_connect);
    void create_thread();
    void terminate();

public:
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(Server&&) = delete;

    ~Server();

    /// Broadcast data to all connected clients
    /// Thread-safe, can be called from any thread
    /// @param data Data to broadcast
    void broadcast(std::span<const std::byte> data) const;

};
}