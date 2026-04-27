#include "gx-rnd-txt-cube-lut.hpp"
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../core/sync/gx-cr-sync-parallel-for.hpp"
#include "../../math/gx-math-half-float.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "gx-rnd-txt-lut-encode.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <cstring>
#include <string_view>
#include <system_error>

namespace {
[[nodiscard]] constexpr bool starts_with_ci(const std::string_view s, const std::string_view prefix)
{
    if (s.size() < prefix.size()) {
        return false;
    }
    for (std::size_t i = 0; i < prefix.size(); ++i) {
        const auto a = s[i];
        const auto b = prefix[i];
        const auto la = (a >= 'A' && a <= 'Z') ? static_cast<char>(a - 'A' + 'a') : a;
        const auto lb = (b >= 'A' && b <= 'Z') ? static_cast<char>(b - 'A' + 'a') : b;
        if (la != lb) {
            return false;
        }
    }
    return true;
}

[[nodiscard]] constexpr std::string_view trim(std::string_view s)
{
    while (!s.empty() && (s.front() == ' ' || s.front() == '\t' || s.front() == '\r')) {
        s.remove_prefix(1);
    }
    while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\r')) {
        s.remove_suffix(1);
    }
    return s;
}

/// Parse one float from `[p, end)`, advancing `p` past it. Skips leading whitespace.
/// Returns false (and leaves `p` unchanged) on parse failure.
[[nodiscard]] bool parse_float(const char*& p, const char* const end, float& out)
{
    while (p < end && (*p == ' ' || *p == '\t' || *p == '\r')) {
        ++p;
    }
    const auto result = std::from_chars(p, end, out);
    if (result.ec != std::errc()) {
        return false;
    }
    p = result.ptr;
    return true;
}
}

gearoenix::render::texture::CubeLutData gearoenix::render::texture::parse_cube_lut(const char* const text, const std::size_t size)
{
    GX_ASSERT_D(nullptr != text);

    constexpr std::string_view lut_size_keyword = "LUT_3D_SIZE";
    constexpr std::array ignored_keywords {
        std::string_view { "TITLE" },
        std::string_view { "DOMAIN_MIN" },
        std::string_view { "DOMAIN_MAX" },
        std::string_view { "LUT_1D_SIZE" },
        std::string_view { "LUT_3D_INPUT_RANGE" },
        std::string_view { "LUT_1D_INPUT_RANGE" },
    };

    CubeLutData out;
    std::uint16_t* dst = nullptr;
    std::size_t total_voxels = 0;
    std::size_t voxels_written = 0;
    constexpr std::uint16_t half_one = math::to_half(1.0f);

    const char* const text_end = text + size;
    const char* cursor = text;

    while (cursor < text_end) {
        // memchr is typically SIMD-vectorised in libc and beats a hand-rolled byte loop.
        const auto remaining = static_cast<std::size_t>(text_end - cursor);
        const auto* const newline = static_cast<const char*>(std::memchr(cursor, '\n', remaining));
        const char* const line_end = newline ? newline : text_end;
        std::string_view line(cursor, static_cast<std::size_t>(line_end - cursor));
        cursor = newline ? newline + 1 : text_end;

        line = trim(line);
        if (line.empty() || line.front() == '#') {
            continue;
        }

        // Hot-path fast-out: data lines start with [0-9.+-]; keyword lines never do. Skip the
        // keyword comparisons entirely for the ~N^3 data lines.
        const auto first = line.front();
        const bool is_data = (first >= '0' && first <= '9') || first == '+' || first == '-' || first == '.';
        if (!is_data) {
            if (starts_with_ci(line, lut_size_keyword)) {
                line.remove_prefix(lut_size_keyword.size());
                line = trim(line);
                std::uint32_t parsed = 0;
                const auto result = std::from_chars(line.data(), line.data() + line.size(), parsed);
                if (result.ec != std::errc() || parsed < 2 || parsed > 256) {
                    GX_LOG_F("LUT_3D_SIZE out of supported range [2, 256]: " << line);
                }
                out.size = parsed;
                total_voxels = static_cast<std::size_t>(parsed) * parsed * parsed;
                out.pixels_rgba16f.resize(total_voxels * 4 * sizeof(std::uint16_t));
                dst = reinterpret_cast<std::uint16_t*>(out.pixels_rgba16f.data());
                continue;
            }
            // DOMAIN_MIN/MAX are accepted but not honoured: the engine assumes the standard
            // [0,1]^3 input domain. Non-default domains will produce wrong colours.
            for (const auto& kw : ignored_keywords) {
                if (starts_with_ci(line, kw)) {
                    goto next_line;
                }
            }
            // Unknown header keyword: silently ignore to be permissive about future spec extensions.
        next_line:
            continue;
        }

        if (dst == nullptr) {
            GX_LOG_F(".cube file has data lines before LUT_3D_SIZE; cannot parse.");
        }
        if (voxels_written >= total_voxels) {
            GX_LOG_F(".cube file has more data lines than LUT_3D_SIZE * 3 indicates.");
        }

        const char* p = line.data();
        const char* const p_end = line.data() + line.size();
        float r = 0.0f;
        float g = 0.0f;
        float b = 0.0f;
        if (!parse_float(p, p_end, r) || !parse_float(p, p_end, g) || !parse_float(p, p_end, b)) {
            GX_LOG_F(".cube data line malformed: " << line);
        }

        const auto base = voxels_written * 4;
        dst[base + 0] = math::to_half(r);
        dst[base + 1] = math::to_half(g);
        dst[base + 2] = math::to_half(b);
        dst[base + 3] = half_one;
        ++voxels_written;
    }

    if (out.size == 0) {
        GX_LOG_F(".cube file missing LUT_3D_SIZE");
    }
    if (voxels_written != total_voxels) {
        GX_LOG_F(".cube file has " << voxels_written << " entries, expected " << total_voxels);
    }

    return out;
}

namespace {
struct Vec3 final {
    float r, g, b;
};
[[nodiscard]] Vec3 sample_linear_cube_trilinear(const std::uint16_t* const halves, const std::uint32_t n, const float fx, const float fy, const float fz)
{
    // halves layout: R varies fastest, then G, then B (matches `.cube` on-disk order).
    const auto last = static_cast<int>(n - 1);
    const auto sample_at = [&](int xi, int yi, int zi) -> Vec3 {
        xi = std::clamp(xi, 0, last);
        yi = std::clamp(yi, 0, last);
        zi = std::clamp(zi, 0, last);
        const auto base = (static_cast<std::size_t>(zi) * n * n + static_cast<std::size_t>(yi) * n + static_cast<std::size_t>(xi)) * 4;
        return Vec3 { gearoenix::math::from_half(halves[base + 0]), gearoenix::math::from_half(halves[base + 1]), gearoenix::math::from_half(halves[base + 2]) };
    };
    const float coord_x = std::clamp(fx, 0.0f, 1.0f) * static_cast<float>(last);
    const float coord_y = std::clamp(fy, 0.0f, 1.0f) * static_cast<float>(last);
    const float coord_z = std::clamp(fz, 0.0f, 1.0f) * static_cast<float>(last);
    const int x0 = static_cast<int>(coord_x);
    const int y0 = static_cast<int>(coord_y);
    const int z0 = static_cast<int>(coord_z);
    const float tx = coord_x - static_cast<float>(x0);
    const float ty = coord_y - static_cast<float>(y0);
    const float tz = coord_z - static_cast<float>(z0);
    const auto lerp = [](const float a, const float b, const float t) { return a + (b - a) * t; };
    const auto lerp3 = [&](const Vec3 a, const Vec3 b, const float t) { return Vec3 { lerp(a.r, b.r, t), lerp(a.g, b.g, t), lerp(a.b, b.b, t) }; };
    const auto c000 = sample_at(x0, y0, z0);
    const auto c100 = sample_at(x0 + 1, y0, z0);
    const auto c010 = sample_at(x0, y0 + 1, z0);
    const auto c110 = sample_at(x0 + 1, y0 + 1, z0);
    const auto c001 = sample_at(x0, y0, z0 + 1);
    const auto c101 = sample_at(x0 + 1, y0, z0 + 1);
    const auto c011 = sample_at(x0, y0 + 1, z0 + 1);
    const auto c111 = sample_at(x0 + 1, y0 + 1, z0 + 1);
    const auto c00 = lerp3(c000, c100, tx);
    const auto c10 = lerp3(c010, c110, tx);
    const auto c01 = lerp3(c001, c101, tx);
    const auto c11 = lerp3(c011, c111, tx);
    const auto c0 = lerp3(c00, c10, ty);
    const auto c1 = lerp3(c01, c11, ty);
    return lerp3(c0, c1, tz);
}

struct LutVoxel final {
    std::uint16_t r, g, b, a;
};
}

gearoenix::render::texture::CubeLutData gearoenix::render::texture::resample_cube_to_log(
    const CubeLutData& src,
    const CubeInputLogEncoding log_encoding,
    const std::uint32_t target_size)
{
    GX_ASSERT_D(src.size >= 2);
    GX_ASSERT_D(target_size >= 2);

    const auto voxel_count = static_cast<std::size_t>(target_size) * target_size * target_size;
    std::vector<LutVoxel> voxels(voxel_count);
    const auto* const src_halves = reinterpret_cast<const std::uint16_t*>(src.pixels_rgba16f.data());
    const auto src_n = src.size;
    const auto target_inv = 1.0f / static_cast<float>(target_size - 1);
    constexpr auto half_one = math::to_half(1.0f);
    // Source already log-encoded with our range -> axis matches; sample directly.
    const bool passthrough = log_encoding == CubeInputLogEncoding::EngineRange;

    core::sync::parallel_for_i(voxels, [&](LutVoxel& voxel, const std::uint32_t i, auto) {
        const auto i_r = static_cast<std::uint32_t>(i % target_size);
        const auto i_g = static_cast<std::uint32_t>((i / target_size) % target_size);
        const auto i_b = static_cast<std::uint32_t>(i / (target_size * target_size));
        const auto log_in_r = static_cast<float>(i_r) * target_inv;
        const auto log_in_g = static_cast<float>(i_g) * target_inv;
        const auto log_in_b = static_cast<float>(i_b) * target_inv;

        float src_r = log_in_r;
        float src_g = log_in_g;
        float src_b = log_in_b;
        if (!passthrough) {
            // Linear-input source: map our log-input grid back to linear and sample there.
            src_r = lut_log_to_linear(log_in_r);
            src_g = lut_log_to_linear(log_in_g);
            src_b = lut_log_to_linear(log_in_b);
        }

        const auto sampled = sample_linear_cube_trilinear(src_halves, src_n, src_r, src_g, src_b);
        voxel.r = math::to_half(sampled.r);
        voxel.g = math::to_half(sampled.g);
        voxel.b = math::to_half(sampled.b);
        voxel.a = half_one;
    });

    CubeLutData out;
    out.size = target_size;
    out.pixels_rgba16f.resize(voxel_count * sizeof(LutVoxel));
    std::memcpy(out.pixels_rgba16f.data(), voxels.data(), out.pixels_rgba16f.size());
    return out;
}