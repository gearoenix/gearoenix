#include "gx-gl-shd-colour-tuning.hpp"
#if GX_RENDER_OPENGL_ENABLED

constexpr static auto vertex_shader_body = R"SHADER(
layout(location = 0) in vec2 position;

out vec2 out_uv;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    out_uv = position * 0.5 + 0.5;
}
)SHADER";

constexpr static auto fragment_shader_body = R"SHADER(
uniform sampler2D source_texture;
uniform sampler3D colour_lut;
uniform sampler2D blue_noise;
uniform float frame_index;
uniform float lut_size;
uniform int tonemap_mode;
uniform float agx_peak_ratio;
uniform vec2 noise_uv_scale;

in vec2 out_uv;

out vec4 frag_colour;

vec3 linear_to_srgb(vec3 c) {
    return mix(c * 12.92, 1.055 * pow(max(c, vec3(0.0)), vec3(1.0 / 2.4)) - 0.055, step(vec3(0.0031308), c));
}

// AgX (Sobotka) tonemap, inlined per-pixel. MUST MATCH gx-math-colour.hpp's reference.
vec3 agx_input_matrix(vec3 v) {
    return vec3(
        0.842479062253094 * v.r + 0.0784335999999992 * v.g + 0.0792237451477643 * v.b,
        0.0423282422610123 * v.r + 0.878468636469772  * v.g + 0.0791661274605434 * v.b,
        0.0423756549057051 * v.r + 0.0784336          * v.g + 0.879142973793104  * v.b
    );
}

vec3 agx_log_encode(vec3 linear) {
    const float min_ev = -12.47393;
    const float max_ev = 4.026069;
    const float ev_range = max_ev - min_ev;
    return clamp((log2(max(linear, vec3(1e-10))) - min_ev) / ev_range, vec3(0.0), vec3(1.0));
}

vec3 agx_default_contrast(vec3 x) {
    vec3 x2 = x * x;
    vec3 x4 = x2 * x2;
    return 15.5 * x4 * x2
        - 40.14 * x4 * x
        + 31.96 * x4
        - 6.868 * x2 * x
        + 0.4298 * x2
        + 0.1191 * x
        - vec3(0.00232);
}

vec3 agx_tonemap(vec3 linear, float peak) {
    return agx_default_contrast(agx_log_encode(agx_input_matrix(linear))) * peak;
}

// Narkowicz ACES filmic fit -- 5-coefficient rational approximation, per-channel, SDR-only.
vec3 aces_narkowicz(vec3 x) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), vec3(0.0), vec3(1.0));
}

void main() {
    vec3 hdr = texture(source_texture, out_uv).rgb;

    vec3 graded;
    if (tonemap_mode == 0) {
        // Identity: scene-linear pass-through.
        graded = hdr;
    } else if (tonemap_mode == 1) {
        // AgX: matrix + log + sigmoid + peak_ratio, all inline.
        graded = agx_tonemap(max(hdr, vec3(0.0)), agx_peak_ratio);
    } else if (tonemap_mode == 2) {
        // ACES (Narkowicz): per-channel rational approximation, SDR-only.
        graded = aces_narkowicz(max(hdr, vec3(0.0)));
    } else {
        // Custom LUT: log-encode the HDR input, sample the 3D LUT.
        // MUST MATCH gx-rnd-txt-lut-encode.hpp.
        const float lut_log_grey = 0.18;
        const float lut_log_range = 14.0;
        const float lut_log_eps = 0.18 * 0.0078125;
        vec3 log_hdr = log2(max(hdr, vec3(lut_log_eps)) / lut_log_grey) / lut_log_range + 0.5;
        float scale = (lut_size - 1.0) / lut_size;
        float bias = 0.5 / lut_size;
        vec3 lut_uv = clamp(log_hdr, vec3(0.0), vec3(1.0)) * scale + bias;
        graded = texture(colour_lut, lut_uv).rgb;
    }

    // GL backend always pins SDR (no HDR swapchain on GLES/WebGL2), so apply sRGB OETF unconditionally.
    vec3 encoded = linear_to_srgb(clamp(graded, vec3(0.0), vec3(1.0)));

    // Blue-noise dither to break 8-bit banding. Scrolled per frame using golden-ratio offset.
    vec2 frame_offset = vec2(frame_index * 0.61803398875, frame_index * 0.7548776662);
    vec2 noise_uv = (gl_FragCoord.xy + frame_offset) * noise_uv_scale;
    float noise = texture(blue_noise, noise_uv).r;
    float dither = (noise - 0.5) / 255.0;
    frag_colour = vec4(encoded + vec3(dither), 1.0);
}
)SHADER";

gearoenix::gl::shader::ColourTuning::ColourTuning()
{
    set_vertex_shader(get_common_shader_starter() + vertex_shader_body);
    set_fragment_shader(get_common_shader_starter() + fragment_shader_body);
    link();
    GX_GL_SHADER_SET_TEXTURE_INDEX_STARTING;
    GX_GL_THIS_GET_UNIFORM_TEXTURE(source_texture);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(colour_lut);
    GX_GL_THIS_GET_UNIFORM_TEXTURE(blue_noise);
    GX_GL_THIS_GET_UNIFORM(frame_index);
    GX_GL_THIS_GET_UNIFORM(lut_size);
    GX_GL_THIS_GET_UNIFORM(tonemap_mode);
    GX_GL_THIS_GET_UNIFORM(agx_peak_ratio);
    GX_GL_THIS_GET_UNIFORM(noise_uv_scale);
}

gearoenix::gl::shader::ColourTuning::~ColourTuning() = default;

void gearoenix::gl::shader::ColourTuning::bind(uint& current_shader) const
{
    if (shader_program == current_shader) {
        return;
    }
    Shader::bind(current_shader);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(source_texture);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(colour_lut);
    GX_GL_SHADER_SET_TEXTURE_INDEX_UNIFORM(blue_noise);
}

#endif
