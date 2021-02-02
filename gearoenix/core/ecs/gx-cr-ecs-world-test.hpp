#ifndef GEAROENIX_CORE_ECS_WORLD_TEST_HPP
#define GEAROENIX_CORE_ECS_WORLD_TEST_HPP
#include "gx-cr-ecs-world.hpp"

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

    w.parallel_system<Position>([&](const Entity::id_t ent, Position& p) {
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

    w.parallel_system<Not<Speed>>([&](const Entity::id_t ent, Not<Speed>&) {
        if (ent == e2)
            e2s = true;
        else if (ent == e5)
            e5s = true;
        else
            BOOST_TEST(false);
    });

    BOOST_TEST((e2s && e5s));
    e2s = e5s = false;

    w.parallel_system<Not<Position>>([&](const Entity::id_t ent, Not<Position>&) {
        if (ent == e4)
            e4s = true;
        else if (ent == e5)
            e5s = true;
        else
            BOOST_TEST(false);
    });

    BOOST_TEST((e4s && e5s));
    e4s = e5s = false;

    w.parallel_system<Speed>([&](const Entity::id_t ent, Speed& s) {
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

    w.parallel_system<Speed, Position>([&](const Entity::id_t ent, Speed& s, Position& p) {
        if (e1 == ent) {
            e1s = true;
            BOOST_TEST(2.0 == p.x);
            BOOST_TEST(3.0 == p.y);
            BOOST_TEST(4.0 == s.x);
            BOOST_TEST(5.0 == s.y);
        } else if (e3 == ent) {
            e3s = true;
            BOOST_TEST(8.0 == p.x);
            BOOST_TEST(9.0 == p.y);
            BOOST_TEST(10.0 == s.x);
            BOOST_TEST(11.0 == s.y);
        } else {
            BOOST_TEST(false);
        }
    });

    BOOST_TEST((e1s && e3s));
    e1s = e3s = false;

    w.parallel_system<Not<Speed>, Position>([&](const Entity::id_t ent, Not<Speed>&, Position& p) {
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

    w.parallel_system<Speed, Not<Position>>([&](const Entity::id_t ent, Speed& s, Not<Position>&) {
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

BOOST_AUTO_TEST_CASE(gearoenix_core_ecs_world)
{

    e1 = w.create_entity(Position { 2.0, 3.0 }, Speed { 4.0, 5.0 });
    e2 = w.create_entity(Position { 6.0, 7.0 });
    e3 = w.create_entity(Speed { 10.0, 11.0 }, Position { 8.0, 9.0 });
    e4 = w.create_entity(Speed { 12.0, 13.0 });
    e5 = w.create_entity();

    check_components();

    check_systems();

    w.remove_entity(e1);
    w.remove_entity(e2);
    w.remove_entity(e3);
    w.remove_entity(e4);
    w.remove_entity(e5);
}

#endif