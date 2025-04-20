#pragma once
#include "gx-cr-ecs-entity.hpp"
#include "gx-cr-ecs-world.hpp"
#include <chrono>
#include <cmath>
#include <thread>

namespace gearoenix_core_ecs_world_test {
struct Position final : gearoenix::core::ecs::Component {
    constexpr static auto max_count = 100;
    constexpr static auto object_type_index = 100;
    double x;
    double y;

    Position(const double x, const double y)
        : Component(gearoenix::core::ecs::ComponentType::create_index(this), "position")
        , x(x)
        , y(y)
    {
    }

    ~Position() override = default;
};

struct Speed final : gearoenix::core::ecs::Component {
    constexpr static auto max_count = 100;
    constexpr static auto object_type_index = 101;

    double x;
    double y;

    Speed(const double x, const double y)
        : Component(gearoenix::core::ecs::ComponentType::create_index(this), "speed")
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
    using namespace gearoenix::core;

    ComponentType::add<Position>();
    ComponentType::add<Speed>();

    static EntityPtr e1, e2, e3, e4, e5;
    auto& w = World::get();

    w.clear();

    const auto check_components = [&] {
        if (auto* const p = e1->get_component<Position>(); nullptr != p) {
            BOOST_TEST(p->x == 2.0);
            BOOST_TEST(p->y == 3.0);
        }
        if (auto* const s = e1->get_component<Speed>(); nullptr != s) {
            BOOST_TEST(s->x == 4.0);
            BOOST_TEST(s->y == 5.0);
        }
        if (auto* const p = e2->get_component<Position>(); nullptr != p) {
            BOOST_TEST(p->x == 6.0);
            BOOST_TEST(p->y == 7.0);
        }
        if (auto* const p = e3->get_component<Position>(); nullptr != p) {
            BOOST_TEST(p->x == 8.0);
            BOOST_TEST(p->y == 9.0);
        }
        if (auto* const s = e3->get_component<Speed>(); nullptr != s) {
            BOOST_TEST(s->x == 10.0);
            BOOST_TEST(s->y == 11.0);
        }
        if (auto* const s = e4->get_component<Speed>(); nullptr != s) {
            BOOST_TEST(s->x == 12.0);
            BOOST_TEST(s->y == 13.0);
        }
    };

    const auto check_systems = [&] {
        std::atomic<int> e1s = 0, e2s = 0, e3s = 0, e4s = 0, e5s = 0;

        w.parallel_system<All<Position>>([&](auto* const ent, const Position* const p, const unsigned int) {
            if (e1 == ent && 2.0 == p->x && 3.0 == p->y) {
                ++e1s;
            } else if (e2 == ent && 6.0 == p->x && 7.0 == p->y) {
                ++e2s;
            } else if (e3 == ent && 8.0 == p->x && 9.0 == p->y) {
                ++e3s;
            } else {
                BOOST_TEST(false);
            } });

        BOOST_TEST((e1s == 1 && e2s == 1 && e3s == 1));
        e1s = e2s = e3s = 0;

        w.parallel_system<Not<Speed>>([&](auto* const ent, Speed* const, const unsigned int) {
            if (ent == e2)
                ++e2s;
            else if (ent == e5)
                ++e5s;
            else
                BOOST_TEST(false); });

        BOOST_TEST((e2s == 1 && e5s == 1));
        e2s = e5s = 0;

        w.parallel_system<Not<Position>>([&](auto* const ent, Position* const, const unsigned int) {
            if (ent == e4)
                ++e4s;
            else if (ent == e5)
                ++e5s;
            else
                BOOST_TEST(false); });

        BOOST_TEST((e4s == 1 && e5s == 1));
        e4s = e5s = 0;

        w.parallel_system<Speed>([&](auto* const ent, const Speed* const s, const unsigned int) {
            if (e1 == ent && 4.0 == s->x && 5.0 == s->y) {
                ++e1s;
            } else if (e3 == ent && 10.0 == s->x && 11.0 == s->y) {
                ++e3s;
            } else if (e4 == ent && 12.0 == s->x && 13.0 == s->y) {
                ++e4s;
            } else {
                BOOST_TEST(false);
            } });

        BOOST_TEST((e1s == 1 && e3s == 1 && e4s == 1));
        e1s = e3s = e4s = 0;

        w.parallel_system<All<Speed, Position>>([&](auto* const ent, const Speed* const s, const Position* const p, const unsigned int) {
            if (e1 == ent && 2 == std::lround(p->x) && 3 == std::lround(p->y) && 4 == std::lround(s->x) && 5 == std::lround(s->y)) {
                ++e1s;
            } else if (e3 == ent && 8 == std::lround(p->x) && 9 == std::lround(p->y) && 10 == std::lround(s->x) && 11 == std::lround(s->y)) {
                ++e3s;
            } else {
                BOOST_TEST(false);
            } });

        BOOST_TEST((e1s == 1 && e3s == 1));
        e1s = e3s = 0;

        w.parallel_system<All<Not<Speed>, Position>>([&](auto* const ent, const Speed* const, const Position* const p, const unsigned int) {
            if (e2 == ent && 6.0 == p->x && 7.0 == p->y) {
                ++e2s;
            } else {
                BOOST_TEST(false);
            } });

        BOOST_TEST(e2s == 1);
        e2s = 0;

        w.parallel_system<All<Speed, Not<Position>>>([&](auto* const ent, const Speed* const s, Position*, const unsigned int) {
            if (e4 == ent && 12.0 == s->x && 13.0 == s->y) {
                ++e4s;
            } else {
                BOOST_TEST(false);
            } });

        BOOST_TEST(e4s == 1);
        e4s = 0;

        w.parallel_system<Any<All<Speed, Position>, Speed>>([&](auto* const ent, const Speed* const s, Position*, const unsigned int) {
            if (e1 == ent && 4.0 == s->x && 5.0 == s->y) {
                ++e1s;
            } else if (e3 == ent && 10.0 == s->x && 11.0 == s->y) {
                ++e3s;
            } else if (e4 == ent && 12.0 == s->x && 13.0 == s->y) {
                ++e4s;
            } else {
                BOOST_TEST(false);
            } });

        BOOST_TEST((e1s == 1 && e3s == 1 && e4s == 1));
        e1s = e3s = e4s = 0;
    };

    const auto delete_entities = [&] {
        e1 = e2 = e3 = e4 = e5 = EntityPtr {};
        w.update();
    };

    const auto end_of_step = [&] {
        w.update();
        check_components();
        check_systems();
        delete_entities();
    };

    e1 = Entity::construct("1", nullptr);
    e1->add_component(Object::construct<Position>(2.0, 3.0));
    e1->add_component(Object::construct<Speed>(4.0, 5.0));

    e2 = Entity::construct("2", e1.get());
    e2->add_component(Object::construct<Position>(6.0, 7.0));

    e3 = Entity::construct("3", e2.get());
    e3->add_component(Object::construct<Speed>(10.0, 11.0));
    e3->add_component(Object::construct<Position>(8.0, 9.));

    e4 = Entity::construct("4", e3.get());
    e4->add_component(Object::construct<Speed>(12.0, 13.0));

    e5 = Entity::construct("5", e4.get());

    e1->add_to_world();

    end_of_step();

    e1 = Entity::construct("1", nullptr);
    e2 = Entity::construct("2", e1.get());
    e3 = Entity::construct("3", e2.get());
    e4 = Entity::construct("4", e3.get());
    e5 = Entity::construct("5", e4.get());

    e1->add_to_world();

    w.update();

    w.delayed_pullout_entity(std::move(e1), EndCaller<EntityPtr>([&](EntityPtr&& e) {
        e1 = std::move(e);
        e1->add_component(Object::construct<Position>(2.0, 3.0));
        e1->add_component(Object::construct<Speed>(4.0, 5.0));
    }));

    w.update();

    w.delayed_pullout_entity(std::move(e2), EndCaller<EntityPtr>([&](EntityPtr&& e) {
        e2 = std::move(e);
        e2->add_component(Object::construct<Position>(6.0, 7.0));
    }));

    w.update();

    w.delayed_pullout_entity(std::move(e3), EndCaller<EntityPtr>([&](EntityPtr&& e) {
        e3 = std::move(e);
        e3->add_component(Object::construct<Speed>(10.0, 11.0));
        e3->add_component(Object::construct<Position>(8.0, 9.));
    }));

    w.update();

    w.delayed_pullout_entity(std::move(e4), EndCaller<EntityPtr>([&](EntityPtr&& e) {
        e4 = std::move(e);
        e4->add_component(Object::construct<Speed>(12.0, 13.0));
    }));

    w.update();

    e1->add_to_world();

    w.update();

    end_of_step();

    e1 = Entity::construct("1", nullptr);
    e1->add_component(Object::construct<Position>(2.0, 3.0));
    e1->add_component(Object::construct<Speed>(4.0, 5.0));

    e2 = Entity::construct("2", e1.get());
    e2->add_component(Object::construct<Position>(6.0, 7.0));

    e3 = Entity::construct("3", e2.get());
    e3->add_component(Object::construct<Speed>(10.0, 11.0));
    e3->add_component(Object::construct<Position>(8.0, 9.));

    e4 = Entity::construct("4", e3.get());
    e4->add_component(Object::construct<Speed>(12.0, 13.0));

    e5 = Entity::construct("5", e4.get());

    e1->add_to_world();

    w.update();

    std::atomic<int> progressed = 0;

    w.delayed_pullout_entity(std::move(e2), EndCaller<EntityPtr>([&](EntityPtr&& e) {
        e2 = std::move(e);
        e2->add_component(Object::construct<Speed>(-1.0, -1.0));
        ++progressed;
    }));

    w.delayed_pullout_entity(std::move(e4), EndCaller<EntityPtr>([&](EntityPtr&& e) {
        e4 = std::move(e);
        e4->add_component(Object::construct<Position>(-1.0, -1.0));
        ++progressed;
    }));

    w.delayed_pullout_entity(std::move(e5), EndCaller<EntityPtr>([&](EntityPtr&& e) {
        e5 = std::move(e);
        e5->add_component(Object::construct<Position>(-1.0, -1.0));
        e5->add_component(Object::construct<Speed>(-1.0, -1.0));
        ++progressed;
    }));

    do {
        w.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    } while (progressed < 3);

    e1->add_to_world();

    w.update();

    progressed = 0;

    w.delayed_pullout_entity(std::move(e2), EndCaller<EntityPtr>([&](EntityPtr&& e) {
        e2 = std::move(e);
        e2->remove_component(Speed::object_type_index);
        ++progressed;
    }));

    w.delayed_pullout_entity(std::move(e4), EndCaller<EntityPtr>([&](EntityPtr&& e) {
        e4 = std::move(e);
        e4->remove_component(Position::object_type_index);
        ++progressed;
    }));

    w.delayed_pullout_entity(std::move(e5), EndCaller<EntityPtr>([&](EntityPtr&& e) {
        e5 = std::move(e);
        e5->remove_component(Position::object_type_index);
        e5->remove_component(Speed::object_type_index);
        ++progressed;
    }));

    do {
        w.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    } while (progressed < 3);

    e1->add_to_world();

    w.update();

    end_of_step();

    w.clear();
}