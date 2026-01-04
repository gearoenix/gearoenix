#include "gx-vk-shader-stage-fragment.hpp"
#if GX_RENDER_VULKAN_ENABLED
gearoenix::render::shader::stage::Fragment::Fragment(std::vector<unsigned char>& data, Engine* engine)
    : Stage(data, engine)
{
}
#endif
