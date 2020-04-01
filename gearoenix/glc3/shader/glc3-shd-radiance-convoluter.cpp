#include "glc3-shd-radiance-convoluter.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/cr-function-loader.hpp"
#include "../../gl/gl-loader.hpp"
#include "../engine/glc3-eng-engine.hpp"
#include <sstream>

gearoenix::glc3::shader::RadianceConvoluter::RadianceConvoluter(engine::Engine* const e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Shader(e, c)
{
    GX_GLC3_SHADER_SRC_DEFAULT_VERTEX_STARTING <<
        // output
        "out vec3 out_pos;\n"
        // Main function
        "void main()\n"
        "{\n"
        "    out_pos = normal;\n" // normal in here is world pos of the face vertex
        "    gl_Position = vec4(position, 1.0);\n"
        "}";
    GX_GLC3_SHADER_SRC_DEFAULT_FRAGMENT_STARTING <<
        // input
        "in vec3 out_pos;\n"
        "out vec4 frag_color;\n"
        "uniform samplerCube environment;\n"
        "uniform float roughness;\n"
        "uniform float roughness_p_4;\n"
        // TODO lots of performance improvement lies in here
        "float DistributionGGX(vec3 N, vec3 H) {\n"
        "    float NdotH = max(dot(N, H), 0.0);\n"
        "    float NdotH2 = NdotH*NdotH;\n"
        "    float nom   = roughness_p_4;\n"
        "    float denom = (NdotH2 * (roughness_p_4 - 1.0) + 1.0);\n"
        "    denom = GX_PI * denom * denom;\n"
        "    return nom / denom;\n"
        "}\n"
        "float RadicalInverse_VdC(uint bits) {\n"
        "    bits = (bits << 16u) | (bits >> 16u);\n"
        "    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);\n"
        "    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);\n"
        "    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);\n"
        "    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);\n"
        "    return float(bits) * 2.3283064365386963e-10; // / 0x100000000\n"
        "}\n"
        "vec2 Hammersley(uint i, uint N) {\n"
        "    return vec2(float(i)/float(N), RadicalInverse_VdC(i));\n"
        "}\n"
        "vec3 ImportanceSampleGGX(vec2 Xi, vec3 N) {\n"
        "    float phi = 2.0 * GX_PI * Xi.x;\n"
        "    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (roughness_p_4 - 1.0) * Xi.y));\n"
        "    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);\n"
        "    vec3 H;\n"
        "    H.x = cos(phi) * sinTheta;\n"
        "    H.y = sin(phi) * sinTheta;\n"
        "    H.z = cosTheta;\n"
        "    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);\n"
        "    vec3 tangent   = normalize(cross(up, N));\n"
        "    vec3 bitangent = cross(N, tangent);\n"
        "    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;\n"
        "    return normalize(sampleVec);\n"
        "}\n"
        "void main() {\n"
        "    vec3 N = normalize(out_pos);\n"
        "    vec3 R = N;\n"
        "    vec3 V = R;\n"
        "    const uint SAMPLE_COUNT = 1024u;\n"
        "    vec3 prefilteredColor = vec3(0.0);\n"
        "    float totalWeight = 0.0;\n"
        "    for(uint i = 0u; i < SAMPLE_COUNT; ++i) {\n"
        "        vec2 Xi = Hammersley(i, SAMPLE_COUNT);\n"
        "        vec3 H = ImportanceSampleGGX(Xi, N);\n"
        "        vec3 L  = normalize(2.0 * dot(V, H) * H - V);\n"
        "        float NdotL = max(dot(N, L), 0.0);\n"
        "        if(NdotL > 0.0) {\n"
        "            float D   = DistributionGGX(N, H);\n"
        "            float NdotH = max(dot(N, H), 0.0);\n"
        "            float HdotV = max(dot(H, V), 0.0);\n"
        "            float pdf = D * NdotH / (4.0 * HdotV) + 0.0001; \n"
        "            float resolution = 512.0; // resolution of source cubemap (per face)\n"
        "            float saTexel  = 4.0 * GX_PI / (6.0 * resolution * resolution);\n"
        "            float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);\n"
        "            float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel); \n"
        "            prefilteredColor += textureLod(environment, L, mipLevel).rgb * NdotL;\n"
        "            totalWeight      += NdotL;\n"
        "        }\n"
        "    }\n"
        "    prefilteredColor = prefilteredColor / totalWeight;\n"
        "    prefilteredColor.x = (prefilteredColor.x <= 0.0 || prefilteredColor.x >= 0.0)? prefilteredColor.x: 0.0;\n"
        "    prefilteredColor.y = (prefilteredColor.y <= 0.0 || prefilteredColor.y >= 0.0)? prefilteredColor.y: 0.0;\n"
        "    prefilteredColor.z = (prefilteredColor.z <= 0.0 || prefilteredColor.z >= 0.0)? prefilteredColor.z: 0.0;\n"
        "    frag_color = vec4(prefilteredColor, 1.0);\n"
        "}";
    e->get_function_loader()->load([this, vertex_shader_code { vertex_shader_code.str() }, fragment_shader_code { fragment_shader_code.str() }] {
        set_vertex_shader(vertex_shader_code);
        set_fragment_shader(fragment_shader_code);
        link();
        GX_GLC3_SHADER_SET_TEXTURE_INDEX_STARTING
        GX_GLC3_THIS_GET_UNIFORM(roughness)
        //        GX_GLC3_THIS_GET_UNIFORM(roughness_p_2)
        GX_GLC3_THIS_GET_UNIFORM(roughness_p_4)
        GX_GLC3_THIS_GET_UNIFORM_TEXTURE(environment)
    });
}

gearoenix::glc3::shader::RadianceConvoluter::~RadianceConvoluter() noexcept = default;

void gearoenix::glc3::shader::RadianceConvoluter::bind() const noexcept
{
    Shader::bind();
    GX_GLC3_SHADER_SET_TEXTURE_INDEX_UNIFORM(environment)
}

#endif
