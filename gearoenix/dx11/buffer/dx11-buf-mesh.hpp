#ifndef GEAROENIX_DX11_BUFFER_MESH_HPP
#define GEAROENIX_DX11_BUFFER_MESH_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_DIRECTX11
#include "../../core/cr-end-caller.hpp"
#include "../../render/buffer/rnd-buf-mesh.hpp"
#include <d3d11.h>
namespace gearoenix {
namespace dx11 {
    class Engine;
    namespace buffer {
        class Mesh : public render::buffer::Mesh {
        public:
        private:
            ID3D11Buffer* vb = nullptr;
            ID3D11Buffer* ib = nullptr;
            unsigned int ic = 0;
            const unsigned int stride;

        public:
            Mesh(
                unsigned int vertex_elements_count,
                system::File* f, Engine* e,
				core::EndCaller<core::EndCallerIgnore> c);
            ~Mesh();
            void bind();
            void draw();
        };
    } // namespace buffer
} // namespace dx11
} // namespace gearoenix
#endif
#endif // GEAROENIX_DX11_BUFFER_MESH_HPP
