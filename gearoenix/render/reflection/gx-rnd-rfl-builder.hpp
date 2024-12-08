#pragma once
#include "../../core/ecs/gx-cr-ecs-types.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include "../../core/macro/gx-cr-mcr-getter-setter.hpp"
#include "../../math/gx-math-aabb.hpp"
#include <memory>
#include <string>

namespace gearoenix::core::ecs {
struct EntitySharedBuilder;
}

namespace gearoenix::physics {
struct Transformation;
}

namespace gearoenix::render::camera {
struct Builder;
}

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::texture {
struct TextureCube;
}

namespace gearoenix::render::reflection {
struct Runtime;
struct Builder {
    typedef std::array<std::shared_ptr<camera::Builder>, 6> CameraBuilders;

    GX_GET_REFC_PRT(std::shared_ptr<core::ecs::EntitySharedBuilder>, entity_builder);
    GX_GET_REFC_PRT(std::shared_ptr<physics::Transformation>, transformation);
    GX_GET_CREF_PRT(CameraBuilders, faces_camera_builders);

    Builder(
        engine::Engine& e, std::string&& name,
        physics::Transformation* parent_transform,
        core::job::EndCaller<>&& end_callback);

public:
    virtual ~Builder();
    Builder(Builder&&) = delete;
    Builder(const Builder&) = delete;
    Builder& operator=(Builder&&) = delete;
    Builder& operator=(const Builder&) = delete;
    [[nodiscard]] const Runtime& get_runtime() const;
    [[nodiscard]] Runtime& get_runtime();
    void set_camera_builder(std::shared_ptr<camera::Builder>&& builder, std::uint64_t face_index);
    [[nodiscard]] core::ecs::entity_id_t get_id() const;
};
}