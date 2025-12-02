#pragma once
#include "../core/gx-cr-singleton.hpp"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace gearoenix::net {
struct Client;
struct Server;
struct ServerClient;

struct Manager final : core::Singleton<Manager> {
    Manager();
    ~Manager() override;

    /// Constructs and configures a client for network communication.
    ///
    /// @param address The address of the server to connect to, provided as an rvalue reference to a string.
    /// @param port The port number of the server to connect to.
    /// @param disconnection_callback Callback function invoked when the client is disconnected.
    /// @param data_callback Callback function invoked when data is received from the server.
    ///        The received data is passed as an rvalue reference to a vector of bytes.
    /// @param connection_callback Callback function invoked when the client successfully establishes a connection.
    ///        The newly created client is passed as an rvalue reference to a shared pointer. If the connection
    ///        wasn't successful, it will receive a nullptr.
    void construct_client(
        std::string&& address,
        std::uint16_t port,
        std::function<void()>&& disconnection_callback,
        std::function<void(std::vector<std::uint8_t>&&)>&& data_callback,
        std::function<void(std::shared_ptr<Client>&&)>&& connection_callback) const;

    /// Constructs and configures a server for network communication.
    ///
    /// @param port The port number on which the server will listen for incoming connections.
    /// @param max_clients Maximum number of concurrent clients the server is expected to handle.
    /// @param on_connect Callback invoked when a new client has successfully connected to this server.
    ///        The user must immediately and quickly set all required callbacks on the provided
    ///        `ServerClient` instance; otherwise, some early client events (such as `receive` or
    ///        `disconnect`) may be lost.
    ///        This callback must return quickly and must not perform long‑running work
    ///        because it is executed on the server's main thread (not on a worker thread).
    ///        Blocking in this callback will stall the server loop and degrade overall
    ///        server performance.
    ///        The callback is executed on the Server's thread so that the server can guarantee
    ///        that the `ServerClient` is fully initialised and ready to receive events
    ///        before any packets are processed. This simplifies both the public API and
    ///        the internal implementation.
    ///        For more info look at ServerClient's doc.
    [[nodiscard]] std::shared_ptr<Server> construct_server(
        std::uint16_t port,
        std::uint64_t max_clients,
        std::function<void(std::shared_ptr<ServerClient>&&)>&& on_connect) const;
};

}