#include "gx-rnd-sky-manager.hpp"
#include "gx-rnd-sky-cube.hpp"
#include "gx-rnd-sky-equirectangular.hpp"

gearoenix::render::skybox::Manager::Manager(std::unique_ptr<platform::stream::Stream> s, engine::Engine* const e) noexcept
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
        id, [id, c, this](std::string name) noexcept {
            GXLOGD("Going to load Skybox: " << id)
            platform::stream::Stream* const f = cache.get_file();
            const auto t = f->read<Type>();
            const core::sync::EndCaller<core::sync::EndCallerIgnore> call([c] {});
            switch (t) {
            case Type::Cube:
            case Type::Equirectangular:
                return std::shared_ptr<Skybox>(new Cube(id, std::move(name), f, e, call));
            default:
                GX_UNEXPECTED
            }
        });
    c.set_data(s);
    return s;
}
