#ifndef GEAROENIX_RENDER_MATERIAL_FRAME_HPP
#define GEAROENIX_RENDER_MATERIAL_FRAME_HPP
#include <memory>
namespace gearoenix {
namespace render {
    namespace buffer {
        class Uniform;
    }
    namespace pipeline {
        class Resource;
    }
    namespace material {
        struct Frame {
            bool textures_changes = true;
            std::shared_ptr<buffer::Uniform> uniform_buffer = nullptr;
            std::shared_ptr<pipeline::Resource> pipeline_resouce = nullptr;
        };
    }
}
}
#endif
