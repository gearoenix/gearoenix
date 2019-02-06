#ifndef GEAROENIX_DX11_SHADER_COMPILER_HPP
#define GEAROENIX_DX11_SHADER_COMPILER_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_DIRECTX11
#include "../../render/shader/rnd-shd-shader.hpp"
#include <d3d11.h>
#include <string>
#include <vector>
namespace gearoenix {
namespace dx11 {
    class Engine;
    namespace shader {
        class Shader : public render::shader::Shader {
        protected:
            Engine* eng = nullptr;
            ID3D11VertexShader* vtxshd = nullptr;
            ID3D11PixelShader* frgshd = nullptr;
            ID3D11InputLayout* inlay = nullptr;
            void run();
            void* compile_shader(
                const std::string& shd,
                const render::shader::stage::Id& shader_type,
                std::vector<D3D11_INPUT_ELEMENT_DESC> polygon_layout = {});

        public:
            Shader(core::Id my_id, Engine* eng, core::sync::EndCaller<core::sync::EndCallerIgnore> end);
            virtual ~Shader();
        };
    }
}
}
#endif
#endif
