#ifndef GEAROENIX_RENDER_COMMAND_BUFFER_HPP
#define GEAROENIX_RENDER_COMMAND_BUFFER_HPP
#include "../../core/gx-cr-static.hpp"
#include "gx-rnd-cmd-type.hpp"
#include <memory>
#include <vector>

namespace gearoenix::render::pipeline {
class ResourceSet;
}

namespace gearoenix::render::texture {
class Target;
class Texture;
}

namespace gearoenix::render::command {
class Buffer {
    GX_GET_CVAL_PRT(Type, command_buffer_type)
protected:
    std::vector<Buffer*> recorded_secondaries;
    std::vector<pipeline::ResourceSet*> bound_resource_sets;
    std::vector<texture::Texture*> bound_texture_for_mipmap_generation;
    const texture::Target* render_target = nullptr;
    explicit Buffer(Type command_buffer_type) noexcept;

public:
    virtual ~Buffer() noexcept = default;
    virtual void begin() noexcept;
    virtual void end() noexcept;
    virtual void record(Buffer* o) noexcept;
    virtual void record_generate_mipmap(texture::Texture* t) noexcept;
    virtual void bind(pipeline::ResourceSet* r) noexcept;
    virtual void bind(const texture::Target* t) noexcept;
};
}

#endif