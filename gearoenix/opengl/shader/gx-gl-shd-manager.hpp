#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include <boost/container/flat_map.hpp>
#include <memory>
#include <mutex>
#include <typeindex>

namespace gearoenix::gl::shader {
struct ShaderCombination;
    struct Shader;
struct Manager final {
private:
    std::mutex combinations_lock;
    boost::container::flat_map<std::type_index, std::weak_ptr<ShaderCombination>> combinations;
    std::mutex shaders_lock;
    boost::container::flat_map<std::type_index, std::weak_ptr<Shader>> shaders;

public:
    Manager();
    ~Manager();
    [[nodiscard]] static Manager& get();

    template <typename SC>
    [[nodiscard]] std::shared_ptr<SC> get_combiner()
    {
        const auto type_index = std::type_index(typeid(SC));
        const std::lock_guard _lg(combinations_lock);
        if (const auto search = combinations.find(type_index); combinations.end() != search) {
            if (const auto s = search->second.lock()) {
                return std::dynamic_pointer_cast<SC>(s);
            }
            auto s = std::shared_ptr<SC>(new SC());
            search->second = s;
            return s;
        }
        auto s = std::shared_ptr<SC>(new SC());
        combinations.emplace(type_index, s);
        return s;
    }

    template <typename S>
    [[nodiscard]] std::shared_ptr<S> get_shader()
    {
        const auto type_index = std::type_index(typeid(S));
        const std::lock_guard _lg(shaders_lock);
        if (const auto search = shaders.find(type_index); shaders.end() != search) {
            if (const auto s = search->second.lock()) {
                return std::dynamic_pointer_cast<S>(s);
            }
            auto s = std::shared_ptr<S>(new S());
            search->second = s;
            return s;
        }
        auto s = std::shared_ptr<S>(new S());
        shaders.emplace(type_index, s);
        return s;
    }
};
}

#endif