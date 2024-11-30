#pragma once
#include <cstdint>
#include <variant>

#define GX_RENDER_HD_RESOLUTION_WIDTH 1280
#define GX_RENDER_HD_RESOLUTION_HEIGHT 720
#define GX_RENDER_DHD_RESOLUTION_WIDTH 1920
#define GX_RENDER_DHD_RESOLUTION_HEIGHT 1080
#define GX_RENDER_UHD_RESOLUTION_WIDTH 3840
#define GX_RENDER_UHD_RESOLUTION_HEIGHT 2160
#define GX_RENDER_DEFAULT_RESOLUTION_WIDTH GX_RENDER_HD_RESOLUTION_WIDTH
#define GX_RENDER_DEFAULT_RESOLUTION_HEIGHT GX_RENDER_HD_RESOLUTION_HEIGHT

namespace gearoenix::render {
struct FixedResolution final {
    std::uint32_t width = GX_RENDER_DEFAULT_RESOLUTION_WIDTH;
    std::uint32_t height = GX_RENDER_DEFAULT_RESOLUTION_HEIGHT;
};

struct ScreenBasedResolution final {
    std::uint32_t numerator = 1;
    std::uint32_t denominator = 1;
};

typedef std::variant<FixedResolution, ScreenBasedResolution> Resolution;

bool imgui_show(Resolution&);
}