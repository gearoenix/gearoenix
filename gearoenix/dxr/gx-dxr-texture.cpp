#include "gx-dxr-texture.hpp"

#ifdef GX_RENDER_DXR_ENABLED
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "gx-dxr-check.hpp"
#include "gx-dxr-device.hpp"
#include "gx-dxr-uploader.hpp"

gearoenix::dxr::Texture2D::Texture2D(
    Microsoft::WRL::ComPtr<ID3D12Resource>&& r,
    Descriptor&& d) noexcept
    : resource(std::move(r))
    , descriptor(std::move(d))
{
}

gearoenix::dxr::Texture2D::~Texture2D() noexcept = default;

gearoenix::dxr::TextureManager::TextureManager(
    std::shared_ptr<Uploader> _uploader,
    std::shared_ptr<DescriptorManager> _descriptor_manager) noexcept
    : descriptor_manager(std::move(_descriptor_manager))
    , uploader(std::move(_uploader))
    , device(descriptor_manager->get_device())
    , checkers(create_2d({ 0, 0, 0, 255, 255, 255, 255, 255, 0, 0, 0, 255, 255, 255, 255, 255 }))
{
}

gearoenix::dxr::TextureManager::~TextureManager() noexcept = default;

std::shared_ptr<gearoenix::dxr::Texture2D> gearoenix::dxr::TextureManager::create_2d(std::vector<std::uint8_t> pixels) noexcept
{
    auto descriptor = descriptor_manager->allocate();
    Microsoft::WRL::ComPtr<ID3D12Resource> resource;
    D3D12_RESOURCE_DESC desc;
    GX_SET_ZERO(desc)
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.Width = 2;
    desc.Height = 2;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;
    desc.DepthOrArraySize = 1;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    const auto p = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    GX_DXR_CHECK(device->get_device()->CreateCommittedResource(&p, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&resource)))
    std::shared_ptr<Texture2D> t(new Texture2D(std::move(resource), std::move(descriptor)));
    uploader->upload(std::move(pixels), std::shared_ptr<Texture2D>(t), core::sync::EndCallerIgnored([this, t, desc]() {
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
