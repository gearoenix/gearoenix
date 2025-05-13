#pragma once
#include "../../core/ecs/gx-cr-ecs-entity-ptr.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include <memory>
#include <vector>

namespace gearoenix::platform::stream {
struct Path;
}

namespace gearoenix::render::gltf {
void load(
    const platform::stream::Path& file,
    core::job::EndCaller<std::vector<core::ecs::EntityPtr>>&& scene_entities_end_callback);
}