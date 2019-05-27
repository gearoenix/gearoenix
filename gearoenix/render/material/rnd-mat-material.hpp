#ifndef GEAROENIX_RENDER_MATERIAL_MATERIAL_HPP
#define GEAROENIX_RENDER_MATERIAL_MATERIAL_HPP
#include "../../core/cr-build-configuration.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "rnd-mat-translucency-mode.hpp"
#include "rnd-mat-uniform.hpp"
#include <vector>
namespace gearoenix {
namespace system::stream {
        class Stream;
    }
namespace render {
    namespace buffer {
        class FramedUniform;
    }
    namespace engine {
        class Engine;
    }
    namespace texture {
        class Texture2D;
    }
    namespace material {
        class Material {
        private:
            const std::shared_ptr<engine::Engine> e;
            const std::shared_ptr<buffer::FramedUniform> uniform_buffers;

            std::shared_ptr<texture::Texture2D> color = nullptr;
            std::shared_ptr<texture::Texture2D> metallic_roughness = nullptr;
            std::shared_ptr<texture::Texture2D> normal = nullptr;
            std::shared_ptr<texture::Texture2D> emissive = nullptr;
            TranslucencyMode::Id translucency = TranslucencyMode::Opaque;

            Uniform uniform;

        public:
            Material(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept ;
            Material(system::stream::Stream* f, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept ;
            ~Material() noexcept = default;
            void update_uniform() noexcept ;
            const std::shared_ptr<buffer::FramedUniform>& get_uniform_buffers() const noexcept ;
            const std::shared_ptr<texture::Texture2D>& get_color() const noexcept ;
            const std::shared_ptr<texture::Texture2D>& get_metallic_roughness() const noexcept ;
            const std::shared_ptr<texture::Texture2D>& get_normal() const noexcept ;
            const std::shared_ptr<texture::Texture2D>& get_emissive() const noexcept ;
            void set_metallic_factor(core::Real f) noexcept;
            void set_roughness_factor(core::Real f) noexcept ;
        };
    }
}
}
#endif
