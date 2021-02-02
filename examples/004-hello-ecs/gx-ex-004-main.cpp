#define GX_PLATFORM_LOG_STD_OUT_ENABLED
#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/platform/gx-plt-main-entry.hpp>

struct Position final : public gearoenix::core::ecs::Component {
    double x;
    double y;

    Position(const double x, const double y) noexcept
        : gearoenix::core::ecs::Component(this)
        , x(x)
        , y(y)
    {
    }

    Position(Position&&) noexcept = default;
    Position(const Position&) = delete;
    Position& operator=(Position&&) = delete;
    Position& operator=(const Position&) = delete;
};

struct Speed final : public gearoenix::core::ecs::Component {
    double x;
    double y;

    Speed(const double x, const double y) noexcept
        : gearoenix::core::ecs::Component(this)
        , x(x)
        , y(y)
    {
    }

    Speed(Speed&&) noexcept = default;
    Speed(const Speed&) = delete;
    Speed& operator=(Speed&&) = delete;
    Speed& operator=(const Speed&) = delete;
};

void start()
{
    using namespace gearoenix::core::ecs;
    World w;
    const auto entity = w.create_entity(Position { 2.0, 3.0 }, Speed { 4.0, 5.0 });
    (void)w.create_entity(Position { 6.0, 7.0 });
    (void)w.create_entity(Speed { 10.0, 11.0 }, Position { 8.0, 9.0 });
    (void)w.create_entity(Speed { 12.0, 13.0 });
    w.parallel_system((std::function<void(Entity::id_t, Position&)>)[&](Entity::id_t ent, Position & p) {
        GX_LOG_D("Entity: " << ent << ", Position { " << p.x << ", " << p.y << " }") });
    w.parallel_system((std::function<void(Entity::id_t, Not<Speed>&)>)[&](Entity::id_t ent, Not<Speed>&) {
        GX_LOG_D("Entity: " << ent << ", Without Speed") });
    w.parallel_system((std::function<void(Entity::id_t, Not<Position>&)>)[&](Entity::id_t ent, Not<Position>&) {
        GX_LOG_D("Entity: " << ent << ", Without Position") });
    w.parallel_system((std::function<void(Entity::id_t, Speed&)>)[&](Entity::id_t ent, Speed & s) {
        GX_LOG_D("Entity: " << ent << ", Speed {" << s.x << ", " << s.y << "}") });
    w.parallel_system((std::function<void(Entity::id_t, Speed&, Position&)>)[&](Entity::id_t ent, Speed & s, Position & p) {
        GX_LOG_D("Entity: " << ent << ", Speed {" << s.x << ", " << s.y << "}, Position {" << p.x << "," << p.y << "}") });

    // It is not a good practice to fetch components by entity id, it is better to use a system, however this
    // functionality is provided, for some use-cases.
    auto& p = w.get_component<Position>(entity);
    auto& s = w.get_component<Speed>(entity);
    GX_LOG_D("Position { " << p.x << ", " << p.y << " }")
    GX_LOG_D("Speed { " << s.x << ", " << s.y << " }")
    GX_LOG_D("Ended!");
}

GX_MAIN_ENTRY(start())