#ifndef GEAROEMIX_RENDER_SHADER_SHADER_HPP
#define GEAROEMIX_RENDER_SHADER_SHADER_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-types.hpp"
#include <memory>
#include <vector>
namespace gearoenix {
namespace core {
    class EndCaller;
}
namespace system {
    class File;
}
namespace render {
    class Engine;
    namespace shader {
        typedef enum : core::Id {
            WHITE_POSITION = 0,
            WHITE_POSITION_NORMAL = 1,
            WHITE_POSITION_UV = 2,
            WHITE_POSITION_NORMAL_UV = 3,
            SOLID_COLORED_NOTSPECULATED_NOCUBE_SHADELESS_OPAQUE = 201000,
        } Id;
        namespace stage {

            typedef enum : core::Id {
                FRAGMENT = 0,
                VERTEX = 1,
            } Id;
        }

        typedef enum {
            VEC3F32,
            VEC2F32,
        } VertexAttribute;

        typedef struct
        {
            typedef enum : unsigned int {
                UNIFORM,
                SAMPER2D,
                // Add whenever needed
            } Type;
            std::vector<stage::Id> binding_stages;
            unsigned int count;
            Type t;
        } ResourceDescription;

        class Shader : public core::asset::Asset {
        private:
            static const std::vector<ResourceDescription> buffer_rsc_des;
            static const std::vector<ResourceDescription> textured_rsc_des;
            static const std::vector<VertexAttribute> has_pos;
            static const std::vector<VertexAttribute> has_pos_nrm;
            static const std::vector<VertexAttribute> has_pos_uv;
            static const std::vector<VertexAttribute> has_pos_nrm_uv;

        protected:
            static const std::vector<stage::Id> graphic_2_stage;
            Shader();

        public:
            virtual ~Shader();
            virtual const std::vector<stage::Id>& get_stages_ids() const = 0;
            virtual void use() = 0;
            static std::shared_ptr<Shader> read(core::Id sid, system::File* file, Engine* engine, std::shared_ptr<core::EndCaller> end);
            static const std::vector<ResourceDescription>& get_resources_descriptions(Id id);
            static const std::vector<VertexAttribute>& get_vertex_atributes(Id id);
            static unsigned int get_uniform_size(Id id);
            static unsigned int get_vertex_real_count(Id id);
        };
    }
}
}
#endif
