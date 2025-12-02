#pragma once

#include "../core/sync/gx-cr-sync-semaphore.hpp"
#include "gx-net-client.hpp"
#include "gx-net-manager.hpp"
#include "gx-net-server-client.hpp"
#include "gx-net-server.hpp"

#include <array>
#include <atomic>

BOOST_AUTO_TEST_CASE(gearoenix_net_manager_000)
{
    using GxClient = gearoenix::net::Client;
    using GxServerClient = gearoenix::net::ServerClient;
    using GxManager = gearoenix::net::Manager;
    using GxSemaphore = gearoenix::core::sync::Semaphore;

    GxSemaphore clients_init_sem;
    GxSemaphore receive_sem;
    GxSemaphore disconnect_sem;

    std::array<std::shared_ptr<GxServerClient>, 2> server_clients;
    std::atomic server_clients_count = 0;

    const std::array clients_messages { std::string("Hello from first client!"), std::string("Bonjour du deuxième client!") };
    const std::array server_messages { std::string("Hello from first server!"), std::string("Bonjour du serveur!") };

    constexpr std::uint16_t server_port = 14567;

    auto server = GxManager::get().construct_server(
        server_port, 1000,
        [&](std::shared_ptr<GxServerClient>&& c) {
            BOOST_TEST(c);

            const auto client_index = server_clients_count++;
            BOOST_TEST(client_index < server_clients.size());

            c->disconnected_callback = [&, client_index] {
                BOOST_TEST(server_clients[client_index]);
                server_clients[client_index] = nullptr;
                disconnect_sem.release();
            };

            c->received_callback = [&, client_index](std::vector<std::uint8_t>&& data) {
                BOOST_TEST(clients_messages[client_index] == std::string(reinterpret_cast<const char*>(data.data()), data.size()));
                receive_sem.release();
            };

            BOOST_TEST(server_clients[client_index] == nullptr);
            server_clients[client_index] = std::move(c);

            clients_init_sem.release();
        });

    BOOST_TEST(server);

    std::array<std::shared_ptr<GxClient>, server_clients.size()> clients;

    bool client0_disconnected_from_server = false;
    GxManager::get().construct_client(
        "127.0.0.1", server_port,
        [&] {
            BOOST_TEST(!client0_disconnected_from_server);
            client0_disconnected_from_server = true;
            disconnect_sem.release();
        },
        [&](std::vector<std::uint8_t>&& data) {
            const auto msg = std::string(reinterpret_cast<const char*>(data.data()), data.size());
            BOOST_TEST(server_messages[0] == msg);
            receive_sem.release();
        },
        [&](std::shared_ptr<GxClient>&& client) {
            BOOST_TEST(client);
            BOOST_TEST(clients[0] == nullptr);
            clients[0] = std::move(client);
            clients_init_sem.release();
        });

    std::this_thread::yield();
    clients_init_sem.lock(); // client
    std::this_thread::yield();
    clients_init_sem.lock(); // server
    std::this_thread::yield();

    GxManager::get().construct_client(
        "127.0.0.1", server_port,
        [&] {
            // This client will close by client side.
            BOOST_TEST(false);
        },
        [&](std::vector<std::uint8_t>&& data) {
            const auto msg = std::string(reinterpret_cast<const char*>(data.data()), data.size());
            BOOST_TEST(server_messages[1] == msg);
            receive_sem.release();
        },
        [&](std::shared_ptr<GxClient>&& client) {
            BOOST_TEST(client);
            BOOST_TEST(clients[1] == nullptr);
            clients[1] = std::move(client);
            clients_init_sem.release();
        });

    std::this_thread::yield();
    clients_init_sem.lock(); // client
    std::this_thread::yield();
    clients_init_sem.lock(); // server
    std::this_thread::yield();

    for (const auto& c : server_clients) {
        while (!c) {
            std::this_thread::yield();
        }
    }

    for (auto i = decltype(clients)::size_type { 0 }; i < clients.size(); ++i) {
        BOOST_TEST(clients[i]);
        std::vector<std::uint8_t> d(clients_messages[i].size(), 0);
        std::memcpy(d.data(), clients_messages[i].data(), clients_messages[i].size());
        clients[i]->send(std::move(d));
    }

    for (auto i = decltype(server_clients)::size_type { 0 }; i < server_clients.size(); ++i) {
        BOOST_TEST(server_clients[i]);
        std::vector<std::uint8_t> d(server_messages[i].size(), 0);
        std::memcpy(d.data(), server_messages[i].data(), server_messages[i].size());
        BOOST_TEST(server_clients[i]->send(std::move(d)));
    }

    for (const auto& c : server_clients) {
        BOOST_TEST(c);
        receive_sem.lock();
    }

    for (const auto& c : clients) {
        BOOST_TEST(c);
        receive_sem.lock();
    }

    server = nullptr;
    clients[1] = nullptr;
    server_clients[0] = nullptr;

    disconnect_sem.lock();
    disconnect_sem.lock();
    BOOST_TEST(client0_disconnected_from_server);
    BOOST_TEST(!server_clients[1]);

    clients[0] = nullptr;
    server_clients[1] = nullptr;
}