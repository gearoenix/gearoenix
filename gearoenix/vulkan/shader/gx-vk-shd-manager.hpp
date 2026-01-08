#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../core/gx-cr-singleton.hpp"

#include <map>
#include <memory>
#include <string>

namespace gearoenix::vulkan::shader {
struct Module;
struct Manager final : core::Singleton<Manager> {
private:
    std::map<std::string, std::weak_ptr<Module>> modules;

public:
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;

    Manager();
    ~Manager() override;
    [[nodiscard]] std::shared_ptr<Module> get(const std::string& shader_name);
};
}
#endif