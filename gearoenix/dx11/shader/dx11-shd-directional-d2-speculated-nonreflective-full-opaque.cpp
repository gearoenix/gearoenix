#include "dx11-shd-directional-d2-speculated-nonreflective-full-opaque.hpp"
#ifdef USE_DIRECTX11
#include "../../core/cr-static.hpp"
#include "../../system/sys-log.hpp"
#include "../buffer/dx11-buf-uniform.hpp"
#include "../dx11-engine.hpp"
#include "../pipeline/dx11-pip-pipeline.hpp"
#include "../texture/dx11-txt-2d.hpp"
#include "../texture/dx11-txt-sampler.hpp"

gearoenix::dx11::shader::DirectionalD2SpeculatedNonreflectiveFullOpaque::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : render::material::DirectionalD2SpeculatedNonreflectiveFullOpaque::Resources(e, pip, u)
{
}

void gearoenix::dx11::shader::DirectionalD2SpeculatedNonreflectiveFullOpaque::Resources::bind()
{
    buffer::Uniform* uniform = reinterpret_cast<buffer::Uniform*>(u);
    DirectionalD2SpeculatedNonreflectiveFullOpaque* shd = reinterpret_cast<DirectionalD2SpeculatedNonreflectiveFullOpaque*>(pip->get_shader());
    uniform->set_for_vertex_shader();
    uniform->set_for_fragment_shader();
    reinterpret_cast<texture::Texture2D*>(txt)->bind(0);
    reinterpret_cast<texture::Texture2D*>(shdtxt)->bind(1);
    shd->use();
}

gearoenix::dx11::shader::DirectionalD2SpeculatedNonreflectiveFullOpaque::DirectionalD2SpeculatedNonreflectiveFullOpaque(core::Id my_id, Engine* eng, core::EndCaller<core::EndCallerIgnore> end)
    : Shader(my_id, eng, end)
{
    eng->add_load_function([this, end] {
        const char pvs[] = "struct VertexInputType {\n"
                           "    float3 position : POSITION;\n"
                           "    float3 normal   : NORMAL;\n"
                           "    float2 texcoord : TEXCOORD;\n"
                           "};\n"
                           "struct PixelInputType {\n"
                           "    float4 position : SV_POSITION;\n"
                           "    float2 texcoord : TEXCOORD1;\n"
                           "	 float3 shadow   : TEXCOORD2;\n"
                           "    float diffuse   : DIFFUSE;\n"
                           "    float speculare : SPECULARE;\n"
                           "    float bias      : BIAS;\n"
                           "};\n"
                           "cbuffer UniformBuffer {\n"
                           "    float4 ambl_color;\n"
                           "    matrix db;\n"
                           "    float4 eye;\n"
                           "    matrix m;\n"
                           "    float4 spec_color;\n"
                           "    float4 spec_factors;\n"
                           "    float4 sun;\n"
                           "    float4 sun_color;\n"
                           "    matrix vp;\n"
                           "};\n"
                           "PixelInputType main(VertexInputType input) {\n"
                           "    PixelInputType output;\n"
                           "    float4 position = float4(input.position, 1.0);\n"
                           "    float4 world_position = mul(position, m);\n"
                           "    float3 world_normal = normalize(mul(float4(input.normal, 0.0), m).xyz);\n"
                           "    float3 reflected = reflect(sun.xyz, world_normal);\n"
                           "    output.texcoord = input.texcoord;\n"
                           "    output.diffuse = dot(sun.xyz, world_normal);\n"
                           "    output.speculare = dot(normalize(world_position.xyz - eye.xyz), reflected);\n"
                           "    output.shadow = mul(world_position, db).xyz;\n"
                           "    output.bias = clamp(0.005 * tan(acos(abs(output.diffuse))), 0.0, 0.01);\n"
                           "    output.position = mul(world_position, vp);\n"
                           "    return output;\n"
                           "}\n";
        const char pfs[] = "struct PixelInputType {\n"
                           "    float4 position : SV_POSITION;\n"
                           "    float2 texcoord : TEXCOORD1;\n"
                           "	 float3 shadow   : TEXCOORD2;\n"
                           "    float diffuse   : DIFFUSE;\n"
                           "    float speculare : SPECULARE;\n"
                           "    float bias      : BIAS;\n"
                           "};\n"
                           "cbuffer UniformBuffer {\n"
                           "    float4 ambl_color;\n"
                           "    matrix db;\n"
                           "    float4 eye;\n"
                           "    matrix m;\n"
                           "    float4 spec_color;\n"
                           "    float4 spec_factors;\n"
                           "    float4 sun;\n"
                           "    float4 sun_color;\n"
                           "    matrix vp;\n"
                           "};\n"
                           "Texture2D txt    : register(t0);\n"
                           "Texture2D shdtxt : register(t1);\n"
                           "SamplerState smp;\n"
                           "float4 main(PixelInputType input) : SV_TARGET {\n"
                           "    float4 txt_color = txt.Sample(smp, input.texcoord);\n"
                           "    float4 ambl_light = float4(txt_color.xyz * ambl_color.xyz, txt_color.w);\n"
                           "    float diff_fac = smoothstep(0.2, 0.5, input.diffuse);\n"
                           "    if(diff_fac < 0.001) return ambl_light;\n"
                           "    if(input.shadow.x <= 0.0 || input.shadow.x >= 1.0 || input.shadow.y <= 0.0 || input.shadow.y >= 1.0) {\n"
                           "        float spec_fac = smoothstep(spec_factors.x, spec_factors.y, input.speculare) * spec_factors.z;\n"
                           "        float3 diff_color = (txt_color.xyz * diff_fac) * sun_color.xyz;\n"
                           "        float3 spc_color = spec_color.xyz * spec_fac;\n"
                           "        return float4(diff_color + ambl_light.xyz + spc_color, txt_color.w);\n"
                           "    }\n"
                           "    float d = shdtxt.Sample(smp, input.shadow.xy).x;\n"
                           "    if(d < input.shadow.z - input.bias) return ambl_light;\n"
                           "    float spec_fac = smoothstep(spec_factors.x, spec_factors.y, input.speculare) * spec_factors.z; \n"
                           "    float3 diff_color = (txt_color.xyz * diff_fac) * sun_color.xyz;\n"
                           "    float3 spc_color = spec_color.xyz * spec_fac;\n"
                           "    return float4(diff_color + ambl_light.xyz + spc_color, txt_color.w);\n"
                           "}\n";
        std::vector<D3D11_INPUT_ELEMENT_DESC> desc(3);
        GXSETZ(desc[0]);
        desc[0].SemanticName = "POSITION";
        desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        GXSETZ(desc[1]);
        desc[1].SemanticName = "NORMAL";
        desc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        desc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        desc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        GXSETZ(desc[2]);
        desc[2].SemanticName = "TEXCOORD";
        desc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
        desc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        desc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        compile_shader(pvs, render::shader::stage::VERTEX, desc);
        compile_shader(pfs, render::shader::stage::FRAGMENT);
        (void)end;
    });
}

gearoenix::dx11::shader::DirectionalD2SpeculatedNonreflectiveFullOpaque::~DirectionalD2SpeculatedNonreflectiveFullOpaque()
{
}

void gearoenix::dx11::shader::DirectionalD2SpeculatedNonreflectiveFullOpaque::use()
{
    run();
    Engine* engine = reinterpret_cast<Engine*>(eng);
    engine->get_sampler()->bind(0);
}

const std::vector<gearoenix::render::shader::stage::Id>& gearoenix::dx11::shader::DirectionalD2SpeculatedNonreflectiveFullOpaque::get_stages_ids() const
{
    return graphic_2_stage;
}
#endif
