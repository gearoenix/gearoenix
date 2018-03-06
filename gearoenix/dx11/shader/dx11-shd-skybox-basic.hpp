#ifndef GEAROENIX_DX11_SHADER_SKYBOX_BASIC_HPP
#define GEAROENIX_DX11_SHADER_SKYBOX_BASIC_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_DIRECTX11
#include "../../render/material/rnd-mat-skybox-basic.hpp"
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
        class SkyboxBasic : public Shader {
        public:
            class Resources : public render::material::SkyboxBasic::Resources {
            public:
                Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
                void bind();
            };

        public:
            SkyboxBasic(Engine* eng, core::EndCaller<core::EndCallerIgnore> end);
            ~SkyboxBasic();
            void use();
            const std::vector<render::shader::stage::Id>& get_stages_ids() const;
        };
    }
}
}
#endif
#endif