#ifndef GEAROENIX_RENDER_SKYBOX_SKYBOX_HPP
#define GEAROENIX_RENDER_SKYBOX_SKYBOX_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/cr-types.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
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
    namespace scene {
        class Scene;
    }
    class Engine;
    namespace skybox {
        class Skybox : public core::asset::Asset {
        private:
            //std::shared_ptr<mesh::Mesh> msh = nullptr;
            core::Id mesh_id;
            //material::SkyboxBasic* mat = nullptr;

        public:
            //Skybox(core::Id my_id, system::stream::Stream* s, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> c);
            //~Skybox();
            //static Skybox* read(core::Id my_id, system::stream::Stream* s, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> c);
            //void draw();
            //void update(const scene::Scene* s);
        };
    }
}
}
#endif
