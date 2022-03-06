#ifndef GEAROENIX_PLATFORM_PATH_HPP
#define GEAROENIX_PLATFORM_PATH_HPP

#include <string>

namespace gearoenix::platform {
struct Application;

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

struct AssetPath final : public Path {
    AssetPath(const Application& app, const std::string& p) noexcept;
    ~AssetPath() noexcept final = default;
};
}

#endif
