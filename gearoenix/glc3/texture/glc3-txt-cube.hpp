#ifndef GEAROENIX_GLC3_TEXTURE_CUBE_HPP
#define GEAROENIX_GLC3_TEXTURE_CUBE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../gl/gl-types.hpp"
#include "../../render/texture/rnd-txt-format.hpp"
#include "../../render/texture/rnd-txt-sample.hpp"
#include "../../render/texture/rnd-txt-texture-cube.hpp"
namespace gearoenix::glc3 {
namespace engine {
    class Engine;
}
namespace texture {
    class Cube : public render::texture::Cube {
    private:
        gl::uint texture_object = 0;

        Cube(core::Id my_id, engine::Engine* engine) noexcept;
    public:
        [[nodiscard]] static std::shared_ptr<Cube> construct(
            core::Id my_id,
            engine::Engine* engine,
            const void* data,
            render::texture::TextureFormat f,
            render::texture::SampleInfo s,
            unsigned int aspect,
            const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
        ~Cube() noexcept final;
        void bind(gl::enumerated texture_unit) const noexcept;
    };
}
}
#endif
#endif
