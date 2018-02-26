#ifndef GEAROENIX_DX11_SHADER_FONT_COLORED_HPP
#define GEAROENIX_DX11_SHADER_FONT_COLORED_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_DIRECTX11
#include "../../render/material/rnd-mat-font-colored.hpp"
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
        class FontColored : public Shader {
        public:
            class Resources : public render::material::FontColored::Resources {
            public:
                Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
                void bind();
            };

        public:
            FontColored(Engine* eng, core::EndCaller<core::EndCallerIgnore> end);
            ~FontColored();
            void use();
            const std::vector<render::shader::stage::Id>& get_stages_ids() const;
        };
    }
}
}
#endif
#endif
