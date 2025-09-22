#pragma once
#include <string>
#include <variant>

namespace gearoenix::platform::stream {
struct AbsolutePath final {
    std::string data;

    explicit AbsolutePath(std::string&& data);
};

struct AssetPath final {
    std::string data;

    explicit AssetPath(std::string&& data);
};

struct Path final {
    std::variant<AbsolutePath, AssetPath> data;

    Path(Path&&) noexcept;
    Path(const Path&);
    Path& operator=(Path&&) noexcept;
    Path& operator=(const Path&);

    explicit Path(AbsolutePath&& data);
    explicit Path(AssetPath&& data);

    [[nodiscard]] static Path create_absolute(std::string&& data);
    [[nodiscard]] static Path create_asset(std::string&& data);

    [[nodiscard]] bool is_absolute() const;
    [[nodiscard]] bool is_asset() const;

    [[nodiscard]] const std::string& get_raw_data() const;
};
}