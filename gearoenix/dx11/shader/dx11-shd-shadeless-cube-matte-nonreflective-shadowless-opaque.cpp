#include "dx11-shd-shadeless-cube-matte-nonreflective-shadowless-opaque.hpp"
#ifdef USE_DIRECTX11
#include "../../core/cr-static.hpp"
#include "../../system/sys-log.hpp"
#include "../buffer/dx11-buf-uniform.hpp"
#include "../dx11-engine.hpp"
#include "../pipeline/dx11-pip-pipeline.hpp"
#include "../texture/dx11-txt-cube.hpp"
#include "../texture/dx11-txt-sampler.hpp"

gearoenix::dx11::shader::ShadelessCubeMatteNonreflectiveShadowlessOpaque::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : render::material::ShadelessCubeMatteNonreflectiveShadowlessOpaque::Resources(e, pip, u)
{
}

void gearoenix::dx11::shader::ShadelessCubeMatteNonreflectiveShadowlessOpaque::Resources::bind()
{
    buffer::Uniform* uniform = reinterpret_cast<buffer::Uniform*>(u);
    ShadelessCubeMatteNonreflectiveShadowlessOpaque* shd = reinterpret_cast<ShadelessCubeMatteNonreflectiveShadowlessOpaque*>(pip->get_shader());
    uniform->set_for_vertex_shader();
    reinterpret_cast<const texture::Cube*>(ctxt)->bind(0);
    shd->use();
}

gearoenix::dx11::shader::ShadelessCubeMatteNonreflectiveShadowlessOpaque::ShadelessCubeMatteNonreflectiveShadowlessOpaque(Engine* eng, core::EndCaller<core::EndCallerIgnore> end)
    : Shader(eng, end)
{
    eng->add_load_function([this, eng, end]() -> void {
        // todo Shader compilation must move to blender part.
        const char p_vertex_src_data[] = "cbuffer UniformBuffer {\n"
                                         "    matrix m;\n"
                                         "    matrix mvp;\n"
                                         "};\n"
                                         "struct VertexInputType {\n"
                                         "    float3 position : POSITION;\n"
                                         "};\n"
                                         "struct PixelInputType\n"
                                         "{\n"
                                         "    float4 position : SV_POSITION;\n"
                                         "    float3 texcoord : TEXCOORD;\n"
                                         "};\n"
                                         "PixelInputType main(VertexInputType input) {\n"
                                         "    PixelInputType output;\n"
                                         "    output.position = mul(float4(input.position, 1.0), mvp);\n"
                                         "    output.texcoord = mul(float4(input.position, 0.0), m).xyz;\n"
                                         "    return output;\n"
                                         "}\n";
        std::vector<D3D11_INPUT_ELEMENT_DESC> desc(1);
        GXSETZ(desc[0]);
        desc[0].SemanticName = "POSITION";
        desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        compile_shader(p_vertex_src_data, render::shader::stage::VERTEX, desc);
        // todo Shader compilation must move to blender part.
        const char p_fragment_src_data[] = "TextureCube txt;\n"
                                           "SamplerState smp;\n"
                                           "struct PixelInputType {\n"
                                           "    float4 position : SV_POSITION;\n"
                                           "    float3 texcoord : TEXCOORD;\n"
                                           "};\n"
                                           "float4 main(PixelInputType input) : SV_TARGET {\n"
                                           "	return txt.Sample(smp, input.texcoord);\n"
                                           "}\n";
        compile_shader(p_fragment_src_data, render::shader::stage::FRAGMENT);
        (void)end;
    });
}

gearoenix::dx11::shader::ShadelessCubeMatteNonreflectiveShadowlessOpaque::~ShadelessCubeMatteNonreflectiveShadowlessOpaque()
{
}

void gearoenix::dx11::shader::ShadelessCubeMatteNonreflectiveShadowlessOpaque::use()
{
    run();
    Engine* engine = reinterpret_cast<Engine*>(eng);
    engine->get_sampler()->bind(0);
}

const std::vector<gearoenix::render::shader::stage::Id>& gearoenix::dx11::shader::ShadelessCubeMatteNonreflectiveShadowlessOpaque::get_stages_ids() const
{
    return graphic_2_stage;
}
#endif
