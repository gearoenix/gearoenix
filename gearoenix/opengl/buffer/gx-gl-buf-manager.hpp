#pragma once
#include "../../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_OPENGL_ENABLED
#include "../../core/gx-cr-singleton.hpp"
#include "../../render/buffer/gx-rnd-buf-manager.hpp"

#include <vector>

namespace gearoenix::gl::buffer {
struct Manager final : render::buffer::Manager, core::Singleton<Manager> {
private:
    std::vector<std::uint8_t> uniform_buffer_data;

protected:
    [[nodiscard]] std::size_t get_uniform_frame_pointer() override;

public:
    Manager();
    ~Manager() override;
};
}
#endif