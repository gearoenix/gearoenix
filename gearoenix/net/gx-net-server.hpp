#pragma once

#include <atomic>
#include <functional>
#include <unordered_map>
#include <memory>
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
    std::unordered_map<ENetPeer*, std::weak_ptr<ServerClient>> clients;
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

    /// This function provides an early security and early out for the server
    ///
    /// When an actor is acting strangely or out of expectation, pass it to the corresponding
    /// server; the server at first puts it in a yellow-carded list of clients after that if for
    /// three times the client is reported as a bad actor, it will totally ignore the client actions for 3 minutes.
    void bad_client(ServerClient&);

};
}