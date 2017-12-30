#ifndef GEAROENIX_DX11_SHADER_DIRECTIONAL_D2_SPECULATED_BAKED_FULL_OPAQUE_HPP
#define GEAROENIX_DX11_SHADER_DIRECTIONAL_D2_SPECULATED_BAKED_FULL_OPAQUE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_DIRECTX11
#include "../../render/material/rnd-mat-directional-d2-speculated-baked-full-opaque.hpp"
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
        class DirectionalD2SpeculatedBakedFullOpaque : public Shader {
        public:
            class Resources : public render::material::DirectionalD2SpeculatedBakedFullOpaque::Resources {
            public:
                Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u);
                void bind();
            };

        public:
            DirectionalD2SpeculatedBakedFullOpaque(Engine* eng, core::EndCaller<core::EndCallerIgnore> end);
            ~DirectionalD2SpeculatedBakedFullOpaque();
            void use();
            const std::vector<render::shader::stage::Id>& get_stages_ids() const;
        };
    }
}
}
#endif
#endif
