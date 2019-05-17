#ifndef GEAROENIX_VULKAN_SHADER_STAGE_VERTEX_HPP
#define GEAROENIX_VULKAN_SHADER_STAGE_VERTEX_HPP
#include "../../../core/cr-build-configuration.hpp"
#ifdef USE_VULKAN
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
#endif
