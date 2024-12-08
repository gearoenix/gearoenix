#include "gx-rnd-rfl-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-cube.hpp"
#include "gx-rnd-rfl-baked.hpp"
#include "gx-rnd-rfl-builder.hpp"
#include "gx-rnd-rfl-runtime.hpp"

gearoenix::render::reflection::Manager::Manager(engine::Engine& e)
    : e(e)
{
    core::ecs::ComponentType::add<Probe>();
    core::ecs::ComponentType::add<Runtime>();
    core::ecs::ComponentType::add<Baked>();
}

gearoenix::render::reflection::Manager::~Manager() = default;

void gearoenix::render::reflection::Manager::build_baked(
    const std::string& name,
    physics::Transformation* parent_transform,
    const platform::stream::Path& path,
    core::job::EndCallerShared<Builder>&& c,
    core::job::EndCaller<>&& entity_end_callback)
{

    const auto stream = platform::stream::Stream::open(path, e.get_platform_application());
    GX_ASSERT(nullptr != stream);

    struct Values {
        Manager* const manager;
        std::string name;
        core::job::EndCallerShared<Builder> c;
        core::job::EndCaller<> entity_end_callback;
        physics::Transformation* const parent_transform;
        math::Aabb3<double> include_box;
        std::shared_ptr<texture::TextureCube> irradiance;
        std::shared_ptr<texture::TextureCube> radiance;

        ~Values()
        {
            GX_ASSERT(irradiance != nullptr);
            GX_ASSERT(radiance != nullptr);

            c.set_return(manager->build_baked(name, parent_transform, std::move(irradiance), std::move(radiance), include_box, std::move(entity_end_callback)));
        }
    };

    auto values = std::make_shared<Values>(this, name, std::move(c), std::move(entity_end_callback), parent_transform);
    values->include_box.read(*stream);

    e.get_texture_manager()->read_gx3d(*stream, core::job::EndCallerShared<texture::Texture>([values](std::shared_ptr<texture::Texture>&& t) {
        values->irradiance = std::dynamic_pointer_cast<texture::TextureCube>(std::move(t));
    }));

    e.get_texture_manager()->read_gx3d(*stream, core::job::EndCallerShared<texture::Texture>([values](std::shared_ptr<texture::Texture>&& t) {
        values->radiance = std::dynamic_pointer_cast<texture::TextureCube>(std::move(t));
    }));
}

void gearoenix::render::reflection::Manager::update()
{
    core::ecs::World::get()->parallel_system<Runtime>(
        [](
            const core::ecs::entity_id_t /*entity_id*/,
            Runtime* const runtime_probe,
            const unsigned int /*kernel_index*/) {
            if (runtime_probe->get_enabled()) {
                runtime_probe->update_state();
            }
        });
}