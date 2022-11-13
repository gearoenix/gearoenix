#ifndef GEAROENIX_GL_SHADER_MANAGER_HPP
#define GEAROENIX_GL_SHADER_MANAGER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include <boost/container/flat_map.hpp>
#include <memory>
#include <typeindex>

namespace gearoenix::gl {
struct Engine;
}

namespace gearoenix::gl::shader {
struct ShaderCombination;
struct Manager final {
    Engine& e;

private:
    /// TODO: use static-allocator in the core
    boost::container::flat_map<std::type_index, std::weak_ptr<ShaderCombination>> shaders;

public:
    explicit Manager(Engine& e) noexcept;

    template <typename SC>
    [[nodiscard]] std::shared_ptr<SC> get() noexcept
    {
        const auto type_index = std::type_index(typeid(SC));
        if (auto search = shaders.find(type_index); shaders.end() != search) {
            if (auto s = search->second.lock()) {
                return std::dynamic_pointer_cast<SC>(s);
            }
            auto s = std::shared_ptr<SC>(new SC(e));
            search->second = s;
            return s;
        }
        auto s = std::shared_ptr<SC>(new SC(e));
        shaders.emplace(type_index, s);
        return s;
    }
};
}

#endif
#endif