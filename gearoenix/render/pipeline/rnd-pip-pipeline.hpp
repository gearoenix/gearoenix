#ifndef GEAROENIX_RENDER_PIPELINE_PIPELINE_HPP
#define GEAROENIX_RENDER_PIPELINE_PIPELINE_HPP
#include "../../core/cache/cr-cache-cached.hpp"
#include "../../core/cr-types.hpp"
namespace gearoenix {
namespace render {
    class Engine;
    namespace pipeline {
        class Pipeline : public core::cache::Cached {
        protected:
            Engine* eng;

        public:
            Pipeline(core::Id sid, Engine* eng);
            virtual ~Pipeline();
            virtual void bind() = 0;
        };
    }
}
}
#endif // GEAROENIX_RENDER_PIPELINE_PIPELINE_HPP
