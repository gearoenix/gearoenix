#ifndef GEAROENIX_RENDER_ENGINE_SPECIFICATION_HPP
#define GEAROENIX_RENDER_ENGINE_SPECIFICATION_HPP
namespace gearoenix::render::engine {
struct Specification final {
    unsigned int texture_maximum_aspect = 0;
    unsigned int texture_maximum_target_attachments = 0;
    bool is_deferred_supported = true;
    bool is_raytracing_supported = true;
    bool is_float_texture_supported = true;
};
}
#endif
