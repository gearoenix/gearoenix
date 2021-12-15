#pragma once

#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DXR_ENABLED
#include "../render/texture/gx-rnd-txt-manager.hpp"
#include "../render/texture/gx-rnd-txt-texture-2d.hpp"
#include "gx-dxr-descriptor.hpp"
#include "gx-dxr-loader.hpp"
#include <memory>

namespace gearoenix::dxr {

struct Engine;
struct Uploader;

struct Texture2D final : public render::texture::Texture2D {
    friend struct TextureManager;

    GX_GET_REFC_PRV(Microsoft::WRL::ComPtr<ID3D12Resource>, resource)
    GX_GET_REFC_PRV(Descriptor, descriptor)

    Texture2D(
        std::string name,
        const render::texture::TextureInfo& info,
        Engine& e,
        Microsoft::WRL::ComPtr<ID3D12Resource>&& resource,
        Descriptor&& descriptor) noexcept;
    ~Texture2D() noexcept final;
};

struct TextureManager final : public render::texture::Manager {
    GX_GET_RRF_PRV(Engine, e)
    GX_GET_REFC_PRV(std::shared_ptr<DescriptorManager>, descriptor_manager)
    GX_GET_REFC_PRV(std::shared_ptr<Uploader>, uploader)
    GX_GET_REFC_PRV(std::shared_ptr<Device>, device)

    TextureManager(Engine& e) noexcept;
    ~TextureManager() noexcept final;
    [[nodiscard]] std::shared_ptr<render::texture::Texture2D> create_2d_from_pixels(
        std::string name,
        std::vector<std::vector<std::uint8_t>> pixels,
        const render::texture::TextureInfo& info,
        const core::sync::EndCallerIgnored& c) noexcept final;
};
}

#endif