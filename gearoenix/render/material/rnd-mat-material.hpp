#ifndef GEAROEMIX_RENDER_MATERIAL_MATERIAL_HPP
#define GEAROEMIX_RENDER_MATERIAL_MATERIAL_HPP
#include "../../core/cr-build-configuration.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "rnd-mat-frame.hpp"
#include "rnd-mat-translucency-mode.hpp"
#include "rnd-mat-uniform.hpp"
namespace gearoenix {
namespace system {
    namespace stream {
        class Stream;
    }
}
namespace render {
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
            std::shared_ptr<texture::Texture2D> color = nullptr;
            std::shared_ptr<texture::Texture2D> metallic_roughness = nullptr;
            std::shared_ptr<texture::Texture2D> normal = nullptr;
            std::shared_ptr<texture::Texture2D> emissive = nullptr;
            TranslucencyMode::Id translucency = TranslucencyMode::Opaque;
            Frame frames[GX_FRAMES_COUNT];
			Uniform uniform;

			void initialize();

        public:
			Material(const std::shared_ptr<engine::Engine>& e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end);
			Material(const std::shared_ptr<system::stream::Stream>& f, const std::shared_ptr<engine::Engine>& e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end);
            ~Material();
        };
    }
}
}
#endif
