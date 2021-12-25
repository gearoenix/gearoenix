#pragma once

#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DXR_ENABLED
#include "../render/model/gx-rnd-mdl-builder.hpp"
#include "../render/model/gx-rnd-mdl-manager.hpp"
#include "../render/model/gx-rnd-mdl-model.hpp"
#include "gx-dxr-buffer.hpp"
#include "gx-dxr-build-configuration.hpp"
#include "gx-dxr-loader.hpp"
#include <array>

namespace gearoenix::dxr {
struct Mesh;
struct Model final : public core::ecs::Component {
    std::array<UniformBuffer, GX_DXR_FRAMES_BACKBUFFER_NUMBER> uniforms;
    const std::shared_ptr<Mesh> bound_mesh;

    Model(
        Engine& e,
        std::shared_ptr<Mesh>&& bound_mesh,
        UINT buffer_size,
        const std::string& name) noexcept;
    ~Model() noexcept final;
    Model(Model&&) noexcept;
};

struct ModelBuilder final : public render::model::Builder {
    friend struct ModelManager;

private:
    Engine& e;

    ModelBuilder(
        Engine& e,
        const std::string& name,
        std::shared_ptr<render::mesh::Mesh>&& bound_mesh,
        bool is_transformable) noexcept;

    void set_material(const render::material::Pbr& material_type) noexcept final;

public:
    ~ModelBuilder() noexcept final;
};

struct ModelManager final : public render::model::Manager {
private:
    [[nodiscard]] std::shared_ptr<render::model::Builder> build(
        std::string&& name,
        std::shared_ptr<render::mesh::Mesh>&& mesh,
        core::sync::EndCallerIgnored&& c,
        bool is_transformable) noexcept final;

public:
    explicit ModelManager(Engine& e) noexcept;
    ~ModelManager() noexcept final;
};
}

#endif