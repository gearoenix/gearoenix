#include "dx11-txt-2d.hpp"
#ifdef USE_DIRECTX11
#include "../../core/cr-static.hpp"
#include "../../render/texture/rnd-txt-png.hpp"
#include "../../system/sys-log.hpp"
#include "../dx11-check.hpp"
#include "../dx11-engine.hpp"

gearoenix::dx11::texture::Texture2D::Texture2D(core::Id my_id, system::stream::Stream* file, Engine* eng, core::sync::EndCaller<core::sync::EndCallerIgnore> end)
    : render::texture::Texture2D(my_id, eng)
{
    std::vector<unsigned char> img_data;
    unsigned int imgw, imgh;
    render::texture::PNG::decode(file, img_data, imgw, imgh);
    D3D11_TEXTURE2D_DESC desc;
    GXSETZ(desc);
    desc.Width = imgw;
    desc.Height = imgh;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
    D3D11_SHADER_RESOURCE_VIEW_DESC sdesc;
    GXSETZ(sdesc);
    sdesc.Format = desc.Format;
    sdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    sdesc.Texture2D.MipLevels = -1;
    eng->add_load_function([this, desc, sdesc, img_data, end]() -> void {
        ID3D11Device* dev = static_cast<Engine*>(render_engine)->get_device();
        ID3D11Texture2D* txt = nullptr;
        GXHRCHK(dev->CreateTexture2D(&desc, nullptr, &txt));
        static_cast<Engine*>(render_engine)->get_context()->UpdateSubresource(txt, 0, nullptr, img_data.data(), desc.Width * 4, 0);
        GXHRCHK(dev->CreateShaderResourceView(txt, &sdesc, &srv));
        static_cast<Engine*>(render_engine)->get_context()->GenerateMips(srv);
        txt->Release();
        (void)end;
    });
}

gearoenix::dx11::texture::Texture2D::Texture2D(core::Id my_id, Engine* eng, ID3D11ShaderResourceView* srv)
    : render::texture::Texture2D(my_id, eng)
    , srv(srv)
{
}

gearoenix::dx11::texture::Texture2D::~Texture2D()
{
    srv->Release();
    srv = nullptr;
}

const ID3D11ShaderResourceView* gearoenix::dx11::texture::Texture2D::get_shader_resource_view() const
{
    return srv;
}

void gearoenix::dx11::texture::Texture2D::bind(unsigned int slot) const
{
    static_cast<Engine*>(render_engine)->get_context()->PSSetShaderResources(slot, 1, &srv);
}
#endif
