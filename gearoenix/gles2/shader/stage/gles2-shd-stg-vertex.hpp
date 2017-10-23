#ifndef GEAROEMIX_OPENGL_SHADER_STAGE_VERTEX_HPP
#define GEAROEMIX_OPENGL_SHADER_STAGE_VERTEX_HPP
#include "../../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES2
#include "gles2-shd-stage.hpp"
namespace gearoenix {
namespace gles2 {
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
