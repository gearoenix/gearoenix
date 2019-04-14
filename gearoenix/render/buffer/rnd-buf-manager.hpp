#ifndef GEAROENIX_RENDER_BUFFER_MANAGER_HPP
#define GEAROENIX_RENDER_BUFFER_MANAGER_HPP
#include "../../core/cr-build-configuration.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../math/math-vertex.hpp"
#include <memory>
#include <vector>

namespace gearoenix {
namespace render {
    namespace engine {
        class Engine;
    }
    namespace buffer {
        class Static;
        class Uniform;
        class Manager {
        protected:
            const std::shared_ptr<engine::Engine> e;
            Manager(const std::shared_ptr<engine::Engine>& e);

        public:
            virtual ~Manager();
            virtual std::shared_ptr<Uniform> create_uniform(const unsigned int size) = 0;
            virtual std::shared_ptr<Static> create_static(const std::vector<math::BasicVertex> vertices, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) = 0;
            virtual std::shared_ptr<Static> create_static(const std::vector<std::uint32_t> indices, const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) = 0;
        };
    }
}
}
#endif
