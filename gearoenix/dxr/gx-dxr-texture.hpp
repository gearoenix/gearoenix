#pragma once

#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DXR_ENABLED
#include "gx-dxr-descriptor.hpp"
#include "gx-dxr-loader.hpp"

#include <memory>

namespace gearoenix::dxr {

struct Uploader;

struct Texture2D final {
    friend struct TextureManager;

    GX_GET_REFC_PRV(Microsoft::WRL::ComPtr<ID3D12Resource>, resource)
    GX_GET_REFC_PRV(Descriptor, descriptor)

    Texture2D(Microsoft::WRL::ComPtr<ID3D12Resource>&& resource, Descriptor&& descriptor) noexcept;
    ~Texture2D() noexcept;
};

struct TextureManager final {
    GX_GET_REFC_PRV(std::shared_ptr<DescriptorManager>, descriptor_manager)
    GX_GET_REFC_PRV(std::shared_ptr<Uploader>, uploader)
    GX_GET_REFC_PRV(std::shared_ptr<Device>, device)
    GX_GET_REFC_PRV(std::shared_ptr<Texture2D>, checkers)

    TextureManager(std::shared_ptr<Uploader> uploader, std::shared_ptr<DescriptorManager> descriptor_manager) noexcept;
    ~TextureManager() noexcept;
    [[nodiscard]] std::shared_ptr<Texture2D> create_2d(std::vector<std::uint8_t> pixels) noexcept;
};
}

#endif