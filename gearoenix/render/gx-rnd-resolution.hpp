#pragma once
#include <cstdint>

#define GX_RENDER_HD_RESOLUTION_WIDTH 1280
#define GX_RENDER_HD_RESOLUTION_HEIGHT 720
#define GX_RENDER_DHD_RESOLUTION_WIDTH 1920
#define GX_RENDER_DHD_RESOLUTION_HEIGHT 1080
#define GX_RENDER_UHD_RESOLUTION_WIDTH 3840
#define GX_RENDER_UHD_RESOLUTION_HEIGHT 2160
#define GX_RENDER_DEFAULT_RESOLUTION_WIDTH GX_RENDER_HD_RESOLUTION_WIDTH
#define GX_RENDER_DEFAULT_RESOLUTION_HEIGHT GX_RENDER_HD_RESOLUTION_HEIGHT

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::render {
struct Resolution final {
    typedef std::uint8_t index_t;

    struct Fixed final {
        std::uint32_t width = GX_RENDER_DEFAULT_RESOLUTION_WIDTH;
        std::uint32_t height = GX_RENDER_DEFAULT_RESOLUTION_HEIGHT;
    };

    struct ScreenBased final {
        std::uint32_t numerator = 1;
        std::uint32_t denominator = 1;
    };

    constexpr static index_t fixed_index = 0;
    constexpr static index_t screen_based_index = 1;

private:
    index_t index = 0;
    union {
        Fixed fixed;
        ScreenBased screen_based;
    } data {};

public:
    Resolution() { data.fixed = {}; }

    Resolution& operator=(const Resolution&) = default;
    Resolution& operator=(Resolution&&) = default;

    Resolution& operator=(const Fixed& v)
    {
        index = fixed_index;
        data.fixed = v;
        return *this;
    }

    Resolution& operator=(const ScreenBased& v)
    {
        index = screen_based_index;
        data.screen_based = v;
        return *this;
    }

    [[nodiscard]] index_t get_index() const { return index; }

    [[nodiscard]] const Fixed& get_fixed() const;
    [[nodiscard]] const ScreenBased& get_screen_based() const;
    [[nodiscard]] bool show_debug_gui();
    void write(platform::stream::Stream& stream) const;
    void read(platform::stream::Stream& stream);
};
}