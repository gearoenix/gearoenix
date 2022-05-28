#include "gx-rnd-rfl-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../math/gx-math-aabb.hpp"
#include "../../platform/stream/gx-plt-stm-path.hpp"
#include "../../platform/stream/gx-plt-stm-stream.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-cube.hpp"
#include "gx-rnd-rfl-builder.hpp"
#include "gx-rnd-rfl-runtime.hpp"

gearoenix::render::reflection::Manager::Manager(engine::Engine& e) noexcept
    : e(e)
{
}

gearoenix::render::reflection::Manager::~Manager() noexcept = default;

std::shared_ptr<gearoenix::render::reflection::Builder> gearoenix::render::reflection::Manager::build_baked(
    const std::string& name,
    const platform::stream::Path& path,
    const core::sync::EndCallerIgnored& c) noexcept
{
    const auto stream = platform::stream::Stream::open(path, e.get_platform_application());
    GX_ASSERT(nullptr != stream);
    math::Aabb3<double> include_box;
    include_box.read(*stream);
    const auto irradiance = std::dynamic_pointer_cast<texture::TextureCube>(e.get_texture_manager()->read_gx3d(stream, c));
    GX_ASSERT(irradiance != nullptr);
    const auto radiance = std::dynamic_pointer_cast<texture::TextureCube>(e.get_texture_manager()->read_gx3d(stream, c));
    GX_ASSERT(radiance != nullptr);
    return build_baked(name, irradiance, radiance, include_box, c);
}

void gearoenix::render::reflection::Manager::update() noexcept
{
    e.get_world()->parallel_system<Runtime>([this](
                                                const core::ecs::Entity::id_t /*entity_id*/,
                                                Runtime& runtime_probe,
                                                const unsigned int /*kernel_index*/) {
        if (runtime_probe.enabled) {
            runtime_probe.update_state();
        }
    });
}