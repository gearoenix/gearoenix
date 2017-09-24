#ifndef GEAROEMIX_RENDER_SHADER_BASIC_HPP
#define GEAROEMIX_RENDER_SHADER_BASIC_HPP
#include "rnd-shd-shader.hpp"

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
            static const std::vector<stage::Id> stages_ids;

            Basic(system::File* file, Engine* engine);

        public:
            ~Basic();
            const std::vector<stage::Id>& get_stages_ids() const;
            const stage::Stage* get_stage(stage::Id id) const;
            stage::Stage* get_stage(stage::Id id);
        };
    }
}
}
#endif
