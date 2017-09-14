#ifndef GEAROEMIX_RENDER_SHADER_BASIC_HPP
#define GEAROEMIX_RENDER_SHADER_BASIC_HPP
#include "render-shader.hpp"
namespace gearoenix {
namespace system {
    class File;
}
namespace render {
    namespace shader {
        class Basic : public Shader {
        private:
            Basic();

        public:
            static std::shared_ptr<Shader> read(std::shared_ptr<system::File>& file);
        };
    }
}
}
#endif
