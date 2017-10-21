#include "vk-shader-stage-fragment.hpp"
#ifdef USE_VULKAN
gearoenix::render::shader::stage::Fragment::Fragment(std::vector<unsigned char>& data, Engine* engine)
    : Stage(data, engine)
{
}
#endif
