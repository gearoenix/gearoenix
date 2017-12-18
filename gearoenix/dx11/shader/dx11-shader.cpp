#include "dx11-shader.hpp"
#ifdef USE_DIRECTX11
#include "../../system/sys-log.hpp"
#include "../dx11-engine.hpp"
#include "../dx11-check.hpp"
#include <d3dcompiler.h>

void gearoenix::dx11::shader::Shader::run()
{
	ID3D11DeviceContext* ctx = eng->get_context();
	ctx->IASetInputLayout(inlay);
	ctx->VSSetShader(vtxshd, NULL, 0);
	ctx->PSSetShader(frgshd, NULL, 0);
}

void* gearoenix::dx11::shader::Shader::compile_shader(
	const std::string& shd, 
	const render::shader::stage::Id& shader_typ,
	std::vector<D3D11_INPUT_ELEMENT_DESC> polygon_layout)
{
	ID3DBlob* shdcd = nullptr;
	ID3DBlob* shderr = nullptr;
	std::string version;
	switch (shader_typ)
	{
	case render::shader::stage::Id::VERTEX:
		version = "vs_5_0";
		break;
	case render::shader::stage::Id::FRAGMENT:
		version = "ps_5_0";
		break;
	default:
		UNEXPECTED;
		break;
	}
	if (FAILED(D3DCompile2(
		shd.c_str(), shd.size(),
		nullptr, nullptr, nullptr,
		"main", version.c_str(),
		D3DCOMPILE_OPTIMIZATION_LEVEL3 | 
		D3DCOMPILE_WARNINGS_ARE_ERRORS | 
		D3DCOMPILE_ENABLE_STRICTNESS | 
		D3DCOMPILE_PACK_MATRIX_ROW_MAJOR,
		0, 0, nullptr, 0, &shdcd, &shderr))){
		GXLOGF("Error in compiling shader " << ((const char*)(shderr->GetBufferPointer())));
	}
	switch (shader_typ)
	{
	case render::shader::stage::Id::VERTEX:
		GXHRCHK(eng->get_device()->CreateVertexShader(
			shdcd->GetBufferPointer(), shdcd->GetBufferSize(), NULL, &vtxshd));
		GXHRCHK(eng->get_device()->CreateInputLayout(
			polygon_layout.data(),
			(UINT) polygon_layout.size(),
			shdcd->GetBufferPointer(),
			shdcd->GetBufferSize(),
			&inlay));
		break;
	case render::shader::stage::Id::FRAGMENT:
		GXHRCHK(eng->get_device()->CreatePixelShader(shdcd->GetBufferPointer(), shdcd->GetBufferSize(), NULL, &frgshd));
		break;
	default:
		// todo add support for other shaders and then return ther pointer
		UNEXPECTED;
		break;
	}
	shdcd->Release();
	shderr->Release();
	return nullptr;
}

gearoenix::dx11::shader::Shader::Shader(Engine* eng, std::shared_ptr<core::EndCaller>)
    : eng(eng)
{

}

gearoenix::dx11::shader::Shader::~Shader() {
	inlay->Release();
	frgshd->Release();
	vtxshd->Release();
	vtxshd = nullptr;
	frgshd = nullptr;
	inlay = nullptr;
	eng = nullptr;
}

#endif
