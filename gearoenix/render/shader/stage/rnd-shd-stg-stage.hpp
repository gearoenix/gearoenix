#ifndef GEAROEMIX_RENDER_SHADER_STAGE_STAGE_HPP
#define GEAROEMIX_RENDER_SHADER_STAGE_STAGE_HPP
#include "../../../core/cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "../../../vulkan/shader/stage/vk-shader-stage.hpp"
#elif defined(USE_OPENGL_ES2)
#include "../../../gles2/shader/stage/gles2-shd-stage.hpp"
#else
#error "Not implemented"
#endif // USE_*
namespace gearoenix {
namespace render {
    namespace shader {
        namespace stage {

            typedef enum {
                VERTEX,
                FRAGMENT,
                // Add whenever needed
            } Id;

            class Stage;
        }
    }
}
}
#endif // GEAROEMIX_RENDER_SHADER_STAGE_STAGE_HPP
