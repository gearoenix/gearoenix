#ifndef GEAROENIX_DX11_TEXTURE_CUBE_HPP
#define GEAROENIX_DX11_TEXTURE_CUBE_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef USE_DIRECTX11
#include "../../render/texture/gx-rnd-txt-texture-cube.hpp"
#include <d3d11.h>
#include <memory>
namespace gearoenix {
namespace system {
    class File;
}
namespace dx11 {
    class Engine;
    namespace texture {
        class Cube : public render::texture::Cube {
        private:
            ID3D11ShaderResourceView* srv = nullptr;

        public:
            Cube(core::Id my_id, system::stream::Stream* file, Engine* engine, core::sync::EndCaller<core::sync::EndCallerIgnore> end);
            ~Cube();
            const ID3D11ShaderResourceView* get_shader_resource_view();
            void bind(unsigned int slot = 0) const;
        };
    }
}
}
#endif
#endif
