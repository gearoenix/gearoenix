#include "dx11-txt-cube.hpp"
#ifdef USE_DIRECTX11
#include "../../render/texture/rnd-txt-png.hpp"
#include "../../system/sys-file.hpp"
#include "../../system/sys-log.hpp"
#include "../dx11-engine.hpp"
#include "../../core/cr-static.hpp"
#include "../dx11-check.hpp"

#define FACES_COUNT 6

gearoenix::dx11::texture::Cube::Cube(system::File* file, Engine* eng, std::shared_ptr<core::EndCaller> end)
{
    std::vector<unsigned char> img_data;
	unsigned int imgw, imgh;
    std::vector<core::Offset> img_offs(5);
    for (int i = 0; i < 5; ++i) {
        file->read(img_offs[i]);
    }
    render::texture::PNG::decode(file, img_data, imgw, imgh);
    for (int i = 1; i < FACES_COUNT; ++i) {
        file->seek((unsigned int)img_offs[i - 1]);
		std::vector<unsigned char> tmp_img_data;
		unsigned int tmpimgw, tmpimgh;
        render::texture::PNG::decode(file, tmp_img_data, tmpimgw, tmpimgh);
		if (imgw != tmpimgw || imgw != tmpimgh || img_data.size() != tmp_img_data.size()) {
			UNEXPECTED;
		}
		img_data.insert(img_data.end(), tmp_img_data.begin(), tmp_img_data.end());
    }
	D3D11_TEXTURE2D_DESC desc;
	setz(desc);
	desc.Width = imgw;
	desc.Height = imgh;
	desc.ArraySize = FACES_COUNT;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;
	D3D11_SHADER_RESOURCE_VIEW_DESC sdesc;
	setz(sdesc);
	sdesc.Format = desc.Format;
	sdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	sdesc.TextureCube.MipLevels = -1;
    eng->add_load_function([&, eng, desc, sdesc, img_data, end]() -> void {
		D3D11_SUBRESOURCE_DATA subsrcdata;
		subsrcdata.pSysMem = img_data.data();
		subsrcdata.SysMemPitch = desc.Width * 4;
		ID3D11Device* dev = eng->get_device();
		ID3D11Texture2D* txt = nullptr;
		GXHRCHK(dev->CreateTexture2D(&desc, &subsrcdata, &txt));
		GXHRCHK(dev->CreateShaderResourceView(txt, &sdesc, &srv));
		eng->get_context()->GenerateMips(srv);
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

#endif
