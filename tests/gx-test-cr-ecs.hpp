#ifndef GEAROENIX_TEST_CORE_ECS_HPP
#define GEAROENIX_TEST_CORE_ECS_HPP

#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>

struct Position {
    double x;
    double y;
};

struct Speed {
    double x;
    double y;
};

BOOST_AUTO_TEST_CASE(gearoenix_core_ecs_test)
{
    using namespace gearoenix::core::ecs;
    World w;
    const auto entity = w.create_entity(Position { 2.0, 3.0 }, Speed { 4.0, 5.0 });
    (void)w.create_entity(Position { 6.0, 7.0 });
    (void)w.create_entity(Speed { 10.0, 11.0 }, Position { 8.0, 9.0 });
    (void)w.create_entity(Speed { 12.0, 13.0 });
    w.parallel_system((std::function<void(entity_id_t, Position&)>)[&](entity_id_t ent, Position & p) {
        BOOST_TEST_MESSAGE("Entity: " << ent << ", Position { " << p.x << ", " << p.y << " }");
    });
    w.parallel_system((std::function<void(entity_id_t, Not<Speed>&)>)[&](entity_id_t ent, Not<Speed>&) {
        BOOST_TEST_MESSAGE("Entity: " << ent << ", Without Speed");
    });
    w.parallel_system((std::function<void(entity_id_t, Not<Position>&)>)[&](entity_id_t ent, Not<Position>&) {
        BOOST_TEST_MESSAGE("Entity: " << ent << ", Without Position");
    });
    w.parallel_system((std::function<void(entity_id_t, Speed&)>)[&](entity_id_t ent, Speed & s) {
        BOOST_TEST_MESSAGE("Entity: " << ent << ", Speed {" << s.x << ", " << s.y << "}");
    });
    w.parallel_system((std::function<void(entity_id_t, Speed&, Position&)>)[&](entity_id_t ent, Speed & s, Position & p) {
        BOOST_TEST_MESSAGE("Entity: " << ent << ", Speed {" << s.x << ", " << s.y << "}, Position {" << p.x << "," << p.y << "}");
    });

    auto& p = w.get_component<Position>(entity);
    auto& s = w.get_component<Speed>(entity);
    BOOST_TEST(p.x == 2.0);
    BOOST_TEST(p.y == 3.0);
    BOOST_TEST(s.x == 4.0);
    BOOST_TEST(s.y == 5.0);
}

#endif