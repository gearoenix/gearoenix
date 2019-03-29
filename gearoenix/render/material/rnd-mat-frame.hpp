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
			// TODO It can become a separat structure as framed uniform buffer
            std::shared_ptr<buffer::Uniform> uniform_buffer = nullptr;
        };
    }
}
}
#endif
