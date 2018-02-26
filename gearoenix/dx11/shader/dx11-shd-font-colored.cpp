#include "dx11-shd-font-colored.hpp"
#ifdef USE_DIRECTX11
#include "../../core/cr-static.hpp"
#include "../../system/sys-log.hpp"
#include "../buffer/dx11-buf-uniform.hpp"
#include "../dx11-engine.hpp"
#include "../pipeline/dx11-pip-pipeline.hpp"
#include "../texture/dx11-txt-2d.hpp"

gearoenix::dx11::shader::FontColored::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : render::material::FontColored::Resources(e, pip, u)
{
}

void gearoenix::dx11::shader::FontColored::Resources::bind()
{
    buffer::Uniform* uniform = reinterpret_cast<buffer::Uniform*>(u);
    FontColored* shd = reinterpret_cast<FontColored*>(pip->get_shader());
    uniform->set_for_vertex_shader();
    uniform->set_for_fragment_shader();
    reinterpret_cast<const texture::Texture2D*>(txt2d)->bind(0);
    shd->use();
}

gearoenix::dx11::shader::FontColored::FontColored(Engine* eng, core::EndCaller<core::EndCallerIgnore> end)
    : Shader(eng, end)
{
    eng->add_load_function([this, end] {
        const char p_vertex_src_data[] = "cbuffer UniformBuffer {\n"
                                         "    matrix mvp;\n"
                                         "    float4 color;\n"
                                         "};\n"
                                         "struct VertexInputType {\n"
                                         "    float3 position : POSITION;\n"
                                         "    float2 texcoord : TEXCOORD;\n"
                                         "};\n"
                                         "struct PixelInputType\n"
                                         "{\n"
                                         "    float4 position : SV_POSITION;\n"
                                         "    float2 texcoord : TEXCOORD;\n"
                                         "};\n"
                                         "PixelInputType main(VertexInputType input) {\n"
                                         "    PixelInputType output;\n"
                                         "    output.position = mul(float4(input.position, 1.0), mvp);\n"
                                         "    output.texcoord = input.texcoord;\n"
                                         "    return output;\n"
                                         "}\n";
        std::vector<D3D11_INPUT_ELEMENT_DESC> desc(2);
        GXSETZ(desc[0]);
        desc[0].SemanticName = "POSITION";
        desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        GXSETZ(desc[1]);
        desc[1].SemanticName = "TEXCOORD";
        desc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
        desc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        desc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        compile_shader(p_vertex_src_data, render::shader::stage::VERTEX, desc);
        const char p_fragment_src_data[] = "Texture2D txt;\n"
                                           "SamplerState smp;\n"
                                           "struct PixelInputType {\n"
                                           "    float4 position : SV_POSITION;\n"
                                           "    float2 texcoord : TEXCOORD;\n"
                                           "};\n"
                                           "cbuffer UniformBuffer {\n"
                                           "    matrix mvp;\n"
                                           "    float4 color;\n"
                                           "};\n"
                                           "float4 main(PixelInputType input) : SV_TARGET {\n"
                                           "   float4 txt_color = txt.Sample(smp, input.texcoord);\n"
                                           "	return float4(txt_color.xyz * color.xyz, txt_color.x * color.w);\n"
                                           "}\n";
        compile_shader(p_fragment_src_data, render::shader::stage::FRAGMENT);
        (void)end;
    });
}

gearoenix::dx11::shader::FontColored::~FontColored()
{
}

void gearoenix::dx11::shader::FontColored::use()
{
    run();
}

const std::vector<gearoenix::render::shader::stage::Id>& gearoenix::dx11::shader::FontColored::get_stages_ids() const
{
    return graphic_2_stage;
}
#endif
