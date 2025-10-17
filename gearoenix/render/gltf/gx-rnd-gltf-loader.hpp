#pragma once
#include "../../core/ecs/gx-cr-ecs-entity-ptr.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include <vector>

namespace gearoenix::platform::stream {
struct Path;
struct Stream;
}

namespace gearoenix::render::gltf {
void load(
    const platform::stream::Path& file,
    core::job::EndCaller<std::vector<core::ecs::EntityPtr>>&& scene_entities_end_callback);

void load(
    std::shared_ptr<platform::stream::Stream>&& stream,
    core::job::EndCaller<std::vector<core::ecs::EntityPtr>>&& scene_entities_end_callback);
}