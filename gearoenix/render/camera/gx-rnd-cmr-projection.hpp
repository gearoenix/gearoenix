#pragma once
#include <cstdint>

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::render::camera {
struct ProjectionData final {
    typedef std::uint8_t index_t;

    struct Perspective {
        float field_of_view_y = 0.8f;
    };

    struct Orthographic {
        float scale = 1.0;
    };

    constexpr static index_t perspective_index = 0;
    constexpr static index_t orthographic_index = 1;

private:
    index_t index = perspective_index;
    union {
        Perspective perspective;
        Orthographic orthographic;
    } data {};

public:
    constexpr ProjectionData()
    {
        data.perspective = {};
    }

    constexpr ProjectionData(const ProjectionData&) = default;

    constexpr explicit ProjectionData(const Perspective& p)
        : index(perspective_index)
    {
        data.perspective = p;
    }

    constexpr explicit ProjectionData(const Orthographic& o)
        : index(orthographic_index)
    {
        data.orthographic = o;
    }

    constexpr ProjectionData& operator=(const ProjectionData&) = default;
    constexpr ProjectionData& operator=(ProjectionData&&) = default;

    constexpr ProjectionData& operator=(const Perspective& v)
    {
        index = perspective_index;
        data.perspective = v;
        return *this;
    }

    constexpr ProjectionData& operator=(const Orthographic& v)
    {
        index = orthographic_index;
        data.orthographic = v;
        return *this;
    }

    [[nodiscard]] constexpr index_t get_index() const
    {
        return index;
    }

    [[nodiscard]] constexpr bool is_perspective() const
    {
        return index == perspective_index;
    }

    [[nodiscard]] constexpr bool is_orthographic() const
    {
        return index == orthographic_index;
    }

    [[nodiscard]] constexpr static ProjectionData construct_perspective(const float field_of_view_y = 0.8f) {
        return ProjectionData { Perspective { field_of_view_y } };
    }

    [[nodiscard]] constexpr static ProjectionData construct_orthographic(const float scale = 1.0f) {
        return ProjectionData { Orthographic { scale } };
    }

    [[nodiscard]] const Perspective& get_perspective() const;
    [[nodiscard]] Perspective& get_perspective();
    [[nodiscard]] const Orthographic& get_orthographic() const;
    [[nodiscard]] Orthographic& get_orthographic();

    void write(platform::stream::Stream& s) const;
    void read(platform::stream::Stream& s);
};
}