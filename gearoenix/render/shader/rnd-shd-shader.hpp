#ifndef GEAROEMIX_RENDER_SHADER_SHADER_HPP
#define GEAROEMIX_RENDER_SHADER_SHADER_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-types.hpp"
#include "stage/rnd-shd-stg-stage.hpp"
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
        typedef enum {
            VEC3F32,
            VEC2F32
        } VertexAttribute;

        class Shader : public core::asset::Asset {
        public:
            typedef struct
            {
                typedef enum : unsigned int {
                    UNIFORM,
                    SAMPER2D
                    // Add whenever needed
                } Type;
                std::vector<stage::Id> binding_stages;
                unsigned int count;
                Type t;

            } ResourceDescription;

        private:
            static const std::vector<ResourceDescription> white_rsc_des;
            static const std::vector<VertexAttribute> has_pos;

        protected:
            Shader();

        public:
            virtual const std::vector<stage::Id>& get_stages_ids() const = 0;
            virtual const stage::Stage* get_stage(stage::Id id) const = 0;
            virtual stage::Stage* get_stage(stage::Id id) = 0;

            static std::shared_ptr<Shader> read(system::File* file, Engine* engine);
            static const std::vector<ResourceDescription>& get_resources_descriptions(Id id);
            static const std::vector<VertexAttribute>& get_vertex_atributes(Id id);
            static unsigned int get_uniform_size(Id id);
            static unsigned int get_vertex_real_count(Id id);
        };
    }
}
}
#endif
