#ifndef GEAROENIX_CORE_ECS_WORLD_TEST_HPP
#define GEAROENIX_CORE_ECS_WORLD_TEST_HPP

#include "../../platform/gx-plt-log.hpp"
#include "gx-cr-ecs-world.hpp"

struct Position final : public gearoenix::core::ecs::Component {
    double x;
    double y;

    Position(const double x, const double y) noexcept
        : x(x)
        , y(y)
    {
    }

    Position(const Position&) noexcept = delete;
    Position& operator=(const Position&) noexcept = delete;

    Position(Position&&) noexcept = default;

    Position& operator=(const Position&& o) noexcept = delete;
};

struct Speed final : public gearoenix::core::ecs::Component {
    double x;
    double y;

    Speed(const double x, const double y) noexcept
        : x(x)
        , y(y)
    {
    }
};

BOOST_AUTO_TEST_CASE(gearoenix_core_ecs_world)
{
    using namespace gearoenix::core::ecs;
    World w;
    const auto entity = w.create_entity(Position { 2.0, 3.0 }, Speed { 4.0, 5.0 });
    (void)w.create_entity(Position { 6.0, 7.0 });
    (void)w.create_entity(Speed { 10.0, 11.0 }, Position { 8.0, 9.0 });
    (void)w.create_entity(Speed { 12.0, 13.0 });
    w.parallel_system((std::function<void(entity_id_t, Position&)>)[&](entity_id_t ent, Position & p) {
        GX_LOG_I("Entity: " << ent << ", Position { " << p.x << ", " << p.y << " }") });
    w.parallel_system((std::function<void(entity_id_t, Not<Speed>&)>)[&](entity_id_t ent, Not<Speed>&) {
        GX_LOG_I("Entity: " << ent << ", Without Speed") });
    w.parallel_system((std::function<void(entity_id_t, Not<Position>&)>)[&](entity_id_t ent, Not<Position>&) {
        GX_LOG_I("Entity: " << ent << ", Without Position") });
    w.parallel_system((std::function<void(entity_id_t, Speed&)>)[&](entity_id_t ent, Speed & s) {
        GX_LOG_I("Entity: " << ent << ", Speed {" << s.x << ", " << s.y << "}") });
    w.parallel_system((std::function<void(entity_id_t, Speed&, Position&)>)[&](entity_id_t ent, Speed & s, Position & p) {
        GX_LOG_I("Entity: " << ent << ", Speed {" << s.x << ", " << s.y << "}, Position {" << p.x << "," << p.y << "}") });

    auto& p = w.get_component<Position>(entity);
    auto& s = w.get_component<Speed>(entity);
    BOOST_TEST(p.x == 2.0);
    BOOST_TEST(p.y == 3.0);
    BOOST_TEST(s.x == 4.0);
    BOOST_TEST(s.y == 5.0);
}

#endif