#ifndef GEAROENIX_RENDER_MATERIAL_MATERIAL_HPP
#define GEAROENIX_RENDER_MATERIAL_MATERIAL_HPP
#include "../../core/cr-build-configuration.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../math/math-vector.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "../texture/rnd-txt-texture-2d.hpp"
#include "rnd-mat-translucency-mode.hpp"
#include "rnd-mat-uniform.hpp"
#include <optional>
#include <vector>
namespace gearoenix {
namespace system::stream {
    class Stream;
}
namespace render {
    namespace engine {
        class Engine;
    }
    namespace material {
        class Material {
            GX_GET_UCPTR_PRV(buffer::FramedUniform, uniform_buffers)
            GX_GET_VAL_PRV(TranslucencyMode, translucency, TranslucencyMode::Opaque)
            GX_GET_VAL_PRV(bool, is_shadow_caster, true)
            GX_GET_CREF_PRV(std::shared_ptr<texture::Texture2D>, color_texture)
            GX_GET_CREF_PRV(std::optional<math::Vec3>, color_value)
            GX_GET_CREF_PRV(std::shared_ptr<texture::Texture2D>, emission_texture)
            GX_GET_CREF_PRV(std::optional<math::Vec3>, emission_value)
            GX_GET_CREF_PRV(std::shared_ptr<texture::Texture2D>, metallic_roughness_texture)
            GX_GET_CREF_PRV(std::optional<math::Vec2>, metallic_roughness_value)
            GX_GET_CREF_PRV(std::shared_ptr<texture::Texture2D>, normal_texture)
            GX_GET_CREF_PRV(std::optional<math::Vec3>, normal_value)
            GX_GET_CREF_PRV(Uniform, uniform)
        private:
            engine::Engine* const e;

        public:
            Material(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept;
            Material(system::stream::Stream* f, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept;
            ~Material() noexcept = default;
            void update_uniform() noexcept;
            void set_metallic_factor(core::Real f) noexcept;
            void set_roughness_factor(core::Real f) noexcept;
            void set_color(
                core::Real r, core::Real g, core::Real b,
                const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept;
            void set_color(const std::shared_ptr<texture::Texture2D>& color) noexcept;
        };
    }
}
}
#endif
