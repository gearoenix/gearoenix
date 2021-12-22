#include "gx-dxr-texture.hpp"

#ifdef GX_RENDER_DXR_ENABLED
#include "../core/gx-cr-string.hpp"
#include "../core/macro/gx-cr-mcr-assert.hpp"
#include "../core/macro/gx-cr-mcr-counter.hpp"
#include "../core/macro/gx-cr-mcr-zeroer.hpp"
#include "../math/gx-math-numeric.hpp"
#include "gx-dxr-check.hpp"
#include "gx-dxr-command.hpp"
#include "gx-dxr-device.hpp"
#include "gx-dxr-engine.hpp"
#include "gx-dxr-pipeline.hpp"
#include "gx-dxr-queue.hpp"
#include "gx-dxr-uploader.hpp"
#include <vector>

gearoenix::dxr::Texture2D::Texture2D(
    std::string name,
    const render::texture::TextureInfo& info,
    Engine& e,
    const UINT sampler_index,
    Microsoft::WRL::ComPtr<ID3D12Resource>&& r,
    Descriptor&& d) noexcept
    : render::texture::Texture2D(std::move(name), info, e)
    , sampler_index(sampler_index)
    , resource(std::move(r))
    , descriptor(std::move(d))
{
}

gearoenix::dxr::Texture2D::~Texture2D() noexcept = default;

const std::map<gearoenix::render::texture::SamplerInfo, UINT> gearoenix::dxr::TextureManager::samplers_indices {
    { render::texture::SamplerInfo {
          .min_filter = render::texture::Filter::Linear,
          .mag_filter = render::texture::Filter::Linear,
          .wrap_s = render::texture::Wrap::Repeat,
          .wrap_t = render::texture::Wrap::Repeat,
          .wrap_r = render::texture::Wrap::Repeat,
          .anisotropic_level = 0,
      },
        0u },
    { render::texture::SamplerInfo {
          .min_filter = render::texture::Filter::Nearest,
          .mag_filter = render::texture::Filter::Nearest,
          .wrap_s = render::texture::Wrap::Repeat,
          .wrap_t = render::texture::Wrap::Repeat,
          .wrap_r = render::texture::Wrap::Repeat,
          .anisotropic_level = 0,
      },
        1u },
    { render::texture::SamplerInfo {
          .min_filter = render::texture::Filter::LinearMipmapLinear,
          .mag_filter = render::texture::Filter::Linear,
          .wrap_s = render::texture::Wrap::Repeat,
          .wrap_t = render::texture::Wrap::Repeat,
          .wrap_r = render::texture::Wrap::Repeat,
          .anisotropic_level = 0,
      },
        2u }
};

void gearoenix::dxr::TextureManager::wait(const UINT64 fv) noexcept
{
    auto fence_event = CreateEventW(nullptr, false, false, nullptr);
    GX_ASSERT_D(NULL != fence_event)
    GX_DXR_CHECK(fence->SetEventOnCompletion(fv, fence_event));
    GX_ASSERT_D(WAIT_OBJECT_0 == WaitForSingleObjectEx(fence_event, INFINITE, false))
    GX_ASSERT_D(0 != CloseHandle(fence_event))
}

gearoenix::dxr::TextureManager::TextureManager(
    Engine& e) noexcept
    : e(e)
    , descriptor_manager(e.get_descriptor_manager())
    , uploader(e.get_uploader())
    , device(descriptor_manager->get_device())
    , queue(new Queue(device, Queue::Type::Direct)) {
        GX_DXR_CHECK(device->get_device()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)))
    }

    gearoenix::dxr::TextureManager::~TextureManager() noexcept = default;

std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::dxr::TextureManager::create_2d_from_pixels(
    std::string name,
    std::vector<std::vector<std::uint8_t>> pixels,
    const render::texture::TextureInfo& info,
    const core::sync::EndCallerIgnored& c) noexcept
{
    GX_GUARD_LOCK(textures_2d)
    auto textures_2d_search = textures_2d.find(name);
    if (textures_2d.end() != textures_2d_search)
        return textures_2d_search->second;
    const auto sampler_search = samplers_indices.find(info.sampler_info);
    GX_ASSERT(samplers_indices.end() != sampler_search)
    auto descriptor = descriptor_manager->allocate_texture_2d();
    Microsoft::WRL::ComPtr<ID3D12Resource> resource;
    D3D12_RESOURCE_DESC desc;
    GX_SET_ZERO(desc)
    bool needs_mipmap_generator = false;
    if (info.has_mipmap) {
        if (pixels.size() == 1) {
            desc.MipLevels = static_cast<UINT16>(render::texture::Texture::compute_mipmaps_count(
                static_cast<std::size_t>(info.width),
                static_cast<std::size_t>(info.height)));
            needs_mipmap_generator = true;
        } else {
            desc.MipLevels = static_cast<UINT16>(pixels.size());
        }
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
    desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    desc.DepthOrArraySize = 1;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    const auto p = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    GX_DXR_CHECK(device->get_device()->CreateCommittedResource(&p, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&resource)))
    const auto w_name = core::String::to_wstring(name);
    GX_DXR_CHECK(resource->SetName(w_name.c_str()))
    auto t = std::make_shared<Texture2D>(name, info, e, sampler_search->second, std::move(resource), std::move(descriptor));

    core::sync::EndCallerIgnored on_mipmap_complete([this, t, desc, c]() {
        D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc;
        GX_SET_ZERO(srv_desc)
        srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srv_desc.Format = desc.Format;
        srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srv_desc.Texture2D.MipLevels = desc.MipLevels;
        device->get_device()->CreateShaderResourceView(t->resource.Get(), &srv_desc, t->descriptor.cpu_handle);
    });

    const core::sync::EndCallerIgnored on_upload_complete([this, t, desc, on_mipmap_complete = std::move(on_mipmap_complete), needs_mipmap_generator]() {
        if (!needs_mipmap_generator)
            return;
        worker.push([this, t = std::move(t), desc, on_mipmap_complete = std::move(on_mipmap_complete)] {
            D3D12_SHADER_RESOURCE_VIEW_DESC src_desc;
            GX_SET_ZERO(src_desc)
            src_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            src_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            src_desc.Texture2D.MipLevels = 1;
            src_desc.Format = desc.Format;

            D3D12_UNORDERED_ACCESS_VIEW_DESC dst_desc;
            GX_SET_ZERO(dst_desc)
            dst_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
            dst_desc.Format = desc.Format;

            auto* const pip_mgr = e.get_pipeline_manager().get();
            auto* const dev = device->get_device().Get();
            auto* const txt = t->get_resource().Get();

            auto command = std::make_shared<Command>(*device, D3D12_COMMAND_LIST_TYPE_DIRECT);
            command->begin(pip_mgr->get_mipmap_generator_pipeline_state().Get());
            auto* const cmd = command->get_list();
            cmd->SetComputeRootSignature(pip_mgr->get_mipmap_generator_root_signature().Get());
            ID3D12DescriptorHeap* heaps[] = {
                descriptor_manager->get_allocator().heap.Get(),
                descriptor_manager->get_sampler_allocator().heap.Get(),
            };
            cmd->SetDescriptorHeaps(GX_COUNT_OF(heaps), heaps);
            const auto b1 = CD3DX12_RESOURCE_BARRIER::Transition(
                txt,
                D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
                D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
            const auto b2 = CD3DX12_RESOURCE_BARRIER::UAV(txt);
            cmd->ResourceBarrier(1, &b1);
            auto descriptors = std::make_shared<std::vector<Descriptor>>(); // It is for preventing stupid MSVC's copy-on-cast-of-lambda
            for (UINT mip_level = 0, dst_mip_level = 1; dst_mip_level < desc.MipLevels; ++mip_level, ++dst_mip_level) {
                const auto src_barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                    txt,
                    D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
                    D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
                    mip_level);
                cmd->ResourceBarrier(1, &src_barrier);
                const auto dst_width = math::Numeric::maximum(static_cast<UINT>(desc.Width >> dst_mip_level), static_cast<UINT>(1));
                const auto dst_height = math::Numeric::maximum(static_cast<UINT>(desc.Height >> dst_mip_level), static_cast<UINT>(1));
                auto src_descriptor = descriptor_manager->allocate_texture_2d();
                auto dst_descriptor = descriptor_manager->allocate_texture_2d();
                src_desc.Texture2D.MostDetailedMip = mip_level;
                dev->CreateShaderResourceView(txt, &src_desc, src_descriptor.cpu_handle);
                dst_desc.Texture2D.MipSlice = dst_mip_level;
                dev->CreateUnorderedAccessView(txt, nullptr, &dst_desc, dst_descriptor.cpu_handle);
                cmd->SetComputeRoot32BitConstant(0, mip_level, 0);
                cmd->SetComputeRoot32BitConstant(0, dst_width, 1);
                cmd->SetComputeRoot32BitConstant(0, dst_height, 2);
                const auto texel_w = 1.0f / static_cast<float>(dst_width);
                const auto texel_h = 1.0f / static_cast<float>(dst_height);
                cmd->SetComputeRoot32BitConstant(0, *reinterpret_cast<const UINT32*>(&texel_w), 4);
                cmd->SetComputeRoot32BitConstant(0, *reinterpret_cast<const UINT32*>(&texel_h), 5);
                cmd->SetComputeRootDescriptorTable(1, src_descriptor.gpu_handle);
                cmd->SetComputeRootDescriptorTable(2, dst_descriptor.gpu_handle);
                cmd->Dispatch((dst_width >> 3) + 1, (dst_height >> 3) + 1, 1);
                cmd->ResourceBarrier(1, &b2);
                descriptors->push_back(std::move(src_descriptor));
                descriptors->push_back(std::move(dst_descriptor));
            }
            const auto last_dst_barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                txt,
                D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
                D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
                desc.MipLevels - 1);
            cmd->ResourceBarrier(1, &last_dst_barrier);
            command->close();
            queue->exe(cmd);
            const auto current_fence_value = fence_value;
            ++fence_value;
            GX_DXR_CHECK(queue->get_command_queue()->Signal(fence.Get(), current_fence_value))
            worker.push([this,
                            t = std::move(t),
                            command = std::move(command),
                            on_mipmap_complete = std::move(on_mipmap_complete),
                            descriptors = std::move(descriptors),
                            current_fence_value]() {
                wait(current_fence_value);
                (void)t;
                (void)command;
                (void)on_mipmap_complete;
                (void)descriptors;
            });
        });
    });

    for (std::size_t mipmap_index = 0; mipmap_index < pixels.size(); ++mipmap_index) {
        uploader->upload(
            std::move(pixels[mipmap_index]), std::shared_ptr<Texture2D>(t),
            static_cast<UINT>(mipmap_index), on_upload_complete);
    }
    textures_2d.emplace(std::move(name), t);
    return t;
}

void gearoenix::dxr::TextureManager::convert(const render::texture::SamplerInfo& in, D3D12_SAMPLER_DESC& out) noexcept
{
    const static std::map<std::pair<render::texture::Filter, render::texture::Filter>, D3D12_FILTER> map_filter {
        { { render::texture::Filter::Nearest, render::texture::Filter::Nearest }, D3D12_FILTER_MIN_MAG_MIP_POINT },
        { { render::texture::Filter::Linear, render::texture::Filter::Linear }, D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT },
        { { render::texture::Filter::Linear, render::texture::Filter::LinearMipmapLinear }, D3D12_FILTER_MIN_MAG_MIP_LINEAR },
    };

    const static std::map<render::texture::Wrap, D3D12_TEXTURE_ADDRESS_MODE> map_address {
        { render::texture::Wrap::Repeat, D3D12_TEXTURE_ADDRESS_MODE_WRAP },
    };

    GX_SET_ZERO(out)

    if (const auto filter_search = map_filter.find(std::make_pair(in.mag_filter, in.min_filter)); map_filter.end() != filter_search)
        out.Filter = filter_search->second;
    else
        GX_UNIMPLEMENTED

    if (const auto s = map_address.find(in.wrap_s); map_address.end() != s)
        out.AddressU = s->second;
    else
        GX_UNIMPLEMENTED
    if (const auto s = map_address.find(in.wrap_t); map_address.end() != s)
        out.AddressV = s->second;
    else
        GX_UNIMPLEMENTED
    if (const auto s = map_address.find(in.wrap_r); map_address.end() != s)
        out.AddressW = s->second;
    else
        GX_UNIMPLEMENTED

    out.MinLOD = 0;
    out.MaxLOD = D3D12_FLOAT32_MAX;
    out.MipLODBias = 0.0f;
    out.MaxAnisotropy = in.anisotropic_level;
    out.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
}

#endif
