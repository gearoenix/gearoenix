#include "dx11-buf-uniform.hpp"
#ifdef USE_DIRECTX11
#include "../../core/cr-static.hpp"
#include "../../system/sys-log.hpp"
#include "../dx11-engine.hpp"
#include "../dx11-check.hpp"
#include <cstring>

gearoenix::dx11::buffer::Uniform::Uniform(unsigned int s, Engine* eng)
    : render::buffer::Uniform(eng)
{
    D3D11_BUFFER_DESC desc;
    setz(desc);
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.ByteWidth = s;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GXHRCHK(eng->get_device()->CreateBuffer(&desc, NULL, &ub));
}

gearoenix::dx11::buffer::Uniform::~Uniform()
{
    ub->Release();
    ub = nullptr;
}

void gearoenix::dx11::buffer::Uniform::update(const void* src, unsigned int size)
{
    ID3D11DeviceContext* ctx = reinterpret_cast<Engine*>(engine)->get_context();
    D3D11_MAPPED_SUBRESOURCE mapped_resource;
    GXHRCHK(ctx->Map(ub, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource));
    std::memcpy(mapped_resource.pData, src, size);
    ctx->Unmap(ub, 0);
}

void* gearoenix::dx11::buffer::Uniform::get_data()
{
    // this should not be called because, 
	// it does/can not expose its data, 
	// it can temporary map and then must unmap its data immidiately.
    UNEXPECTED;
    return nullptr;
}

#endif
