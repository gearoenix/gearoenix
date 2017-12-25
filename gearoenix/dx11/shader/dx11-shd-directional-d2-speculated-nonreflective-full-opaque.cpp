#include "dx11-shd-directional-d2-speculated-nonreflective-full-opaque.hpp"
#ifdef USE_DIRECTX11
#include "../../system/sys-log.hpp"
#include "../buffer/dx11-buf-uniform.hpp"
#include "../dx11-engine.hpp"
#include "../pipeline/dx11-pip-pipeline.hpp"
#include "../texture/dx11-txt-2d.hpp"

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

gearoenix::dx11::shader::DirectionalD2SpeculatedNonreflectiveFullOpaque::DirectionalD2SpeculatedNonreflectiveFullOpaque(Engine* eng, std::shared_ptr<core::EndCaller> end)
    : Shader(eng, end)
{
    eng->add_load_function([this, end] {
		const char pvs[] =
			"struct VertexInputType {\n"
			"    float3 position : POSITION;\n"
			"    float3 normal   : NORMAL;\n"
			"    float2 texcoord : TEXCOORD0;\n"
			"};\n"
			"struct PixelInputType {\n"
			"    float4 position : SV_POSITION;\n"
			"    float2 texcoord : TEXCOORD1;\n"
			"	 float3 shadow   : TEXCOORD2;\n"
			"    float diffuse   : DIFFUSE;\n"
			"    float speculare : SPECULARE;\n"
			"    float bias      : BIAS;\n"
			"}\n"
			"cbuffer UniformBuffer {\n"
			"    matrix vp;\n"
			"    matrix m;\n"
			"    matrix db;\n"
			"    float3 sun;\n"
			"    float3 eye;\n"
			"    float3 sun_color;\n"
			"    float3 spec_color;\n"
			"    float3 spec_factors;\n"
			"    float3 ambl_color;\n"
			"}\n"
			"PixelInputType main(VertexInputType input) {\n"
			"    PixelInputType output;\n"
			"    float4 position = float4(input.position, 1.0);\n"
			"    float4 world_position = mul(position, m);\n"
			"    float3 world_normal = normalize(mul(float4(normal, 0.0), m).xyz);\n"
			"    float3 reflected = reflect(sun, world_normal);\n"
			"    output.texcoord = input.texcoord;\n"
			"    output.diffuse = dot(sun, world_normal);\n"
			"    output.speculare = dot(normalize(world_position.xyz - eye), reflected);\n"
			"    output.shadow = mul(world_position, db).xyz;\n"
			"    output.bias = clamp(0.005 * tan(acos(abs(output.diffuse))), 0.0, 0.01);\n"
			"    output.position = mul(world_position, vp);\n"
			"    return output;\n"
			"}\n";
        const std::string pfs = "precision highp sampler2D;\n"
                                "precision highp float;\n"
                                "varying vec2 out_uv;\n"
                                "varying vec3 out_shd;\n"
                                "varying float out_diffuse;\n"
                                "varying float out_speculare;\n"
                                "varying float out_bias;\n"
                                
                                "uniform sampler2D txt;\n"
                                "uniform sampler2D shdtxt;\n"
                                "void main()\n"
                                "{\n"
                                "    vec3 txt_color = texture2D(txt, out_uv).xyz;\n"
                                "    vec3 ambl_light = txt_color * ambl_color;\n"
                                "    float diff_fac = smoothstep(0.2, 0.5, out_diffuse);\n"
                                "    if(diff_fac < 0.001)\n"
                                "    {\n"
                                "        gl_FragColor = vec4(ambl_light, 1.0);\n"
                                "    }\n"
                                "    else\n"
                                "    {\n"
                                "        vec4 v = texture2D(shdtxt, out_shd.xy);\n"
                                "        float d = v.y;\n"
                                "        d /= 256.0;\n"
                                "        d += v.x;\n"
                                "        if(d < out_shd.z - out_bias)\n"
                                "        {\n"
                                "            gl_FragColor = vec4(ambl_light, 1.0);\n"
                                "        }\n"
                                "        else\n"
                                "        {\n"
                                "            float spec_fac = smoothstep(spec_factors[0], spec_factors[1], out_speculare) * spec_factors[2];\n"
                                "            vec3 diff_color = txt_color * diff_fac * sun_color;\n"
                                "            vec3 spc_color = spec_color * spec_fac;\n"
                                "            gl_FragColor = vec4(diff_color + ambl_light + spc_color, 1.0);\n"
                                "        }\n"
                                "    }\n"
                                "}\n";
        vtx_shd = add_shader_to_program(pvs, GL_VERTEX_SHADER);
        frg_shd = add_shader_to_program(pfs, GL_FRAGMENT_SHADER);
        link();

        glUseProgram(shader_program);

        vtx_att_ind = glGetAttribLocation(shader_program, "vertex");
        nrm_att_ind = glGetAttribLocation(shader_program, "normal");
        uv_att_ind = glGetAttribLocation(shader_program, "uv");

        ambl_color = get_uniform_location("ambl_color");
        db = get_uniform_location("db");
        eye = get_uniform_location("eye");
        m = get_uniform_location("m");
        spec_color = get_uniform_location("spec_color");
        spec_factors = get_uniform_location("spec_factors");
        sun = get_uniform_location("sun");
        sun_color = get_uniform_location("sun_color");
        vp = get_uniform_location("vp");

        txt = get_uniform_location("txt");
        shdtxt = get_uniform_location("shdtxt");

        glUniform1i(txt, 0);
        glUniform1i(shdtxt, 1);

        validate();
        (void)end;
    });
}

gearoenix::dx11::shader::DirectionalD2SpeculatedNonreflectiveFullOpaque::~DirectionalD2SpeculatedNonreflectiveFullOpaque()
{
    eng->add_load_function([this] {
        end_object(vtx_shd);
        end_object(frg_shd);
        end_program();
    });
}

void gearoenix::dx11::shader::DirectionalD2SpeculatedNonreflectiveFullOpaque::use()
{
    glUseProgram(shader_program);
    glEnableVertexAttribArray(vtx_att_ind);
    glEnableVertexAttribArray(nrm_att_ind);
    glEnableVertexAttribArray(uv_att_ind);
    ////////////////////////////////////////////////////////////
    glVertexAttribPointer(vtx_att_ind, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
    glVertexAttribPointer(nrm_att_ind, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glVertexAttribPointer(uv_att_ind, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    glUniform1i(txt, 0);
    glUniform1i(shdtxt, 1);
}

const std::vector<gearoenix::render::shader::stage::Id>& gearoenix::dx11::shader::DirectionalD2SpeculatedNonreflectiveFullOpaque::get_stages_ids() const
{
    return graphic_2_stage;
}
#endif
