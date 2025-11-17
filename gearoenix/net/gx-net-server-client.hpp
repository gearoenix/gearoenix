#pragma once

#include <functional>
#include <memory>
#include <span>
#include <vector>

#include <enet/enet.h>

namespace gearoenix::net {
struct Server;

/// Represents a single client connection managed by a Server instance.
///
/// This type is constructed and owned internally by Server; user code cannot
/// create instances directly. Instead, users receive an `std::shared_ptr<ServerClient>`
/// from the Server callback interface.
///
/// Lifetime and ownership:
/// - The shared pointer must be kept alive for as long as the user wants the
///   corresponding client connection to remain active.
/// - When the last shared pointer is destroyed, the underlying connection is
///   closed.
///
/// Callbacks:
/// - `received_callback` is invoked whenever a complete payload is received
///   from this client. Users must assign this callback to handle incoming data.
/// - `disconnected_callback` is invoked when the client disconnects on its own
///   (e.g. network disconnect or client-initiated shutdown).
///
/// Usage requirements:
/// - Both `received_callback` and `disconnected_callback` should be assigned
///   immediately after the Server reports a newly connected client; otherwise,
///   early incoming data or disconnection events may be missed.
/// - The connection may be terminated at any time by the remote client; user
///   code should not assume the connection is persistent.
struct ServerClient final {
    friend struct Server;

    ENetPeer* const peer;
    const std::shared_ptr<Server> server;

private:
    ServerClient(ENetPeer* peer, std::shared_ptr<Server>&& server);

public:
    std::function<void(std::vector<std::byte>&&)> received_callback = [](auto&&){};
    std::function<void()> disconnected_callback = []{};

    ~ServerClient();
    [[nodiscard]] bool send(std::span<const std::byte> data) const;
};
}