#ifndef GEAROENIX_RENDER_TEXTURE_TEXTURE_HPP
#define GEAROENIX_RENDER_TEXTURE_TEXTURE_HPP
#include "../../core/asset/cr-asset.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include <memory>
namespace gearoenix {
namespace system {
    namespace stream {
        class Stream;
    }
}
namespace render {
    namespace engine {
        class Engine;
    }
    namespace texture {
        class Texture : public core::asset::Asset {
        protected:
            const std::shared_ptr<engine::Engine> render_engine;
            Texture(const core::Id my_id, const std::shared_ptr<engine::Engine>& e);

        public:
            virtual ~Texture();
            static Texture* read(
                const core::Id my_id,
                const std::shared_ptr<system::stream::Stream>& f,
                const std::shared_ptr<engine::Engine>& e,
                core::sync::EndCaller<core::sync::EndCallerIgnore> c);
        };
    }
}
}
#endif
