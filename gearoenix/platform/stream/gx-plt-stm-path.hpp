#ifndef GEAROENIX_SYSTEM_STREAM_MEMORY_HPP
#define GEAROENIX_SYSTEM_STREAM_MEMORY_HPP

#include <string>
#include <variant>

namespace gearoenix::platform::stream {
struct AbsolutePath final {
    std::string data;

    explicit AbsolutePath(std::string data)
        : data(std::move(data))
    {
    }
};

struct AssetPath final {
    std::string data;

    explicit AssetPath(std::string data)
        : data(std::move(data))
    {
    }
};

struct Path final {
    std::variant<AbsolutePath, AssetPath> data;

    Path(Path&&) = default;
    Path(const Path&) = default;

    explicit Path(AbsolutePath data)
        : data(std::move(data))
    {
    }
    explicit Path(AssetPath data)
        : data(std::move(data))
    {
    }

    [[nodiscard]] static Path create_absolute(std::string data)
    {
        return Path(AbsolutePath(std::move(data)));
    }

    [[nodiscard]] static Path create_asset(std::string data)
    {
        return Path(AssetPath(std::move(data)));
    }

    [[nodiscard]] bool is_absolute() const
    {
        return 0 == data.index();
    }

    [[nodiscard]] bool is_asset() const
    {
        return 1 == data.index();
    }

    [[nodiscard]] const std::string& get_raw_data() const;
};
}

#endif