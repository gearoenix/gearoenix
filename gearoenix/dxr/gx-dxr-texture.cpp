#include "gx-dxr-texture.hpp"

#ifdef GX_RENDER_DXR_ENABLED
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "gx-dxr-check.hpp"
#include "gx-dxr-device.hpp"
#include "gx-dxr-engine.hpp"
#include "gx-dxr-uploader.hpp"

gearoenix::dxr::Texture2D::Texture2D(
    std::string name,
    const render::texture::TextureInfo& info,
    Engine& e,
    Microsoft::WRL::ComPtr<ID3D12Resource>&& r,
    Descriptor&& d) noexcept
    : render::texture::Texture2D(std::move(name), info, e)
    , resource(std::move(r))
    , descriptor(std::move(d))
{
}

gearoenix::dxr::Texture2D::~Texture2D() noexcept = default;

gearoenix::dxr::TextureManager::TextureManager(
    Engine& e) noexcept
    : e(e)
    , descriptor_manager(e.get_descriptor_manager())
    , uploader(e.get_uploader())
    , device(descriptor_manager->get_device())
{
}

gearoenix::dxr::TextureManager::~TextureManager() noexcept = default;

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::dxr::TextureManager::create_2d_from_pixels(
    std::string name,
    std::vector<std::vector<std::uint8_t>> pixels,
    const render::texture::TextureInfo& info,
    const core::sync::EndCallerIgnored& c) noexcept
{
    auto descriptor = descriptor_manager->allocate_texture_2d();
    Microsoft::WRL::ComPtr<ID3D12Resource> resource;
    D3D12_RESOURCE_DESC desc;
    GX_SET_ZERO(desc)
    if (info.has_mipmap) {
        desc.MipLevels = static_cast<UINT16>(render::texture::Texture::compute_mipmaps_count(
            static_cast<std::size_t>(info.width),
            static_cast<std::size_t>(info.height)));
        GX_UNIMPLEMENTED
    } else {
        desc.MipLevels = 1;
    }
    switch (info.format) {
    case render::texture::TextureFormat::RgbaUint8:
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        break;
    case render::texture::TextureFormat::RgbaFloat32:
        desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        break;
    default:
        GX_UNIMPLEMENTED
        break;
    }
    desc.Width = static_cast<UINT64>(info.width);
    desc.Height = static_cast<UINT>(info.height);
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;
    desc.DepthOrArraySize = 1;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    const auto p = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    GX_DXR_CHECK(device->get_device()->CreateCommittedResource(&p, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&resource)))
    auto t = std::make_shared<Texture2D>(std::move(name), info, e, std::move(resource), std::move(descriptor));
    uploader->upload(std::move(pixels[0]), std::shared_ptr<Texture2D>(t), core::sync::EndCallerIgnored([this, t, desc, c]() {
        D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc;
        GX_SET_ZERO(srv_desc)
        srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srv_desc.Format = desc.Format;
        srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srv_desc.Texture2D.MipLevels = 1;
        device->get_device()->CreateShaderResourceView(t->resource.Get(), &srv_desc, t->descriptor.cpu_handle);
    }));
    return t;
}

#endif
