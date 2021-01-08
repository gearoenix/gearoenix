#include "gx-rnd-rfl-manager.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-rfl-baked.hpp"
#include "gx-rnd-rfl-runtime.hpp"

gearoenix::render::reflection::Manager::Manager(std::unique_ptr<platform::stream::Stream> s, engine::Engine* const e) noexcept
    : e(e)
    , cache(std::move(s))
{
}

gearoenix::render::reflection::Manager::~Manager() noexcept = default;

std::shared_ptr<gearoenix::render::reflection::Reflection> gearoenix::render::reflection::Manager::get_gx3d(const core::Id id, core::sync::EndCaller<Reflection>& call) noexcept
{
    auto data = cache.get<Reflection>(
        id, [this, id, call](std::string) noexcept -> std::shared_ptr<Reflection> {
            platform::stream::Stream* const file = cache.get_file();
            const auto t = file->read<Type>();
            switch (t) {
            case Type::Runtime:
                GX_UNIMPLEMENTED
            case Type::Baked:
                GX_UNIMPLEMENTED
            }
            (void)id;
            (void)call;
            GX_UNEXPECTED
        });
    call.set_data(data);
    return data;
}
