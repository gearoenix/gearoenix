#ifndef GEAROENIX_RENDER_CAMERA_TARGET_HPP
#define GEAROENIX_RENDER_CAMERA_TARGET_HPP
#include <memory>
#include <variant>
#include "../../math/gx-math-vector-2d.hpp"

namespace gearoenix::render::texture {
    struct Target;
}

namespace gearoenix::render::camera {
    struct Target final {
        struct Customised final {
            std::shared_ptr<texture::Target> target;
        };

        struct Default final {
            std::shared_ptr<texture::Target> first;
            std::shared_ptr<texture::Target> second;
        };

    private:
        static constexpr std::size_t CUSTOMISED_VAR_INDEX = 0;
        static constexpr std::size_t DEFAULT_VAR_INDEX = 1;

        std::variant<Customised, Default> target = Default {};

    public:
        Target();
        explicit Target(Customised&&);
        explicit Target(Default&&);
        [[nodiscard]] static Target construct_customised( std::shared_ptr<texture::Target>&& target);
        [[nodiscard]] static Target construct_default(std::shared_ptr<texture::Target>&& first, std::shared_ptr<texture::Target>&& second);
        [[nodiscard]] bool is_customised() const;
        [[nodiscard]] bool is_default() const;
        void set_customised(std::shared_ptr<texture::Target>&& customised_target);
        [[nodiscard]] double get_aspect_ratio() const;
        [[nodiscard]] math::Vec2<std::size_t> get_dimension() const;
        [[nodiscard]] const Default& get_default() const;
        [[nodiscard]] const Customised& get_customised() const;
    };
}

#endif