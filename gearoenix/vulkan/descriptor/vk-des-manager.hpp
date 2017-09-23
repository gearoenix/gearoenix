#ifndef GEAROENIX_RENDER_DESCRIPTOR_MANAGER_HPP
#define GEAROENIX_RENDER_DESCRIPTOR_MANAGER_HPP
#include "../../render/shader/render-shader.hpp"
#include "../vk-linker.hpp"
#include <memory>
namespace gearoenix {
namespace core {
    namespace cache {
        class Cacher;
    }
}
namespace render {
    namespace device {
        class Logical;
    }
    namespace buffer {
        class Manager;
    }
    namespace descriptor {
        class Pool;
        class Set;
        class Manager {
        private:
            device::Logical* dev;
            core::cache::Cacher* cacher;
            Pool* pool;
            buffer::Manager* bufmgr;

        public:
            Manager(buffer::Manager* bufmgr);
            ~Manager();
            const device::Logical* get_logical_device() const;
            const buffer::Manager* get_buffer_manager() const;
            const Pool* get_pool() const;
            std::shared_ptr<Set> get_set(shader::Id sid);
            std::shared_ptr<Set> get_cached_set(shader::Id sid) const;
        };
    } // namespace descriptor
} // namespace render
} // namespace gearoenix
#endif // GEAROENIX_RENDER_DESCRIPTOR_MANAGER_HPP
