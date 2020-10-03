#ifndef GEAROENIX_GLC3_BUFFER_UNIFORM_HPP
#define GEAROENIX_GLC3_BUFFER_UNIFORM_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../render/buffer/gx-rnd-buf-uniform.hpp"
#include <vector>

namespace gearoenix::glc3::engine {
class Engine;
}

namespace gearoenix::glc3::buffer {
class Uniform final : public render::buffer::Uniform {
private:
    std::vector<unsigned char> data;

public:
    Uniform(std::size_t s, std::size_t frame_number, engine::Engine* e) noexcept;
    ~Uniform() noexcept final = default;
    void update(const void* src) noexcept final;
    [[nodiscard]] const void* get_data() const noexcept final;
    [[nodiscard]] void* get_data() noexcept final;
};
}

#endif
#endif