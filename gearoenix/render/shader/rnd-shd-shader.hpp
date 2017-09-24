#ifndef GEAROEMIX_RENDER_SHADER_SHADER_HPP
#define GEAROEMIX_RENDER_SHADER_SHADER_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-types.hpp"
#include <cstdint>
#include <memory>
#include <vector>
namespace gearoenix {
namespace system {
    class File;
}
namespace render {
    class Engine;
    namespace shader {
        typedef enum : core::Id {
            WHITE = 0,
            DIRECTIONAL_TEXTURED_SPECULATED_NOCUBE_FULLSHADOW_OPAQUE = 0XFFFFFFFF, // TODO change this
        } Id;
        class Shader : public core::asset::Asset {
        private:
        protected:
            Shader();

        public:
            typedef struct
            {
                typedef enum : unsigned int {
                    VERTEX,
                    FRAGMENT,
                    // Add whenever needed
                } BindingStage;
                typedef enum : unsigned int {
                    UNIFORM,
                    // Add whenever needed
                } Type;
                std::vector<BindingStage> binding_stages;
                unsigned int count;
                Type t;

            } ResourceDescription;

            static std::shared_ptr<Shader> read(std::shared_ptr<system::File>& file, Engine* engine);
            static const std::vector<ResourceDescription> get_resources_descriptions(Id id);
            static unsigned int get_uniform_size(Id id);
        };
    }
}
}
#endif
