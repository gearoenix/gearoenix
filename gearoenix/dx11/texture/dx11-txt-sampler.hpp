#ifndef GEAROENIX_DX11_SAMPLER_HPP
#define GEAROENIX_DX11_SAMPLER_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_DIRECTX11
#include <d3d11.h>
namespace gearoenix {
	namespace core {
		class EndCaller;
	}
	namespace system {
		class File;
	}
	namespace dx11 {
		class Engine;
		namespace texture {
			class Sampler {
			private:
				Engine * engine;
				ID3D11SamplerState * sampler;
			public:
				Sampler(Engine* engine);
				~Sampler();
				const ID3D11SamplerState* get_sampler() const;
				void bind(unsigned int slot) const;
			};
		}
	}
}
#endif
#endif