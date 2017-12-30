#ifndef GEAROENIX_DX11_TEXTURE_2D_HPP
#define GEAROENIX_DX11_TEXTURE_2D_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_DIRECTX11
#include "../../render/texture/rnd-txt-texture-2d.hpp"
#include <d3d11.h>
#include <memory>
namespace gearoenix {
namespace system {
    class File;
}
namespace dx11 {
    class Engine;
    namespace texture {
        class Texture2D : public render::texture::Texture2D {
        private:
            Engine* engine = nullptr;
            ID3D11ShaderResourceView* srv = nullptr;

        public:
            Texture2D(_Notnull_ system::File* file, _Notnull_ Engine* engine, core::EndCaller<core::EndCallerIgnore> end);
            Texture2D(_Notnull_ Engine* engine, _Notnull_ ID3D11ShaderResourceView* srv);
            ~Texture2D();
            const ID3D11ShaderResourceView* get_shader_resource_view() const;
            void bind(unsigned int slot = 0) const;
        };
    }
}
}
#endif
#endif
