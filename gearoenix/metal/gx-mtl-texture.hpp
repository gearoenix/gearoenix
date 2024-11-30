#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_METAL_ENABLED
#import "../render/texture/gx-rnd-txt-manager.hpp"
#import "../render/texture/gx-rnd-txt-texture-2d.hpp"
#import <Metal/MTLSampler.h>
#import <Metal/MTLTexture.h>
#include <boost/container/flat_map.hpp>

namespace gearoenix::metal {

struct Engine;
struct Uploader;

struct Texture2D final : public render::texture::Texture2D {
    const id<MTLTexture> resource;
    const id<MTLSamplerState> sampler;

    Texture2D(
        Engine& e,
        id<MTLTexture> resource,
        id<MTLSamplerState> sampler,
        const render::texture::TextureInfo& info,
        std::string name);
    ~Texture2D() final;
};

struct TextureManager final : public render::texture::Manager {
    Engine& e;
    const boost::container::flat_map<render::texture::SamplerInfo, id<MTLSamplerState>> samplers;

    TextureManager(Engine& e);
    ~TextureManager() final;
    [[nodiscard]] std::shared_ptr<render::texture::Texture2D> create_2d_from_pixels(
        std::string name,
        std::vector<std::vector<std::uint8_t>> pixels,
        const render::texture::TextureInfo& info,
        const core::job::EndCaller& c) final;
};
}

#endif