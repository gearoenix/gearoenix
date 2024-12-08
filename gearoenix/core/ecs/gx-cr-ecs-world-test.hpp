#pragma once
#include "gx-cr-ecs-comp-allocator.hpp"
#include "gx-cr-ecs-world.hpp"
#include <cmath>

namespace gearoenix_core_ecs_world_test {
struct Position final : gearoenix::core::ecs::Component {
    constexpr static std::uint32_t MAX_COUNT = 100;
    constexpr static gearoenix::core::ecs::component_index_t TYPE_INDEX = 100;
    constexpr static gearoenix::core::ecs::component_index_set_t ALL_PARENT_TYPE_INDICES {};
    constexpr static gearoenix::core::ecs::component_index_set_t IMMEDIATE_PARENT_TYPE_INDICES {};
    double x;
    double y;

    Position(const double x, const double y, const gearoenix::core::ecs::entity_id_t entity_id)
        : Component(gearoenix::core::ecs::ComponentType::create_index(this), "position", entity_id)
        , x(x)
        , y(y)
    {
    }

    ~Position() override = default;
};

struct Speed final : gearoenix::core::ecs::Component {
    constexpr static std::uint32_t MAX_COUNT = 100;
    constexpr static gearoenix::core::ecs::component_index_t TYPE_INDEX = 101;
    constexpr static gearoenix::core::ecs::component_index_set_t ALL_PARENT_TYPE_INDICES {};
    constexpr static gearoenix::core::ecs::component_index_set_t IMMEDIATE_PARENT_TYPE_INDICES {};

    double x;
    double y;

    Speed(const double x, const double y, const gearoenix::core::ecs::entity_id_t entity_id)
        : Component(gearoenix::core::ecs::ComponentType::create_index(this), "speed", entity_id)
        , x(x)
        , y(y)
    {
    }

    ~Speed() override = default;
};
}

BOOST_AUTO_TEST_CASE(gearoenix_core_ecs_world)
{
    using namespace gearoenix_core_ecs_world_test;
    using namespace gearoenix::core::ecs;
    using namespace gearoenix::core::job;
    using namespace gearoenix::core::sync;

    ComponentType::add<Position>();
    ComponentType::add<Speed>();

    static entity_id_t e1, e2, e3, e4, e5;
    World& w = *World::get();

    const auto check_components = [&]() {
        if (auto* const p = w.get_component<Position>(e1); nullptr != p) {
            BOOST_TEST(p->x == 2.0);
            BOOST_TEST(p->y == 3.0);
        }
        if (auto* const s = w.get_component<Speed>(e1); nullptr != s) {
            BOOST_TEST(s->x == 4.0);
            BOOST_TEST(s->y == 5.0);
        }
        if (auto* const p = w.get_component<Position>(e2); nullptr != p) {
            BOOST_TEST(p->x == 6.0);
            BOOST_TEST(p->y == 7.0);
        }
        if (auto* const p = w.get_component<Position>(e3); nullptr != p) {
            BOOST_TEST(p->x == 8.0);
            BOOST_TEST(p->y == 9.0);
        }
        if (auto* const s = w.get_component<Speed>(e3); nullptr != s) {
            BOOST_TEST(s->x == 10.0);
            BOOST_TEST(s->y == 11.0);
        }
        if (auto* const s = w.get_component<Speed>(e4); nullptr != s) {
            BOOST_TEST(s->x == 12.0);
            BOOST_TEST(s->y == 13.0);
        }
    };

    const auto check_systems = [&]() {
        std::atomic<int> e1s = 0, e2s = 0, e3s = 0, e4s = 0, e5s = 0;

        w.parallel_system<All<Position>>([&](const entity_id_t ent, const Position* const p, const unsigned int) {
            if (e1 == ent && 2.0 == p->x && 3.0 == p->y) {
                ++e1s;
            } else if (e2 == ent && 6.0 == p->x && 7.0 == p->y) {
                ++e2s;
            } else if (e3 == ent && 8.0 == p->x && 9.0 == p->y) {
                ++e3s;
            } else {
                BOOST_TEST(false);
            }
        });

        BOOST_TEST((e1s == 1 && e2s == 1 && e3s == 1));
        e1s = e2s = e3s = 0;

        w.parallel_system<Not<Speed>>([&](const entity_id_t ent, Speed* const, const unsigned int) {
            if (ent == e2)
                ++e2s;
            else if (ent == e5)
                ++e5s;
            else
                BOOST_TEST(false);
        });

        BOOST_TEST((e2s == 1 && e5s == 1));
        e2s = e5s = 0;

        w.parallel_system<Not<Position>>([&](const entity_id_t ent, Position* const, const unsigned int) {
            if (ent == e4)
                ++e4s;
            else if (ent == e5)
                ++e5s;
            else
                BOOST_TEST(false);
        });

        BOOST_TEST((e4s == 1 && e5s == 1));
        e4s = e5s = 0;

        w.parallel_system<Speed>([&](const entity_id_t ent, const Speed* const s, const unsigned int) {
            if (e1 == ent && 4.0 == s->x && 5.0 == s->y) {
                ++e1s;
            } else if (e3 == ent && 10.0 == s->x && 11.0 == s->y) {
                ++e3s;
            } else if (e4 == ent && 12.0 == s->x && 13.0 == s->y) {
                ++e4s;
            } else {
                BOOST_TEST(false);
            }
        });

        BOOST_TEST((e1s == 1 && e3s == 1 && e4s == 1));
        e1s = e3s = e4s = 0;

        w.parallel_system<All<Speed, Position>>([&](const entity_id_t ent, const Speed* const s, const Position* const p, const unsigned int) {
            if (e1 == ent && 2 == std::lround(p->x) && 3 == std::lround(p->y) && 4 == std::lround(s->x) && 5 == std::lround(s->y)) {
                ++e1s;
            } else if (e3 == ent && 8 == std::lround(p->x) && 9 == std::lround(p->y) && 10 == std::lround(s->x) && 11 == std::lround(s->y)) {
                ++e3s;
            } else {
                BOOST_TEST(false);
            }
        });

        BOOST_TEST((e1s == 1 && e3s == 1));
        e1s = e3s = 0;

        w.parallel_system<All<Not<Speed>, Position>>([&](const entity_id_t ent, const Speed* const, const Position* const p, const unsigned int) {
            if (e2 == ent && 6.0 == p->x && 7.0 == p->y) {
                ++e2s;
            } else {
                BOOST_TEST(false);
            }
        });

        BOOST_TEST(e2s == 1);
        e2s = 0;

        w.parallel_system<All<Speed, Not<Position>>>([&](const entity_id_t ent, const Speed* const s, Position*, const unsigned int) {
            if (e4 == ent && 12.0 == s->x && 13.0 == s->y) {
                ++e4s;
            } else {
                BOOST_TEST(false);
            }
        });

        BOOST_TEST(e4s == 1);
        e4s = 0;

        w.parallel_system<Any<All<Speed, Position>, Speed>>([&](const entity_id_t ent, const Speed* const s, Position*, const unsigned int) {
            if (e1 == ent && 4.0 == s->x && 5.0 == s->y) {
                ++e1s;
            } else if (e3 == ent && 10.0 == s->x && 11.0 == s->y) {
                ++e3s;
            } else if (e4 == ent && 12.0 == s->x && 13.0 == s->y) {
                ++e4s;
            } else {
                BOOST_TEST(false);
            }
        });

        BOOST_TEST((e1s == 1 && e3s == 1 && e4s == 1));
        e1s = e3s = e4s = 0;
    };

    const auto& delete_entities = [&]() {
        w.delete_entity(e1);
        w.delete_entity(e2);
        w.delete_entity(e3);
        w.delete_entity(e4);
        w.delete_entity(e5);
    };

    const auto& end_of_step = [&]() {
        check_components();
        check_systems();
        delete_entities();
    };

    {
        auto b = EntityBuilder("1", EndCaller<>([] { }));
        b.add_components(construct_component<Position>(2.0, 3.0, b.get_id()), construct_component<Speed>(4.0, 5.0, b.get_id()));
        e1 = b.get_id();
        w.create_entity(std::move(b));
    }

    {
        auto b = EntityBuilder("2", EndCaller<>([] { }));
        b.add_components(construct_component<Position>(6.0, 7.0, b.get_id()));
        e2 = b.get_id();
        w.create_entity(std::move(b));
    }

    {
        auto b = EntityBuilder("3", EndCaller<>([] { }));
        b.add_components(construct_component<Speed>(10.0, 11.0, b.get_id()), construct_component<Position>(8.0, 9., b.get_id()));
        e3 = b.get_id();
        w.create_entity(std::move(b));
    }

    {
        auto b = EntityBuilder("4", EndCaller<>([] { }));
        b.add_components(construct_component<Speed>(12.0, 13.0, b.get_id()));
        e4 = b.get_id();
        w.create_entity(std::move(b));
    }

    {
        auto b = EntityBuilder("5", EndCaller<>([] { }));
        e5 = b.get_id();
        w.create_entity(std::move(b));
    }

    end_of_step();

    {
        auto b = EntityBuilder("1", EndCaller<>([] { }));
        e1 = b.get_id();
        w.create_entity(std::move(b));
    }

    {
        auto b = EntityBuilder("2", EndCaller<>([] { }));
        e2 = b.get_id();
        w.create_entity(std::move(b));
    }

    {
        auto b = EntityBuilder("3", EndCaller<>([] { }));
        e3 = b.get_id();
        w.create_entity(std::move(b));
    }

    {
        auto b = EntityBuilder("4", EndCaller<>([] { }));
        e4 = b.get_id();
        w.create_entity(std::move(b));
    }

    {
        auto b = EntityBuilder("5", EndCaller<>([] { }));
        e5 = b.get_id();
        w.create_entity(std::move(b));
    }

    w.add_components(e1, construct_component<Position>(2.0, 3.0, e1), construct_component<Speed>(4.0, 5.0, e1));
    w.add_components(e2, construct_component<Position>(6.0, 7.0, e2));
    w.add_components(e3, construct_component<Speed>(10.0, 11.0, e3), construct_component<Position>(8.0, 9.0, e3));
    w.add_components(e4, construct_component<Speed>(12.0, 13.0, e4));

    end_of_step();

    {
        auto b = EntityBuilder("1", EndCaller<>([] { }));
        e1 = b.get_id();
        w.create_entity(std::move(b));
    }

    {
        auto b = EntityBuilder("2", EndCaller<>([] { }));
        e2 = b.get_id();
        w.create_entity(std::move(b));
    }

    {
        auto b = EntityBuilder("3", EndCaller<>([] { }));
        e3 = b.get_id();
        w.create_entity(std::move(b));
    }

    {
        auto b = EntityBuilder("4", EndCaller<>([] { }));
        e4 = b.get_id();
        w.create_entity(std::move(b));
    }

    {
        auto b = EntityBuilder("5", EndCaller<>([] { }));
        e5 = b.get_id();
        w.create_entity(std::move(b));
    }

    w.add_components(e1, construct_component<Position>(2.0, 3.0, e1), construct_component<Speed>(4.0, 5.0, e1));
    w.add_components(e2, construct_component<Position>(6.0, 7.0, e2), construct_component<Speed>(-1.0, -1.0, e2));
    w.add_components(e3, construct_component<Speed>(10.0, 11.0, e3), construct_component<Position>(8.0, 9.0, e3));
    w.add_components(e4, construct_component<Speed>(12.0, 13.0, e4), construct_component<Position>(-1.0, -1.0, e4));
    w.add_components(e5, construct_component<Speed>(-1.0, -1.0, e5), construct_component<Position>(-1.0, -1.0, e5));

    w.remove_components<Speed>(e2);
    w.remove_components<Position>(e4);
    w.remove_components<Position, Speed>(e5);

    end_of_step();

    // Delayed tests -------------------------------------------------------------

    {
        auto b = EntityBuilder("1", EndCaller<>([] { }));
        b.add_components(construct_component<Position>(2.0, 3.0, b.get_id()), construct_component<Speed>(4.0, 5.0, b.get_id()));
        e1 = b.get_id();
        w.delayed_create_entity(std::move(b));
    }

    {
        auto b = EntityBuilder("2", EndCaller<>([] { }));
        b.add_components(construct_component<Position>(6.0, 7.0, b.get_id()));
        e2 = b.get_id();
        w.delayed_create_entity(std::move(b));
    }

    {
        auto b = EntityBuilder("3", EndCaller<>([] { }));
        b.add_components(construct_component<Speed>(10.0, 11.0, b.get_id()), construct_component<Position>(8.0, 9.0, b.get_id()));
        e3 = b.get_id();
        w.delayed_create_entity(std::move(b));
    }

    {
        auto b = EntityBuilder("4", EndCaller<>([] { }));
        b.add_components(construct_component<Speed>(12.0, 13.0, b.get_id()));
        e4 = b.get_id();
        w.delayed_create_entity(std::move(b));
    }

    {
        auto b = EntityBuilder("5", EndCaller<>([] { }));
        e5 = b.get_id();
        w.delayed_create_entity(std::move(b));
    }

    w.update();
    end_of_step();

    EntityBuilder b21("21", EndCaller<>([] { })),
        b22("22", EndCaller<>([] { })),
        b23("23", EndCaller<>([] { })),
        b24("24", EndCaller<>([] { })),
        b25("25", EndCaller<>([] { }));

    b21.add_components(construct_component<Position>(2.0, 3.0, b21.get_id()), construct_component<Speed>(4.0, 5.0, b21.get_id()));
    b22.add_component(construct_component<Position>(6.0, 7.0, b22.get_id()));
    b23.add_components(construct_component<Speed>(10.0, 11.0, b23.get_id()), construct_component<Position>(8.0, 9.0, b23.get_id()));
    b24.add_components(construct_component<Speed>(12.0, 13.0, b24.get_id()));

    e1 = b21.get_id();
    w.delayed_create_entity(std::move(b21));
    e2 = b22.get_id();
    w.delayed_create_entity(std::move(b22));
    e3 = b23.get_id();
    w.delayed_create_entity(std::move(b23));
    e4 = b24.get_id();
    w.delayed_create_entity(std::move(b24));
    e5 = b25.get_id();
    w.delayed_create_entity(std::move(b25));

    w.update();
    end_of_step();

    {
        auto b = EntityBuilder("1", EndCaller<>([] { }));
        e1 = b.get_id();
        w.delayed_create_entity(std::move(b));
    }

    {
        auto b = EntityBuilder("2", EndCaller<>([] { }));
        e2 = b.get_id();
        w.delayed_create_entity(std::move(b));
    }

    {
        auto b = EntityBuilder("3", EndCaller<>([] { }));
        e3 = b.get_id();
        w.delayed_create_entity(std::move(b));
    }

    {
        auto b = EntityBuilder("4", EndCaller<>([] { }));
        e4 = b.get_id();
        w.delayed_create_entity(std::move(b));
    }

    {
        auto b = EntityBuilder("5", EndCaller<>([] { }));
        e5 = b.get_id();
        w.delayed_create_entity(std::move(b));
    }

    w.delayed_add_components(e1, EndCaller<>([] { }), construct_component<Position>(2.0, 3.0, e1), construct_component<Speed>(4.0, 5.0, e1));
    w.delayed_add_components(e2, EndCaller<>([] { }), construct_component<Position>(6.0, 7.0, e2));
    w.delayed_add_components(e3, EndCaller<>([] { }), construct_component<Speed>(10.0, 11.0, e3), construct_component<Position>(8.0, 9.0, e3));
    w.delayed_add_components(e4, EndCaller<>([] { }), construct_component<Speed>(12.0, 13.0, e4));

    w.update();
    end_of_step();

    {
        auto b = EntityBuilder("1", EndCaller<>([] { }));
        e1 = b.get_id();
        w.delayed_create_entity(std::move(b));
    }

    {
        auto b = EntityBuilder("2", EndCaller<>([] { }));
        e2 = b.get_id();
        w.delayed_create_entity(std::move(b));
    }

    {
        auto b = EntityBuilder("3", EndCaller<>([] { }));
        e3 = b.get_id();
        w.delayed_create_entity(std::move(b));
    }

    {
        auto b = EntityBuilder("4", EndCaller<>([] { }));
        e4 = b.get_id();
        w.delayed_create_entity(std::move(b));
    }

    {
        auto b = EntityBuilder("5", EndCaller<>([] { }));
        e5 = b.get_id();
        w.delayed_create_entity(std::move(b));
    }

    w.delayed_add_components(e1, EndCaller([] { }), construct_component<Position>(2.0, 3.0, e1), construct_component<Speed>(4.0, 5.0, e1));
    w.delayed_add_components(e2, EndCaller([] { }), construct_component<Position>(6.0, 7.0, e2), construct_component<Speed>(-1.0, -1.0, e2));
    w.delayed_add_components(e3, EndCaller([] { }), construct_component<Speed>(10.0, 11.0, e3), construct_component<Position>(8.0, 9.0, e3));
    w.delayed_add_components(e4, EndCaller([] { }), construct_component<Speed>(12.0, 13.0, e4), construct_component<Position>(-1.0, -1.0, e4));
    w.delayed_add_components(e5, EndCaller([] { }), construct_component<Speed>(-1.0, -1.0, e5), construct_component<Position>(-1.0, -1.0, e5));

    w.delayed_remove_components<Speed>(e2, EndCaller<>([] { }));
    w.delayed_remove_components<Position>(e4, EndCaller<>([] { }));
    w.delayed_remove_components<Position, Speed>(e5, EndCaller<>([] { }));

    w.update();
    end_of_step();
}