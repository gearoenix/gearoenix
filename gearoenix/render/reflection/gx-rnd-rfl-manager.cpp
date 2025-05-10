#include "gx-rnd-rfl-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-cube.hpp"
#include "gx-rnd-rfl-baked.hpp"
#include "gx-rnd-rfl-runtime.hpp"

gearoenix::render::reflection::Manager::Manager()
    : Singleton(this)
{
    core::ecs::ComponentType::add<Probe>();
    core::ecs::ComponentType::add<Runtime>();
    core::ecs::ComponentType::add<Baked>();
}

gearoenix::render::reflection::Manager::~Manager() = default;

void gearoenix::render::reflection::Manager::build_baked(
    std::string&& name,
    core::ecs::Entity* const parent,
    const platform::stream::Path& path,
    core::job::EndCaller<core::ecs::EntityPtr>&& entity_callback)
{
    const auto stream = platform::stream::Stream::open(path);
    GX_ASSERT(nullptr != stream);

    struct Values {
        Manager* const manager;
        std::string name;
        core::job::EndCaller<core::ecs::EntityPtr> entity_callback;
        core::ecs::Entity* const parent;
        math::Aabb3<double> include_box;
        std::shared_ptr<texture::TextureCube> irradiance;
        std::shared_ptr<texture::TextureCube> radiance;

        ~Values()
        {
            GX_ASSERT(irradiance != nullptr);
            GX_ASSERT(radiance != nullptr);

            entity_callback.set_return(manager->build_baked(std::move(name), parent, std::move(irradiance), std::move(radiance), include_box));
        }
    };

    auto values = std::make_shared<Values>(this, std::move(name), std::move(entity_callback), parent);
    values->include_box.read(*stream);

    GX_TODO; // I have to read it from the object streamer

    texture::Manager::get().read_gx3d(*stream, core::job::EndCallerShared<texture::Texture>([values](std::shared_ptr<texture::Texture>&& t) {
        values->irradiance = std::dynamic_pointer_cast<texture::TextureCube>(std::move(t));
    }));

    texture::Manager::get().read_gx3d(*stream, core::job::EndCallerShared<texture::Texture>([values](std::shared_ptr<texture::Texture>&& t) {
        values->radiance = std::dynamic_pointer_cast<texture::TextureCube>(std::move(t));
    }));
}

void gearoenix::render::reflection::Manager::update()
{
    core::ecs::World::get().parallel_system<Runtime>(
        [](
            const auto /*entity_id*/,
            auto* const runtime_probe,
            const unsigned int /*kernel_index*/) {
            if (runtime_probe->get_enabled()) {
                runtime_probe->update_state();
            }
        });
}