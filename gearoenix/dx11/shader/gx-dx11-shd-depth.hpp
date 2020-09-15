#ifndef GEAROENIX_DX11_SHADER_DEPTH_POSITION_HPP
#define GEAROENIX_DX11_SHADER_DEPTH_POSITION_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_DIRECTX11
#include "../../render/material/gx-rnd-mat-depth.hpp"
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
        class Depth : public Shader {
        public:
            class Resources : public render::material::Depth::Resources {
            public:
                Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
                void bind();
            };

        protected:
        public:
            Depth(core::Id my_id, Engine* eng, core::sync::EndCaller<core::sync::EndCallerIgnore> end);
            ~Depth();
            void use();
            const std::vector<render::shader::stage::Id>& get_stages_ids() const;
        };
    }
}
}
#endif
#endif
