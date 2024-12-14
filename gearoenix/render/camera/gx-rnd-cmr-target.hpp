#pragma once
#include "../../math/gx-math-vector-2d.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include <memory>
#include <variant>

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::render::texture {
struct Target;
}

namespace gearoenix::render::camera {
struct Target final {
    struct Customised final {
        std::shared_ptr<texture::Target> target;
    };

private:
    constexpr static std::uint32_t CUSTOMISED_VAR_INDEX = 0;
    constexpr static std::uint32_t DEFAULT_VAR_INDEX = 1;

    std::variant<Customised, texture::DefaultCameraTargets> target = texture::DefaultCameraTargets {};

public:
    Target();
    ~Target();
    Target(Target&&) noexcept = default;
    Target& operator=(Target&&) noexcept = default;
    Target(const Target&) = default;
    Target& operator=(const Target&) = default;
    explicit Target(Customised&&);
    explicit Target(texture::DefaultCameraTargets&&);
    [[nodiscard]] static Target construct_customised(std::shared_ptr<texture::Target>&& target);
    [[nodiscard]] static Target construct_default(texture::DefaultCameraTargets&& targets);
    [[nodiscard]] bool is_customised() const;
    [[nodiscard]] bool is_default() const;
    void set_customised(std::shared_ptr<texture::Target>&& customised_target);
    [[nodiscard]] double get_aspect_ratio() const;
    [[nodiscard]] math::Vec2<std::uint32_t> get_dimension() const;
    [[nodiscard]] const texture::DefaultCameraTargets& get_default() const;
    [[nodiscard]] const Customised& get_customised() const;
    void write(std::shared_ptr<platform::stream::Stream>&& stream, core::job::EndCaller<>&& end) const;
    static void read(std::shared_ptr<platform::stream::Stream>&& stream, core::job::EndCaller<Target>&& end);
};
}