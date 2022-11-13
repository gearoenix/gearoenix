#include "gx-gl-sbm-camera.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include <thread>

gearoenix::gl::submission::Camera::Camera() noexcept
    : threads_opaque_models_data(std::thread::hardware_concurrency())
    , threads_translucent_models_data(std::thread::hardware_concurrency())
    , threads_mvps(std::thread::hardware_concurrency())
    , debug_meshes_threads(std::thread::hardware_concurrency())
{
}

void gearoenix::gl::submission::Camera::clear() noexcept
{
    models_data.clear();
    mvps.clear();
    debug_meshes.clear();

    for (auto& v : threads_opaque_models_data)
        v.clear();
    for (auto& v : threads_translucent_models_data)
        v.clear();
    for (auto& v : threads_mvps)
        v.clear();
    for (auto& v : debug_meshes_threads)
        v.clear();
}

#endif