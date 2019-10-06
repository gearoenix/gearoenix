#ifndef GEAROENIX_GLES2_TEXTURE_2D_HPP
#define GEAROENIX_GLES2_TEXTURE_2D_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../gl/gl-types.hpp"
#include "../../render/texture/rnd-txt-format.hpp"
#include "../../render/texture/rnd-txt-sample.hpp"
#include "../../render/texture/rnd-txt-texture-2d.hpp"
namespace gearoenix::gles2 {
namespace engine {
    class Engine;
}
namespace texture {
    class Texture2D : public render::texture::Texture2D {
    private:
        gl::uint texture_object = 0;

        Texture2D(core::Id my_id, engine::Engine* e) noexcept;
    public:
        [[nodiscard]] static std::shared_ptr<Texture2D> construct(
            core::Id my_id,
            engine::Engine* e,
            const void* data,
            render::texture::TextureFormat f,
            render::texture::SampleInfo s,
            unsigned int width,
            unsigned int heigt,
            const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
        Texture2D(core::Id my_id, gl::uint txtobj, engine::Engine* e) noexcept;
        ~Texture2D() noexcept final;
        void bind(gl::enumerated texture_unit) const noexcept;
    };
}
}
#endif
#endif
