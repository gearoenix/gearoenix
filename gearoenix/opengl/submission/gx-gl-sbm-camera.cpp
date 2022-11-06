#include "gx-gl-sbm-camera.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include <thread>

gearoenix::gl::submission::Camera::Camera() noexcept
    : threads_opaque_models_data(std::thread::hardware_concurrency())
    , threads_translucent_models_data(std::thread::hardware_concurrency())
    , opaque_threads_mvps(std::thread::hardware_concurrency())
    , translucent_threads_mvps(std::thread::hardware_concurrency())
    , debug_meshes_threads(std::thread::hardware_concurrency())
{
}

#endif