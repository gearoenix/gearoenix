#ifndef GEAROENIX_CORE_ECS_WORLD_TEST_HPP
#define GEAROENIX_CORE_ECS_WORLD_TEST_HPP
#include "gx-cr-ecs-world.hpp"
#include <cmath>

BOOST_AUTO_TEST_CASE(gearoenix_core_ecs_world)
{
    using namespace gearoenix::core::ecs;
    using namespace gearoenix::core::sync;

    struct Position final : public Component {
        double x;
        double y;

        Position(const double x, const double y) noexcept
            : Component(this, "position")
            , x(x)
            , y(y)
        {
        }

        Position(Position&&) noexcept = default;
        ~Position() noexcept final = default;
    };

    Component::register_type<Position>();

    struct Speed final : public Component {
        double x;
        double y;

        Speed(const double x, const double y) noexcept
            : Component(this, "speed")
            , x(x)
            , y(y)
        {
        }

        Speed(Speed&&) noexcept = default;
        ~Speed() noexcept final = default;
    };

    Component::register_type<Speed>();

    static entity_id_t e1, e2, e3, e4, e5;
    static World w;

    const auto check_components = [&]() noexcept {
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

    const auto check_systems = [&]() noexcept {
        std::atomic<int> e1s = 0, e2s = 0, e3s = 0, e4s = 0, e5s = 0;

        w.parallel_system<And<Position>>([&](const entity_id_t ent, Position* const p, const unsigned int) noexcept {
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

        w.parallel_system<Not<Speed>>([&](const entity_id_t ent, Speed* const, const unsigned int) noexcept {
            if (ent == e2)
                ++e2s;
            else if (ent == e5)
                ++e5s;
            else
                BOOST_TEST(false);
        });

        BOOST_TEST((e2s == 1 && e5s == 1));
        e2s = e5s = 0;

        w.parallel_system<Not<Position>>([&](const entity_id_t ent, Position* const, const unsigned int) noexcept {
            if (ent == e4)
                ++e4s;
            else if (ent == e5)
                ++e5s;
            else
                BOOST_TEST(false);
        });

        BOOST_TEST((e4s == 1 && e5s == 1));
        e4s = e5s = 0;

        w.parallel_system<Speed>([&](const entity_id_t ent, Speed* const s, const unsigned int) noexcept {
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

        w.parallel_system<And<Speed, Position>>([&](const entity_id_t ent, Speed* const s, Position* const p, const unsigned int) noexcept {
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

        w.parallel_system<And<Not<Speed>, Position>>([&](const entity_id_t ent, Speed*, Position* const p, const unsigned int) noexcept {
            if (e2 == ent && 6.0 == p->x && 7.0 == p->y) {
                ++e2s;
            } else {
                BOOST_TEST(false);
            }
        });

        BOOST_TEST(e2s == 1);
        e2s = 0;

        w.parallel_system<And<Speed, Not<Position>>>([&](const entity_id_t ent, Speed* const s, Position*, const unsigned int) noexcept {
            if (e4 == ent && 12.0 == s->x && 13.0 == s->y) {
                ++e4s;
            } else {
                BOOST_TEST(false);
            }
        });

        BOOST_TEST(e4s == 1);
        e4s = 0;

        w.parallel_system<Or<And<Speed, Position>, Speed>>([&](const entity_id_t ent, Speed* const s, Position*, const unsigned int) noexcept {
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

    const auto& delete_entities = [&]() noexcept {
        w.remove_entity(e1);
        w.remove_entity(e2);
        w.remove_entity(e3);
        w.remove_entity(e4);
        w.remove_entity(e5);
    };

    const auto& end_of_step = [&]() noexcept {
        check_components();
        check_systems();
        delete_entities();
    };

    e1 = w.create_entity("1", Position { 2.0, 3.0 }, Speed { 4.0, 5.0 });
    e2 = w.create_entity("2", Position { 6.0, 7.0 });
    e3 = w.create_entity("3", Speed { 10.0, 11.0 }, Position { 8.0, 9.0 });
    e4 = w.create_entity("4", Speed { 12.0, 13.0 });
    e5 = w.create_entity("5");

    end_of_step();

    EntityBuilder b1("1", EndCaller([] {})),
        b2("2", EndCaller([] {})),
        b3("3", EndCaller([] {})),
        b4("4", EndCaller([] {})),
        b5("5", EndCaller([] {}));

    b1.add_components(Position { 2.0, 3.0 }, Speed { 4.0, 5.0 });
    b2.add_component(Position { 6.0, 7.0 });
    b3.add_components(Speed { 10.0, 11.0 }, Position { 8.0, 9.0 });
    b4.add_components(Speed { 12.0, 13.0 });

    e1 = b1.get_id();
    w.create_entity_with_builder(std::move(b1));
    e2 = b2.get_id();
    w.create_entity_with_builder(std::move(b2));
    e3 = b3.get_id();
    w.create_entity_with_builder(std::move(b3));
    e4 = b4.get_id();
    w.create_entity_with_builder(std::move(b4));
    e5 = b5.get_id();
    w.create_entity_with_builder(std::move(b5));

    end_of_step();

    e1 = w.create_entity("1");
    e2 = w.create_entity("2");
    e3 = w.create_entity("3");
    e4 = w.create_entity("4");
    e5 = w.create_entity("5");

    w.add_components(e1, Position { 2.0, 3.0 }, Speed { 4.0, 5.0 });
    w.add_components(e2, Position { 6.0, 7.0 });
    w.add_components(e3, Speed { 10.0, 11.0 }, Position { 8.0, 9.0 });
    w.add_components(e4, Speed { 12.0, 13.0 });

    end_of_step();

    e1 = w.create_entity("1");
    e2 = w.create_entity("2");
    e3 = w.create_entity("3");
    e4 = w.create_entity("4");
    e5 = w.create_entity("5");

    w.add_components(e1, Position { 2.0, 3.0 }, Speed { 4.0, 5.0 });
    w.add_components(e2, Position { 6.0, 7.0 }, Speed { -1.0, -1.0 });
    w.add_components(e3, Speed { 10.0, 11.0 }, Position { 8.0, 9.0 });
    w.add_components(e4, Speed { 12.0, 13.0 }, Position { -1.0, -1.0 });
    w.add_components(e5, Speed { -1.0, -1.0 }, Position { -1.0, -1.0 });

    w.remove_components<Speed>(e2);
    w.remove_components<Position>(e4);
    w.remove_components<Position, Speed>(e5);

    end_of_step();

    // Delayed tests -------------------------------------------------------------

    e1 = w.delayed_create_entity("1", EndCaller([] {}), Position { 2.0, 3.0 }, Speed { 4.0, 5.0 });
    e2 = w.delayed_create_entity("2", EndCaller([] {}), Position { 6.0, 7.0 });
    e3 = w.delayed_create_entity("3", EndCaller([] {}), Speed { 10.0, 11.0 }, Position { 8.0, 9.0 });
    e4 = w.delayed_create_entity("4", EndCaller([] {}), Speed { 12.0, 13.0 });
    e5 = w.delayed_create_entity("5", EndCaller([] {}));

    w.update();
    end_of_step();

    EntityBuilder b21("21", EndCaller([] {})),
        b22("22", EndCaller([] {})),
        b23("23", EndCaller([] {})),
        b24("24", EndCaller([] {})),
        b25("25", EndCaller([] {}));

    b21.add_components(Position { 2.0, 3.0 }, Speed { 4.0, 5.0 });
    b22.add_component(Position { 6.0, 7.0 });
    b23.add_components(Speed { 10.0, 11.0 }, Position { 8.0, 9.0 });
    b24.add_components(Speed { 12.0, 13.0 });

    e1 = b21.get_id();
    w.delayed_create_entity_with_builder(std::move(b21));
    e2 = b22.get_id();
    w.delayed_create_entity_with_builder(std::move(b22));
    e3 = b23.get_id();
    w.delayed_create_entity_with_builder(std::move(b23));
    e4 = b24.get_id();
    w.delayed_create_entity_with_builder(std::move(b24));
    e5 = b25.get_id();
    w.delayed_create_entity_with_builder(std::move(b25));

    w.update();
    end_of_step();

    e1 = w.delayed_create_entity("1", EndCaller([] {}));
    e2 = w.delayed_create_entity("2", EndCaller([] {}));
    e3 = w.delayed_create_entity("3", EndCaller([] {}));
    e4 = w.delayed_create_entity("4", EndCaller([] {}));
    e5 = w.delayed_create_entity("5", EndCaller([] {}));

    w.delayed_add_components(e1, EndCaller([] {}), Position { 2.0, 3.0 }, Speed { 4.0, 5.0 });
    w.delayed_add_components(e2, EndCaller([] {}), Position { 6.0, 7.0 });
    w.delayed_add_components(e3, EndCaller([] {}), Speed { 10.0, 11.0 }, Position { 8.0, 9.0 });
    w.delayed_add_components(e4, EndCaller([] {}), Speed { 12.0, 13.0 });

    w.update();
    end_of_step();

    e1 = w.delayed_create_entity("1", EndCaller([] {}));
    e2 = w.delayed_create_entity("2", EndCaller([] {}));
    e3 = w.delayed_create_entity("3", EndCaller([] {}));
    e4 = w.delayed_create_entity("4", EndCaller([] {}));
    e5 = w.delayed_create_entity("5", EndCaller([] {}));

    w.delayed_add_components(e1, EndCaller([] {}), Position { 2.0, 3.0 }, Speed { 4.0, 5.0 });
    w.delayed_add_components(e2, EndCaller([] {}), Position { 6.0, 7.0 }, Speed { -1.0, -1.0 });
    w.delayed_add_components(e3, EndCaller([] {}), Speed { 10.0, 11.0 }, Position { 8.0, 9.0 });
    w.delayed_add_components(e4, EndCaller([] {}), Speed { 12.0, 13.0 }, Position { -1.0, -1.0 });
    w.delayed_add_components(e5, EndCaller([] {}), Speed { -1.0, -1.0 }, Position { -1.0, -1.0 });

    w.delayed_remove_components<Speed>(e2);
    w.delayed_remove_components<Position>(e4);
    w.delayed_remove_components<Position, Speed>(e5);

    w.update();
    end_of_step();
}

#endif