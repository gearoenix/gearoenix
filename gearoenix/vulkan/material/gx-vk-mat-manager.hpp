#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/material/gx-rnd-mat-manager.hpp"

namespace gearoenix::vulkan::buffer {
struct Uniform;
}

namespace gearoenix::vulkan::material {
struct Manager final : render::material::Manager, core::Singleton<Manager> {
    Manager();
    ~Manager() override;
    [[nodiscard]] static const buffer::Uniform& get_uniform_buffer();
    void construct_pbr(std::string&& name, core::job::EndCallerShared<render::material::Pbr>&& c) override;
    void construct_unlit(std::string&& name, core::job::EndCallerShared<render::material::Unlit>&& c) override;
    void construct_sprite(std::string&& name, core::job::EndCallerShared<render::material::Sprite>&& c) override;
};
}
#endif