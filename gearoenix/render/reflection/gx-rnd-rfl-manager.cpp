#include "gx-rnd-rfl-manager.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-texture-cube.hpp"
#include "gx-rnd-rfl-builder.hpp"
#include "gx-rnd-rfl-runtime.hpp"

gearoenix::render::reflection::Manager::Manager(engine::Engine& e)
    : e(e)
{
}

gearoenix::render::reflection::Manager::~Manager() = default;

void gearoenix::render::reflection::Manager::build_baked(
    const std::string& name,
    const platform::stream::Path& path,
    core::job::EndCallerShared<Builder>&& c,
    core::job::EndCaller<>&& entity_end_callback) const
{

    const auto stream = platform::stream::Stream::open(path, e.get_platform_application());
    GX_ASSERT(nullptr != stream);

    struct Values {
        core::job::EndCallerShared<Builder> c;
        core::job::EndCaller<>&& entity_end_callback;
        math::Aabb3<double> include_box;
        std::shared_ptr<texture::TextureCube> irradiance;
        std::shared_ptr<texture::TextureCube> radiance;

        ~Values()
        {
            GX_ASSERT(irradiance != nullptr);
            GX_ASSERT(radiance != nullptr);

            c.set_return(build_baked(name, std::move(irradiance), std::move(radiance), include_box, std::move(entity_end_callback)));
        }
    };

    auto values = std::make_shared<Values>(std::move(c), std::move(entity_end_callback));
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
    e.get_world()->parallel_system<Runtime>(
        [this](
            const core::ecs::entity_id_t /*entity_id*/,
            Runtime* const runtime_probe,
            const unsigned int /*kernel_index*/) {
            if (runtime_probe->get_enabled()) {
                runtime_probe->update_state();
            }
        });
}