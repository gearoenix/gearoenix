#ifndef GEAROEMIX_OPENGL_SHADER_STAGE_STAGE_HPP
#define GEAROEMIX_OPENGL_SHADER_STAGE_STAGE_HPP
#include "../../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES2
#include <vector>
namespace gearoenix {
namespace render {
    class Engine;
    namespace shader {
        namespace stage {
            class Stage {
            private:
            public:
                Stage(std::vector<unsigned char>&, Engine*);
                ~Stage();
            };
        }
    }
}
}
#endif
#endif
