#ifndef GEAROENIX_RENDER_MODEL_MANAGER_HPP
#define GEAROENIX_RENDER_MODEL_MANAGER_HPP
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cache/cr-cache-file.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "rnd-mdl-model.hpp"
#include <memory>

namespace gearoenix::system::stream {
class Stream;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::model {
class Manager {
    GX_GET_CPTR_PRV(engine::Engine, e)
    GX_GET_CREF_PRV(core::cache::File<Model>, cache)

public:
    Manager(std::unique_ptr<system::stream::Stream> s, engine::Engine* e) noexcept;
    ~Manager() noexcept = default;
    [[nodiscard]] std::shared_ptr<Model> get_gx3d(core::Id mid, core::sync::EndCaller<Model>& c) noexcept;
    [[nodiscard]] std::shared_ptr<Model> get_gx3d(const std::string& name, core::sync::EndCaller<Model>& c) noexcept;
    /// T must be derived from Model and have the same constructor that Model has.
    template <typename T>
    [[nodiscard]] typename std::enable_if<std::is_base_of<Model, T>::value, std::shared_ptr<T>>::type
    create(core::sync::EndCaller<T>& c) noexcept;
};
}

template <typename T>
typename std::enable_if<std::is_base_of<gearoenix::render::model::Model, T>::value, std::shared_ptr<T>>::type
gearoenix::render::model::Manager::create(core::sync::EndCaller<T>& c) noexcept
{
    const core::Id id = core::asset::Manager::create_id();
    const core::sync::EndCaller<core::sync::EndCallerIgnore> call([c] {});
    const std::shared_ptr<T> result(new T(id, e, call));
    c.set_data(result);
    cache.get_cacher().get_cacheds()[id] = result;
    return result;
}

#endif