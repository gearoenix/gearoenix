#ifndef GEAROENIX_DX11_TEXTURE_CUBE_HPP
#define GEAROENIX_DX11_TEXTURE_CUBE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_DIRECTX11
#include "../../render/texture/rnd-txt-texture-cube.hpp"
#include <d3d11.h>
#include <memory>
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
        class Cube : public render::texture::Cube {
        private:
            Engine* engine = nullptr;
            ID3D11ShaderResourceView* srv = nullptr;

        public:
            Cube(system::File* file, Engine* engine, std::shared_ptr<core::EndCaller> end);
            ~Cube();
            const ID3D11ShaderResourceView* get_shader_resource_view();
            void bind(unsigned int slot = 0) const;
        };
    }
}
}
#endif
#endif
