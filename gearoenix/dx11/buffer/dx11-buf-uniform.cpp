#include "dx11-buf-uniform.hpp"
#ifdef GX_USE_DIRECTX11
#include "../../core/cr-static.hpp"
#include "../../system/sys-log.hpp"
#include "../dx11-check.hpp"
#include "../dx11-engine.hpp"
#include <cstring>

gearoenix::dx11::buffer::Uniform::Uniform(unsigned int s, Engine* eng, core::sync::EndCaller<core::sync::EndCallerIgnore> c)
    : render::buffer::Uniform(eng)
{
    eng->add_load_function([this, eng, s, c]() -> void {
        D3D11_BUFFER_DESC desc;
        GX_SET_ZERO(desc);
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth = ((s & 15) == 0) ? s : (s & (~((UINT)15))) + 16;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        ID3D11Device* dev = eng->get_device();
        GXHRCHK(dev->CreateBuffer(&desc, nullptr, &ub));
        (void)c;
    });
}

gearoenix::dx11::buffer::Uniform::~Uniform()
{
    ub->Release();
    ub = nullptr;
}

void gearoenix::dx11::buffer::Uniform::update(const void* src, unsigned int size)
{
    ID3D11DeviceContext* ctx = static_cast<Engine*>(engine)->get_context();
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

void gearoenix::dx11::buffer::Uniform::set_for_vertex_shader()
{
    ID3D11DeviceContext* ctx = static_cast<Engine*>(engine)->get_context();
    ctx->VSSetConstantBuffers(0, 1, &ub);
}

void gearoenix::dx11::buffer::Uniform::set_for_fragment_shader()
{
    ID3D11DeviceContext* ctx = static_cast<Engine*>(engine)->get_context();
    ctx->PSSetConstantBuffers(0, 1, &ub);
}

#endif
