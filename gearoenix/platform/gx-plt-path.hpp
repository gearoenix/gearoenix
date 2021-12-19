#ifndef GEAROENIX_PLATFORM_PATH_HPP
#define GEAROENIX_PLATFORM_PATH_HPP

#include <string>

namespace gearoenix::platform {

struct Path {
    const std::string raw;

protected:
    Path(std::string raw) noexcept
        : raw(std::move(raw))
    {
    }

public:
    virtual ~Path() noexcept = default;
};

struct AbsolutePath final : public Path {
    AbsolutePath(std::string p) noexcept
        : Path(std::move(p))
    {
    }
    ~AbsolutePath() noexcept final = default;
};

/// TODO: Add asset path that can create the raw string for different kind of platform or a way of spawning stream
}

#endif