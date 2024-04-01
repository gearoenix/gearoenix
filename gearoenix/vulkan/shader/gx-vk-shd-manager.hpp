#ifndef GEAROENIX_VULKAN_SHADER_MANAGER_HPP
#define GEAROENIX_VULKAN_SHADER_MANAGER_HPP
#include "../../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include <map>
#include <memory>
#include <string>

namespace gearoenix::vulkan::engine {
struct Engine;
}

namespace gearoenix::vulkan::shader {
struct Module;
struct Manager final {
private:
    std::map<std::string, std::weak_ptr<Module>> modules;
    const engine::Engine& e;

public:
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;
    explicit Manager(const engine::Engine& e);
    ~Manager();
    [[nodiscard]] std::shared_ptr<Module> get(const std::string& shader_name);
};
}

#endif
#endif