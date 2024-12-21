#pragma once
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include <memory>
#include <vector>

namespace gearoenix::platform::stream {
struct Path;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::scene {
struct Builder;
}

namespace gearoenix::render::gltf {
void load(
    const platform::stream::Path& file,
    const core::job::EndCaller<std::vector<std::shared_ptr<scene::Builder>>>& scenes_end_callback,
    const core::job::EndCaller<>& entity_end_callback);
}