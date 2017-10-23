#ifndef GEAROEMIX_GLES2_SHADER_STAGE_FRAGMENT_HPP
#define GEAROEMIX_GLES2_SHADER_STAGE_FRAGMENT_HPP
#include "../../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES2
#include "gles2-shd-stage.hpp"
namespace gearoenix {
namespace gles2 {
    class Engine;
    namespace shader {
        namespace stage {
            class Fragment : public Stage {
            private:
            public:
                Fragment(std::vector<unsigned char>& data, Engine* engine);
            };
        }
    }
}
}
#endif
#endif
