#include "gx-dx11-txt-cube.hpp"
#ifdef USE_DIRECTX11
#include "../../core/gx-cr-static.hpp"
#include "../../render/texture/gx-rnd-txt-png.hpp"
#include "../../system/gx-sys-log.hpp"
#include "../../system/stream/gx-sys-stm-stream.hpp"
#include "../dx11-check.hpp"
#include "../dx11-engine.hpp"

#define FACES_COUNT 6

gearoenix::dx11::texture::Cube::Cube(core::Id my_id, system::stream::Stream* file, Engine* eng, core::sync::EndCaller<core::sync::EndCallerIgnore> end)
    : render::texture::Cube(my_id, eng)
{
    std::vector<std::vector<unsigned char>> img_data(FACES_COUNT);
    unsigned int imgw, imgh;
    render::texture::Image::decode(file, img_data[0], imgw, imgh);
    for (int i = 1; i < FACES_COUNT; ++i) {
        unsigned int tmpimgw, tmpimgh;
        render::texture::Image::decode(file, img_data[i], tmpimgw, tmpimgh);
        if (imgw != tmpimgw || imgw != tmpimgh) {
            UNEXPECTED;
        }
    }
    D3D11_TEXTURE2D_DESC desc;
    GX_SET_ZERO(desc);
    desc.Width = imgw;
    desc.Height = imgh;
    desc.MipLevels = 1;
    desc.ArraySize = FACES_COUNT;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
    D3D11_SHADER_RESOURCE_VIEW_DESC sdesc;
    GX_SET_ZERO(sdesc);
    sdesc.Format = desc.Format;
    sdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    sdesc.TextureCube.MipLevels = 1;
    eng->add_load_function([this, desc, sdesc, img_data, end]() -> void {
        ID3D11Device* dev = static_cast<Engine*>(render_engine)->get_device();
        ID3D11DeviceContext* ctx = static_cast<Engine*>(render_engine)->get_context();
        ID3D11Texture2D* txt = nullptr;
        D3D11_SUBRESOURCE_DATA facesdata[FACES_COUNT];
        GX_SET_ARRAY_ZERO(facesdata);
        facesdata[0].pSysMem = img_data[2].data();
        facesdata[0].SysMemPitch = desc.Width * 4;
        facesdata[1].pSysMem = img_data[3].data();
        facesdata[1].SysMemPitch = desc.Width * 4;
        facesdata[2].pSysMem = img_data[5].data();
        facesdata[2].SysMemPitch = desc.Width * 4;
        facesdata[3].pSysMem = img_data[4].data();
        facesdata[3].SysMemPitch = desc.Width * 4;
        facesdata[4].pSysMem = img_data[0].data();
        facesdata[4].SysMemPitch = desc.Width * 4;
        facesdata[5].pSysMem = img_data[1].data();
        facesdata[5].SysMemPitch = desc.Width * 4;
        GXHRCHK(dev->CreateTexture2D(&desc, facesdata, &txt));
        GXHRCHK(dev->CreateShaderResourceView(txt, &sdesc, &srv));
        txt->Release();
        (void)end;
    });
}

gearoenix::dx11::texture::Cube::~Cube()
{
    srv->Release();
}

const ID3D11ShaderResourceView* gearoenix::dx11::texture::Cube::get_shader_resource_view()
{
    return srv;
}

void gearoenix::dx11::texture::Cube::bind(unsigned int slot) const
{
    static_cast<Engine*>(render_engine)->get_context()->PSSetShaderResources(slot, 1, &srv);
}
#endif
