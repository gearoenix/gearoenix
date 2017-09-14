#ifndef GEAROEMIX_RENDER_SHADER_BASIC_HPP
#define GEAROEMIX_RENDER_SHADER_BASIC_HPP
#include "render-shader.hpp"
namespace gearoenix {
namespace render {
    namespace shader {
        namespace stage {
            class Vertex;
            class Fragment;
        }
        class Basic : public Shader {
            friend class Shader;

        private:
            stage::Vertex* vertex;
            stage::Fragment* fragment;
            Basic(std::shared_ptr<system::File>& file, Engine* engine);

        public:
        };
    }
}
}
#endif
