#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../render/material/gx-rnd-mat-manager.hpp"
#include "../shader/glsl/gx-vk-shd-common.glslh"
#include "../descriptor/gx-vk-des-uniform-holder.hpp"

namespace gearoenix::vulkan::buffer {
struct Uniform;
}

namespace gearoenix::vulkan::material {
/// Vulkan backend's material manager.
///
/// For a new material addition to Vulkan backed:
///   1- Add the corresponding shader in the shader/glsl
///   2- If the material has a different data structure than GxShaderDataMaterial, it has a field that is not inside the current GxShaderDataMaterial
///      You can reinterpret_cast GxShaderDataMaterial* to the new data structure and if GxShaderDataMaterial's size is not enough, you can add additional padding to it.
///      But it is very unlikely because GxShaderDataMaterial is already very big, and it should cover all cases.
///   3- Implement the corresponding pipeline in the pipeline module and manager as a reference, you can look at the pipeline that has been created for the pbr material
///   5- If the shadow pass has a different behaviour not covered in other shadow shaders and pipelines, add a new shadow shader and pipeline based on the need.
///      Bear in mind, your material most likely must be able to have enough commonality with other already implemented shadows.
///      It should be noted that for some material shadowing is not defined.
///   4- Implement corresponding code in this material module like gearoenix::vulkan::material::Pbr and add the corresponding construct function here.
///   5- Most of the time OpenGL backend has the most updated implementation of the materials, so look there for further detail of the implementation of the shader and cpp codes
///      and apply those logics in here based on the current material implementations (like Pbr).
struct Manager final : render::material::Manager, core::Singleton<Manager>, descriptor::UniformHolder<GxShaderDataMaterial> {
    Manager();
    ~Manager() override;
    void construct_pbr(std::string&& name, core::job::EndCallerShared<render::material::Pbr>&& c) override;
    void construct_unlit(std::string&& name, core::job::EndCallerShared<render::material::Unlit>&& c) override;
    void construct_sprite(std::string&& name, core::job::EndCallerShared<render::material::Sprite>&& c) override;
};
}
#endif