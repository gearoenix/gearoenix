#ifndef GEAROENIX_RENDER_SKYBOX_SKYBOX_HPP
#define GEAROENIX_RENDER_SKYBOX_SKYBOX_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../core/cr-types.hpp"
#include <memory>

namespace gearoenix {
namespace system {
    namespace stream {
        class Stream;
    }
}
namespace render {
    namespace material {
        class SkyboxBasic;
    }
    namespace mesh {
        class Mesh;
    }
    class Engine;
    namespace skybox {
        class Skybox : public core::asset::Asset {
        private:
            std::shared_ptr<mesh::Mesh> msh = nullptr;
            core::Id mesh_id;
            material::SkyboxBasic* mat = nullptr;

        public:
            Skybox(system::stream::Stream* s, Engine* e, core::EndCaller<core::EndCallerIgnore> c);
            ~Skybox();
            static Skybox* read(system::stream::Stream* s, Engine* e, core::EndCaller<core::EndCallerIgnore> c);
        };
    }
}
}
#endif
