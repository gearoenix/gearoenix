#include "gx-mtl-texture.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "gx-mtl-engine.hpp"
#import "gx-mtl-heap.hpp"
#import "gx-mtl-uploader.hpp"
#import <vector>

gearoenix::metal::Texture2D::Texture2D(Engine& e, id<MTLTexture> resource, id<MTLSamplerState> sampler, const render::texture::TextureInfo& info, std::string name)
    : render::texture::Texture2D(std::move(name), info, e)
    , resource(resource)
    , sampler(sampler)
{
}

gearoenix::metal::Texture2D::~Texture2D() { [resource release]; }

void convert(MTLSamplerDescriptor* desc, const gearoenix::render::texture::Filter mag_filter, const gearoenix::render::texture::Filter min_filter)
{
    if (mag_filter == gearoenix::render::texture::Filter::LinearMipmapLinear && min_filter == gearoenix::render::texture::Filter::LinearMipmapLinear) {
        desc.minFilter = MTLSamplerMinMagFilterLinear;
        desc.magFilter = MTLSamplerMinMagFilterLinear;
        desc.mipFilter = MTLSamplerMipFilterLinear;
    } else if (mag_filter == gearoenix::render::texture::Filter::Linear && min_filter == gearoenix::render::texture::Filter::LinearMipmapLinear) {
        desc.minFilter = MTLSamplerMinMagFilterLinear;
        desc.magFilter = MTLSamplerMinMagFilterLinear;
        desc.mipFilter = MTLSamplerMipFilterLinear;
    } else if (mag_filter == gearoenix::render::texture::Filter::Linear && min_filter == gearoenix::render::texture::Filter::Linear) {
        desc.minFilter = MTLSamplerMinMagFilterLinear;
        desc.magFilter = MTLSamplerMinMagFilterLinear;
        desc.mipFilter = MTLSamplerMipFilterNotMipmapped;
    } else if (mag_filter == gearoenix::render::texture::Filter::Nearest && min_filter == gearoenix::render::texture::Filter::Nearest) {
        desc.minFilter = MTLSamplerMinMagFilterNearest;
        desc.magFilter = MTLSamplerMinMagFilterNearest;
        desc.mipFilter = MTLSamplerMipFilterNotMipmapped;
    } else {
        GX_UNEXPECTED
    }
}

static MTLSamplerAddressMode convert(const gearoenix::render::texture::Wrap w)
{
    switch (w) {
    case gearoenix::render::texture::Wrap::Repeat:
        return MTLSamplerAddressModeRepeat;
    case gearoenix::render::texture::Wrap::Mirror:
        return MTLSamplerAddressModeMirrorRepeat;
    case gearoenix::render::texture::Wrap::ClampToEdge:
        return MTLSamplerAddressModeClampToEdge;
    }
}

static id<MTLSamplerState> create_sampler(const gearoenix::render::texture::SamplerInfo& sample_info, id<MTLDevice> device, MTLSamplerDescriptor* desc)
{
    convert(desc, sample_info.mag_filter, sample_info.min_filter);
    desc.rAddressMode = convert(sample_info.wrap_r);
    desc.sAddressMode = convert(sample_info.wrap_s);
    desc.tAddressMode = convert(sample_info.wrap_t);
    desc.maxAnisotropy = sample_info.anisotropic_level;
    desc.normalizedCoordinates = YES;
    desc.supportArgumentBuffers = YES;
    return [device newSamplerStateWithDescriptor:desc];
}

static boost::container::flat_map<gearoenix::render::texture::SamplerInfo, id<MTLSamplerState>> create_samplers(const std::vector<gearoenix::render::texture::SamplerInfo>& sample_infos, id<MTLDevice> device)
{
    boost::container::flat_map<gearoenix::render::texture::SamplerInfo, id<MTLSamplerState>> samplers;
    MTLSamplerDescriptor* desc = [MTLSamplerDescriptor new];
    for (auto& sample_info : sample_infos) {
        samplers[sample_info] = create_sampler(sample_info, device, desc);
    }
    return samplers;
}

gearoenix::metal::TextureManager::TextureManager(Engine& e)
    : e(e)
    , samplers(create_samplers(
        { render::texture::SamplerInfo {
             .min_filter = render::texture::Filter::Linear,
             .mag_filter = render::texture::Filter::Linear,
             .wrap_s = render::texture::Wrap::Repeat,
             .wrap_t = render::texture::Wrap::Repeat,
             .wrap_r = render::texture::Wrap::Repeat,
             .anisotropic_level = 0,
         },
           render::texture::SamplerInfo {
               .min_filter = render::texture::Filter::Nearest,
               .mag_filter = render::texture::Filter::Nearest,
               .wrap_s = render::texture::Wrap::Repeat,
               .wrap_t = render::texture::Wrap::Repeat,
               .wrap_r = render::texture::Wrap::Repeat,
               .anisotropic_level = 0,
           },
           render::texture::SamplerInfo {
               .min_filter = render::texture::Filter::LinearMipmapLinear,
               .mag_filter = render::texture::Filter::Linear,
               .wrap_s = render::texture::Wrap::Repeat,
               .wrap_t = render::texture::Wrap::Repeat,
               .wrap_r = render::texture::Wrap::Repeat,
               .anisotropic_level = 0,
           } },
          e.get_device()))
{
}

gearoenix::metal::TextureManager::~TextureManager()
{
    for (auto& s : samplers)
        [s.second release];
}

[[nodiscard]] std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::metal::TextureManager::create_2d_from_pixels(
    std::string name, std::vector<std::vector<std::uint8_t>> pixels, const render::texture::TextureInfo& info, const core::job::EndCaller& c)
{
    const auto sampler_search = samplers.find(info.sampler_info);
    if (samplers.end() == sampler_search)
        GX_UNEXPECTED
    const id<MTLSamplerState> sampler = sampler_search->second;
    MTLTextureDescriptor* const texture_descriptor = [MTLTextureDescriptor new];
    texture_descriptor.width = info.width;
    texture_descriptor.height = info.height;
    texture_descriptor.depth = 1;
    texture_descriptor.storageMode = MTLStorageModePrivate;
    texture_descriptor.arrayLength = 1;
    texture_descriptor.textureType = MTLTextureType2D;
    texture_descriptor.mipmapLevelCount = pixels.size() == 1 && info.has_mipmap ? render::texture::Texture::compute_mipmaps_count(info.width, info.height) : pixels.size();
    id<MTLTexture> texture = [e.get_heap_manager()->gpu newTextureWithDescriptor:texture_descriptor];
    auto result = std::make_shared<Texture2D>(e, texture, sampler, info, std::move(name));
    core::job::EndCaller end([c, result] { });
    e.get_uploader()->upload(texture, texture_descriptor, std::move(pixels), std::move(end));
    return result;
}

#endif
