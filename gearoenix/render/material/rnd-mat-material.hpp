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
        private:
            engine::Engine* const e;
            const std::shared_ptr<buffer::FramedUniform> uniform_buffers;

            std::shared_ptr<texture::Texture2D> color_txt;
            std::optional<math::Vec3> color = math::Vec3(0.999f, 0.999f, 0.999f);

            std::shared_ptr<texture::Texture2D> emission;
            std::shared_ptr<texture::Texture2D> metallic_roughness;
            std::shared_ptr<texture::Texture2D> normal;
            TranslucencyMode::Id translucency = TranslucencyMode::Opaque;

            bool is_shadow_caster = true;

            Uniform uniform;

        public:
            Material(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept;
            Material(system::stream::Stream* f, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept;
            ~Material() noexcept = default;
            void update_uniform() noexcept;
            [[nodiscard]] const std::shared_ptr<buffer::FramedUniform>& get_uniform_buffers() const noexcept;
            [[nodiscard]] const std::shared_ptr<texture::Texture2D>& get_color_texture() const noexcept;
            [[nodiscard]] const std::optional<math::Vec3>& get_color() const noexcept;
            [[nodiscard]] const std::shared_ptr<texture::Texture2D>& get_metallic_roughness() const noexcept;
            [[nodiscard]] const std::shared_ptr<texture::Texture2D>& get_normal() const noexcept;
            [[nodiscard]] const std::shared_ptr<texture::Texture2D>& get_emissive() const noexcept;
            [[nodiscard]] bool get_is_shadow_caster() const noexcept;
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
