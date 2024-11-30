#pragma once
#include "../../../core/gx-cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "gx-vk-shader-stage.hpp"
namespace gearoenix {
namespace render {
    struct Engine;
    namespace shader {
        namespace stage {
            struct Vertex : public Stage {
            private:
            public:
                Vertex(std::vector<unsigned char>& data, Engine* engine);
            };
        }
    }
}
}
#endif