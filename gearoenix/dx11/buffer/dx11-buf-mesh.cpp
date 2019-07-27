#include "dx11-buf-mesh.hpp"
#ifdef GX_USE_DIRECTX11
#include "../../core/cr-static.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"
#include "../dx11-check.hpp"
#include "../dx11-engine.hpp"

gearoenix::dx11::buffer::Mesh::Mesh(
    unsigned int vec, system::stream::Stream* f,
    Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> c)
    : render::buffer::Mesh(e)
    , stride(vec * sizeof(core::Real))
{
    core::Count cnt;
    f->read(cnt);
    core::Count vsec = cnt * vec;
    std::vector<core::Real> vd((size_t)vsec);
    unsigned int vs = (unsigned int)(vsec * sizeof(core::Real));
    for (core::Count i = 0; i < vsec; ++i) {
        f->read(vd[(size_t)i]);
    }
    f->read(cnt);
    ic = (unsigned int)cnt;
    std::vector<std::uint32_t> idata((size_t)cnt);
    for (core::Count i = 0; i < cnt; ++i)
        f->read(idata[(size_t)i]);
    unsigned int is = (unsigned int)(cnt * sizeof(std::uint32_t));
    ID3D11Device* dev = e->get_device();
    D3D11_BUFFER_DESC desc;
    D3D11_SUBRESOURCE_DATA buf_data;
    GX_SET_ZERO(desc);
    GX_SET_ZERO(buf_data);
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.ByteWidth = vs;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buf_data.pSysMem = vd.data();
    GXHRCHK(dev->CreateBuffer(&desc, &buf_data, &vb))
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.ByteWidth = is;
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    buf_data.pSysMem = idata.data();
    GXHRCHK(dev->CreateBuffer(&desc, &buf_data, &ib));
}

gearoenix::dx11::buffer::Mesh::~Mesh()
{
    vb->Release();
    ib->Release();
}

void gearoenix::dx11::buffer::Mesh::bind()
{
    const unsigned int offset = 0;
    ID3D11DeviceContext* ctx = static_cast<Engine*>(engine)->get_context();
    ctx->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
    ctx->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void gearoenix::dx11::buffer::Mesh::draw()
{
    static_cast<Engine*>(engine)->get_context()->DrawIndexed(ic, 0, 0);
}

#endif
