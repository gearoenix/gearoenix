#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include <boost/container/flat_map.hpp>
#include <memory>
#include <mutex>
#include <typeindex>

namespace gearoenix::gl::shader {
struct ShaderCombination;
struct Manager final {
private:
    std::mutex shaders_lock;
    /// TODO: use static-allocator in the core
    boost::container::flat_map<std::type_index, std::weak_ptr<ShaderCombination>> shaders;

public:
    Manager();
    ~Manager();
    [[nodiscard]] static Manager& get();

    template <typename SC>
    [[nodiscard]] std::shared_ptr<SC> get()
    {
        const auto type_index = std::type_index(typeid(SC));
        const std::lock_guard _lg(shaders_lock);
        if (auto search = shaders.find(type_index); shaders.end() != search) {
            if (auto s = search->second.lock()) {
                return std::dynamic_pointer_cast<SC>(s);
            }
            auto s = std::shared_ptr<SC>(new SC());
            search->second = s;
            return s;
        }
        auto s = std::shared_ptr<SC>(new SC());
        shaders.emplace(type_index, s);
        return s;
    }
};
}

#endif