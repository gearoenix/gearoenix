#include "gx-vk-shader-stage-vertex.hpp"
#if GX_RENDER_VULKAN_ENABLED
gearoenix::render::shader::stage::Vertex::Vertex(std::vector<unsigned char>& data, Engine* engine)
    : Stage(data, engine)
{
}
#endif
