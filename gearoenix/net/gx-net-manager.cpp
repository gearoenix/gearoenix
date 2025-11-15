#include "gx-net-manager.hpp"
#include "../core/macro/gx-cr-mcr-assert.hpp"
#include "gx-net-client.hpp"
#include "gx-net-server.hpp"

#include <enet/enet.h>

gearoenix::net::Manager::Manager()
    : Singleton(this)
{
    GX_ASSERT_D(enet_initialize() == 0);
}

gearoenix::net::Manager::~Manager()
{
    enet_deinitialize();
}

void gearoenix::net::Manager::construct_client(
    std::string&& address,
    const std::uint16_t port,
    std::function<void()>&& disconnection_callback,
    std::function<void(std::vector<std::byte>&&)>&& data_callback,
    std::function<void(std::shared_ptr<Client>&&)>&& connection_callback) const
{
    Client::construct(std::move(address), port, std::move(disconnection_callback), std::move(data_callback), std::move(connection_callback));
    (void)this;
}

std::shared_ptr<gearoenix::net::Server> gearoenix::net::Manager::construct_server(
    const std::uint16_t port, const std::uint64_t max_clients,
    std::function<void(std::shared_ptr<ServerClient>&&)>&& on_connect) const
{
    (void)this;
    return Server::construct(port, max_clients, std::move(on_connect));
}