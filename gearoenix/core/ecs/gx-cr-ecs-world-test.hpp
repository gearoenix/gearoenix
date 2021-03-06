#ifndef GEAROENIX_CORE_ECS_WORLD_TEST_HPP
#define GEAROENIX_CORE_ECS_WORLD_TEST_HPP
#include "gx-cr-ecs-world.hpp"
#include <cmath>

using namespace gearoenix::core::ecs;

struct Position final : public Component {
    double x;
    double y;

    Position(const double x, const double y) noexcept
        : Component(this)
        , x(x)
        , y(y)
    {
    }

    Position(Position&&) noexcept = default;
    Position(const Position&) = delete;
    Position& operator=(Position&&) = delete;
    Position& operator=(const Position&) = delete;
};

struct Speed final : public Component {
    double x;
    double y;

    Speed(const double x, const double y) noexcept
        : Component(this)
        , x(x)
        , y(y)
    {
    }

    Speed(Speed&&) noexcept = default;
    Speed(const Speed&) = delete;
    Speed& operator=(Speed&&) = delete;
    Speed& operator=(const Speed&) = delete;
};

static Entity::id_t e1, e2, e3, e4, e5;
static World w;

static void check_components() noexcept
{
    {
        auto& p = w.get_component<Position>(e1);
        auto& s = w.get_component<Speed>(e1);
        BOOST_TEST(p.x == 2.0);
        BOOST_TEST(p.y == 3.0);
        BOOST_TEST(s.x == 4.0);
        BOOST_TEST(s.y == 5.0);
    }
    {
        auto& p = w.get_component<Position>(e2);
        BOOST_TEST(p.x == 6.0);
        BOOST_TEST(p.y == 7.0);
    }
    {
        auto& p = w.get_component<Position>(e3);
        auto& s = w.get_component<Speed>(e3);
        BOOST_TEST(p.x == 8.0);
        BOOST_TEST(p.y == 9.0);
        BOOST_TEST(s.x == 10.0);
        BOOST_TEST(s.y == 11.0);
    }
    {
        auto& s = w.get_component<Speed>(e4);
        BOOST_TEST(s.x == 12.0);
        BOOST_TEST(s.y == 13.0);
    }
}

static void check_systems() noexcept
{
    bool e1s = false, e2s = false, e3s = false, e4s = false, e5s = false;

    w.parallel_system<Position>([&](const Entity::id_t ent, Position& p, const unsigned int) noexcept {
        if (e1 == ent) {
            e1s = true;
            BOOST_TEST(2.0 == p.x);
            BOOST_TEST(3.0 == p.y);
        } else if (e2 == ent) {
            e2s = true;
            BOOST_TEST(6.0 == p.x);
            BOOST_TEST(7.0 == p.y);
        } else if (e3 == ent) {
            e3s = true;
            BOOST_TEST(8.0 == p.x);
            BOOST_TEST(9.0 == p.y);
        } else {
            BOOST_TEST(false);
        }
    });

    BOOST_TEST((e1s && e2s && e3s));
    e1s = e2s = e3s = false;

    w.parallel_system<Not<Speed>>([&](const Entity::id_t ent, Not<Speed>&, const unsigned int) noexcept {
        if (ent == e2)
            e2s = true;
        else if (ent == e5)
            e5s = true;
        else
            BOOST_TEST(false);
    });

    BOOST_TEST((e2s && e5s));
    e2s = e5s = false;

    w.parallel_system<Not<Position>>([&](const Entity::id_t ent, Not<Position>&, const unsigned int) noexcept {
        if (ent == e4)
            e4s = true;
        else if (ent == e5)
            e5s = true;
        else
            BOOST_TEST(false);
    });

    BOOST_TEST((e4s && e5s));
    e4s = e5s = false;

    w.parallel_system<Speed>([&](const Entity::id_t ent, Speed& s, const unsigned int) noexcept {
        if (e1 == ent) {
            e1s = true;
            BOOST_TEST(4.0 == s.x);
            BOOST_TEST(5.0 == s.y);
        } else if (e3 == ent) {
            e3s = true;
            BOOST_TEST(10.0 == s.x);
            BOOST_TEST(11.0 == s.y);
        } else if (e4 == ent) {
            e4s = true;
            BOOST_TEST(12.0 == s.x);
            BOOST_TEST(13.0 == s.y);
        } else {
            BOOST_TEST(false);
        }
    });

    BOOST_TEST((e1s && e3s && e4s));
    e1s = e3s = e4s = false;

    w.parallel_system<Speed, Position>([&](const Entity::id_t ent, Speed& s, Position& p, const unsigned int) noexcept {
        if (e1 == ent) {
            e1s = true;
            BOOST_TEST(2 == std::lround(p.x));
            BOOST_TEST(3 == std::lround(p.y));
            BOOST_TEST(4 == std::lround(s.x));
            BOOST_TEST(5 == std::lround(s.y));
        } else if (e3 == ent) {
            e3s = true;
            BOOST_TEST(8 == std::lround(p.x));
            BOOST_TEST(9 == std::lround(p.y));
            BOOST_TEST(10 == std::lround(s.x));
            BOOST_TEST(11 == std::lround(s.y));
        } else {
            BOOST_TEST(false);
        }
    });

    BOOST_TEST((e1s && e3s));
    e1s = e3s = false;

    w.parallel_system<Not<Speed>, Position>([&](const Entity::id_t ent, Not<Speed>&, Position& p, const unsigned int) noexcept {
        if (e2 == ent) {
            e2s = true;
            BOOST_TEST(6.0 == p.x);
            BOOST_TEST(7.0 == p.y);
        } else {
            BOOST_TEST(false);
        }
    });

    BOOST_TEST(e2s);
    e2s = false;

    w.parallel_system<Speed, Not<Position>>([&](const Entity::id_t ent, Speed& s, Not<Position>&, const unsigned int) noexcept {
        if (e4 == ent) {
            e4s = true;
            BOOST_TEST(12.0 == s.x);
            BOOST_TEST(13.0 == s.y);
        } else {
            BOOST_TEST(false);
        }
    });

    BOOST_TEST(e4s);
}

static void delete_entities() noexcept
{
    w.remove_entity(e1);
    w.remove_entity(e2);
    w.remove_entity(e3);
    w.remove_entity(e4);
    w.remove_entity(e5);
}

static void end_of_step() noexcept
{
    check_components();
    check_systems();
    delete_entities();
}

BOOST_AUTO_TEST_CASE(gearoenix_core_ecs_world)
{
    e1 = w.create_entity(Position { 2.0, 3.0 }, Speed { 4.0, 5.0 });
    e2 = w.create_entity(Position { 6.0, 7.0 });
    e3 = w.create_entity(Speed { 10.0, 11.0 }, Position { 8.0, 9.0 });
    e4 = w.create_entity(Speed { 12.0, 13.0 });
    e5 = w.create_entity();

    end_of_step();

    EntityBuilder b1, b2, b3, b4, b5;

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

    e1 = w.create_entity();
    e2 = w.create_entity();
    e3 = w.create_entity();
    e4 = w.create_entity();
    e5 = w.create_entity();

    w.add_components(e1, Position { 2.0, 3.0 }, Speed { 4.0, 5.0 });
    w.add_components(e2, Position { 6.0, 7.0 });
    w.add_components(e3, Speed { 10.0, 11.0 }, Position { 8.0, 9.0 });
    w.add_components(e4, Speed { 12.0, 13.0 });

    end_of_step();

    e1 = w.create_entity();
    e2 = w.create_entity();
    e3 = w.create_entity();
    e4 = w.create_entity();
    e5 = w.create_entity();

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

    e1 = w.delayed_create_entity(Position { 2.0, 3.0 }, Speed { 4.0, 5.0 });
    e2 = w.delayed_create_entity(Position { 6.0, 7.0 });
    e3 = w.delayed_create_entity(Speed { 10.0, 11.0 }, Position { 8.0, 9.0 });
    e4 = w.delayed_create_entity(Speed { 12.0, 13.0 });
    e5 = w.delayed_create_entity();

    w.update();
    end_of_step();

    EntityBuilder b21, b22, b23, b24, b25;

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

    e1 = w.delayed_create_entity();
    e2 = w.delayed_create_entity();
    e3 = w.delayed_create_entity();
    e4 = w.delayed_create_entity();
    e5 = w.delayed_create_entity();

    w.delayed_add_components(e1, Position { 2.0, 3.0 }, Speed { 4.0, 5.0 });
    w.delayed_add_components(e2, Position { 6.0, 7.0 });
    w.delayed_add_components(e3, Speed { 10.0, 11.0 }, Position { 8.0, 9.0 });
    w.delayed_add_components(e4, Speed { 12.0, 13.0 });

    w.update();
    end_of_step();

    e1 = w.delayed_create_entity();
    e2 = w.delayed_create_entity();
    e3 = w.delayed_create_entity();
    e4 = w.delayed_create_entity();
    e5 = w.delayed_create_entity();

    w.delayed_add_components(e1, Position { 2.0, 3.0 }, Speed { 4.0, 5.0 });
    w.delayed_add_components(e2, Position { 6.0, 7.0 }, Speed { -1.0, -1.0 });
    w.delayed_add_components(e3, Speed { 10.0, 11.0 }, Position { 8.0, 9.0 });
    w.delayed_add_components(e4, Speed { 12.0, 13.0 }, Position { -1.0, -1.0 });
    w.delayed_add_components(e5, Speed { -1.0, -1.0 }, Position { -1.0, -1.0 });

    w.delayed_remove_components<Speed>(e2);
    w.delayed_remove_components<Position>(e4);
    w.delayed_remove_components<Position, Speed>(e5);

    w.update();
    end_of_step();
}

#endif