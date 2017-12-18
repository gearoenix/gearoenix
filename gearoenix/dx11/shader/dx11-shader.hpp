#ifndef GEAROENIX_DX11_SHADER_COMPILER_HPP
#define GEAROENIX_DX11_SHADER_COMPILER_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_DIRECTX11
#include "../../render/shader/rnd-shd-shader.hpp"
#include <string>
#include <vector>
#include <d3d11.h>
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
				std::vector<D3D11_INPUT_ELEMENT_DESC> polygon_layout = {}
			);
        public:
            Shader(Engine* eng, std::shared_ptr<core::EndCaller> end);
            virtual ~Shader();
        };
    }
}
}
#endif
#endif
