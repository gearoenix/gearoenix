#include "rnd-sky-manager.hpp"
#include "rnd-sky-equirectangular.hpp"

gearoenix::render::skybox::Manager::Manager(std::unique_ptr<system::stream::Stream> s, engine::Engine* const e) noexcept
    : e(e)
    , cache(std::move(s))
{
}

gearoenix::render::skybox::Manager::~Manager() noexcept = default;

std::shared_ptr<gearoenix::render::skybox::Skybox> gearoenix::render::skybox::Manager::get_gx3d(
    const core::Id id,
    core::sync::EndCaller<Skybox>& c) noexcept
{
    std::shared_ptr<Skybox> s = cache.get<Skybox>(
        id, [id, c, this]() noexcept -> std::shared_ptr<Skybox> {
            GXLOGD("Going to load Skybox: " << id)
            system::stream::Stream* const f = cache.get_file();
            const auto t = f->read<Type>();
            const core::sync::EndCaller<core::sync::EndCallerIgnore> call([c] {});
            switch (t) {
            case Type::Cube:
            case Type::Equirectangular:
                GXUNIMPLEMENTED
            default:
                GXUNEXPECTED
            }
        });
    c.set_data(s);
    return s;
}
