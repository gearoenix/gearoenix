#include "vk-shader-stage-vertex.hpp"
#ifdef USE_VULKAN
gearoenix::render::shader::stage::Vertex::Vertex(std::vector<unsigned char>& data, Engine* engine)
    : Stage(data, engine)
{
}
#endif
