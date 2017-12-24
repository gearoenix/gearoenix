#include "dx11-txt-sampler.hpp"
#ifdef USE_DIRECTX11
#include "../../core/cr-static.hpp"
#include "../dx11-check.hpp"
#include "../dx11-engine.hpp"

gearoenix::dx11::texture::Sampler::Sampler(Engine* engine)
    : engine(engine)
{
    D3D11_SAMPLER_DESC sampdes;
    GXSETZ(sampdes);
    sampdes.Filter = D3D11_FILTER_ANISOTROPIC;
    sampdes.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
    sampdes.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
    sampdes.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
    sampdes.MipLODBias = 0.0f;
    sampdes.MaxAnisotropy = 16;
    sampdes.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    sampdes.BorderColor[0] = 0.0f;
    sampdes.BorderColor[1] = 0.0f;
    sampdes.BorderColor[2] = 0.0f;
    sampdes.BorderColor[3] = 0.0f;
    sampdes.MinLOD = 0.0f;
    sampdes.MaxLOD = D3D11_FLOAT32_MAX;
    engine->get_device()->CreateSamplerState(&sampdes, &sampler);
}

gearoenix::dx11::texture::Sampler::~Sampler()
{
    sampler->Release();
}

const ID3D11SamplerState* gearoenix::dx11::texture::Sampler::get_sampler() const
{
    return sampler;
}

void gearoenix::dx11::texture::Sampler::bind(unsigned int slot) const
{
    engine->get_context()->PSSetSamplers(slot, 1, &sampler);
}

#endif
