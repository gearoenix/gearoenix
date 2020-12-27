#include <gearoenix/platform/gx-plt-log.hpp>
#include <gearoenix/platform/gx-plt-main-entry.hpp>
#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>

struct Position { double x; double y; };
struct Speed { double x; double y; };

void start() {
    gearoenix::core::ecs::World w;
    const auto entity = w.create_entity(Position { 2.0, 3.0}, Speed {4.0, 5.0});
    auto& p = w.get_component<Position>(entity);
    auto& s = w.get_component<Speed>(entity);
    GX_LOG_D("Position { " << p.x << ", " << p.y << " }")
    GX_LOG_D("Speed { " << s.x << ", " << s.y << " }")
    GX_LOG_D("Ended!");
}

GX_MAIN_ENTRY(start())