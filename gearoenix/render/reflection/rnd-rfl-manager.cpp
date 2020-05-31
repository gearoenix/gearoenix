#include "rnd-rfl-manager.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "rnd-rfl-baked.hpp"
#include "rnd-rfl-runtime.hpp"

gearoenix::render::reflection::Manager::Manager(std::unique_ptr<system::stream::Stream> s, engine::Engine* const e) noexcept
    : e(e)
    , cache(std::move(s))
{
}

gearoenix::render::reflection::Manager::~Manager() noexcept = default;

std::shared_ptr<gearoenix::render::reflection::Reflection> gearoenix::render::reflection::Manager::get_gx3d(const core::Id id, core::sync::EndCaller<Reflection>& call) noexcept
{
    const std::shared_ptr<Reflection> data = cache.get<Reflection>(
        id, [this, id, call]() noexcept -> std::shared_ptr<Reflection> {
            system::stream::Stream* const file = cache.get_file();
            const auto t = file->read<Type>();
            switch (t) {
            case Type::Runtime:
                GX_UNIMPLEMENTED
            case Type::Baked:
                GX_UNIMPLEMENTED
            }
        });
    call.set_data(data);
    return data;
}
