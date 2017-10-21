#ifndef GEAROENIX_OPENGL_PIPELINE_PIPELINE_HPP
#define GEAROENIX_OPENGL_PIPELINE_PIPELINE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_OPENGL_ES2
#include "../../core/cache/cr-cache-cached.hpp"
#include "../../render/shader/rnd-shd-shader.hpp"
namespace gearoenix {
namespace core {
    class Application;
}
namespace render {
    namespace device {
        class Logical;
    }
    namespace descriptor {
        class SetLayout;
    }
    class RenderPass;
    namespace pipeline {
        class Cache;
        class Layout;
        class Pipeline : public core::cache::Cached {
        private:
        public:
            Pipeline(shader::Id sid,
                Cache* cache,
                RenderPass* rndpass,
                const std::shared_ptr<shader::Shader>& shd,
                descriptor::SetLayout* dessetlay,
                Engine* eng);
            ~Pipeline();
            descriptor::SetLayout* get_descriptor_set_layout();
            const Layout* get_layout() const;
            void bind();
        };
    }
}
}
#endif
#endif
