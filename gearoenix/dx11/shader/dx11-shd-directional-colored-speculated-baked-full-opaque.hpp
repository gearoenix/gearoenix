#ifndef GEAROENIX_DX11_SHADER_DIRECTIONAL_COLORED_SPECULATED_BAKED_FULL_OPAQUE_HPP
#define GEAROENIX_DX11_SHADER_DIRECTIONAL_COLORED_SPECULATED_BAKED_FULL_OPAQUE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_DIRECTX11
#include "../../render/material/rnd-mat-directional-colored-speculated-baked-full-opaque.hpp"
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
        class DirectionalColoredSpeculatedBakedFullOpaque : public Shader {
        public:
            class Resources : public render::material::DirectionalColoredSpeculatedBakedFullOpaque::Resources {
            public:
                Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
                void bind();
            };

        public:
            DirectionalColoredSpeculatedBakedFullOpaque(core::Id my_id, Engine* eng, core::EndCaller<core::EndCallerIgnore> end);
            ~DirectionalColoredSpeculatedBakedFullOpaque();
            void use();
            const std::vector<render::shader::stage::Id>& get_stages_ids() const;
        };
    }
}
}
#endif
#endif
