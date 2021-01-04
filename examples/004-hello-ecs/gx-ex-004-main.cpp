#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/platform/gx-plt-log.hpp>
#include <gearoenix/platform/gx-plt-main-entry.hpp>

struct Position {
    double x;
    double y;
};

struct Speed {
    double x;
    double y;
};

void start()
{
    gearoenix::core::ecs::World w;
    const auto entity = w.create_entity(Position { 2.0, 3.0 }, Speed { 4.0, 5.0 });
    (void)w.create_entity(Position { 6.0, 7.0 });
    (void)w.create_entity(Position { 8.0, 9.0 }, Speed { 10.0, 11.0 });
    auto& p = w.get_component<Position>(entity);
    auto& s = w.get_component<Speed>(entity);
    std::function<void(
        gearoenix::core::ecs::entity_id_t,
        Position&)>
        f1 = [&](
                 gearoenix::core::ecs::entity_id_t ent,
                 Position& p) {
            GX_LOG_D("Entity: " << ent << ", Position { " << p.x << ", " << p.y << " }")
        };
    std::function<void(
        gearoenix::core::ecs::entity_id_t,
        gearoenix::core::ecs::Not<Speed>&)>
        f2 = [&](
                 gearoenix::core::ecs::entity_id_t ent,
                 gearoenix::core::ecs::Not<Speed>&) {
            GX_LOG_D("Entity: " << ent << ", Without Speed")
        };
    w.parallel_system(f1);
    w.parallel_system(f2);
    GX_LOG_D("Position { " << p.x << ", " << p.y << " }")
    GX_LOG_D("Speed { " << s.x << ", " << s.y << " }")
    GX_LOG_D("Ended!");
}

GX_MAIN_ENTRY(start())