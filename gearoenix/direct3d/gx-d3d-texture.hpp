#pragma once

#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DIRECT3D_ENABLED
#include "../core/sync/gx-cr-sync-work-waiter.hpp"
#include "../render/texture/gx-rnd-txt-manager.hpp"
#include "../render/texture/gx-rnd-txt-texture-2d.hpp"
#include "gx-d3d-descriptor.hpp"
#include "gx-d3d-loader.hpp"
#include <memory>

namespace gearoenix::d3d {

struct Engine;
struct Uploader;
struct Queue;

struct Texture2D final : public render::texture::Texture2D {
    friend struct TextureManager;

    GX_GET_VAL_PRV(UINT, sampler_index, 0)
    GX_GET_REFC_PRV(Microsoft::WRL::ComPtr<ID3D12Resource>, resource)
    GX_GET_REFC_PRV(Descriptor, descriptor)

    Texture2D(
        std::string name,
        const render::texture::TextureInfo& info,
        Engine& e,
        UINT sampler_index,
        Microsoft::WRL::ComPtr<ID3D12Resource>&& resource,
        Descriptor&& descriptor) noexcept;
    ~Texture2D() noexcept final;
};

struct TextureManager final : public render::texture::Manager {
    const static std::map<render::texture::SamplerInfo, UINT> samplers_indices;

    GX_GET_RRF_PRV(Engine, e)
    GX_GET_REFC_PRV(std::shared_ptr<DescriptorManager>, descriptor_manager)
    GX_GET_REFC_PRV(std::shared_ptr<Uploader>, uploader)
    GX_GET_REFC_PRV(std::shared_ptr<Device>, device)

private:
    core::sync::WorkWaiter worker;
    const std::shared_ptr<Queue> queue;
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;
    UINT64 fence_value = 1;

    void wait(UINT64) noexcept;

public:
    TextureManager(Engine& e) noexcept;
    ~TextureManager() noexcept final;
    [[nodiscard]] std::shared_ptr<render::texture::Texture2D> create_2d_from_pixels(
        std::string name,
        std::vector<std::vector<std::uint8_t>> pixels,
        const render::texture::TextureInfo& info,
        const core::sync::EndCallerIgnored& c) noexcept final;

    static void convert(const render::texture::SamplerInfo& in, D3D12_SAMPLER_DESC& out) noexcept;
};
}

#endif