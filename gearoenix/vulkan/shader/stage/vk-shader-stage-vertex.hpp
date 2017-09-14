#ifndef GEAROEMIX_VULKAN_SHADER_STAGE_VERTEX_HPP
#define GEAROEMIX_VULKAN_SHADER_STAGE_VERTEX_HPP
#include "vk-shader-stage.hpp"
namespace gearoenix {
namespace render {
    class Engine;
    namespace shader {
        namespace stage {
            class Vertex : public Stage {
            private:
            public:
                Vertex(std::vector<unsigned char>& data, Engine* engine);
            };
        }
    }
}
}
#endif
