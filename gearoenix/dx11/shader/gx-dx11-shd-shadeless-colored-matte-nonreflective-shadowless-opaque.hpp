#ifndef GEAROENIX_DX11_SHADER_SHADELESS_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE_HPP
#define GEAROENIX_DX11_SHADER_SHADELESS_COLORED_MATTE_NONREFLECTIVE_SHADOWLESS_OPAQUE_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef USE_DIRECTX11
#include "../../render/material/gx-rnd-mat-shadeless-colored-matte-nonreflective-shadowless-opaque.hpp"
#include "dx11-shader.hpp"
namespace gearoenix {
namespace dx11 {
    namespace buffer {
        class Uniform;
    }
    namespace pipeline {
        class Pipeline;
    }
    namespace shader {
        class ShadelessColoredMatteNonreflectiveShadowlessOpaque : public Shader {
        public:
            class Resources : public render::material::ShadelessColoredMatteNonreflectiveShadowlessOpaque::Resources {
            public:
                Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
                void bind();
            };

        protected:
        public:
            ShadelessColoredMatteNonreflectiveShadowlessOpaque(core::Id my_id, Engine* eng, core::sync::EndCaller<core::sync::EndCallerIgnore> end);
            ~ShadelessColoredMatteNonreflectiveShadowlessOpaque();
            void use();
            const std::vector<render::shader::stage::Id>& get_stages_ids() const;
        };
    }
}
}
#endif
#endif
