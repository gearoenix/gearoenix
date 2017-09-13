#ifndef GEAROEMIX_RENDER_SHADER_SHADER_HPP
#define GEAROEMIX_RENDER_SHADER_SHADER_HPP
#include "../../core/asset/cr-asset.hpp"
#include <memory>
namespace gearoenix {
namespace system {
    class File;
}
namespace render {
    namespace shader {
        class Shader : public core::asset::Asset {
        private:
            Shader();

        public:
            static std::shared_ptr<Shader> read(std::shared_ptr<system::File>& file);
        };
    }
}
}
#endif
