#ifndef GEAROEMIX_RENDER_PIPELINE_RESOURCE_HPP
#define GEAROEMIX_RENDER_PIPELINE_RESOURCE_HPP
#include <vector>
#include <memory>

namespace gearoenix {
namespace render {
namespace buffer {
class Buffer;
}
	namespace texture {
		class Texture;
	}
    namespace pipeline {
        class Resource {
		private:
			const std::vector<std::shared_ptr<texture::Texture>> ts;
        public:
                        Resource(const std::shared_ptr<buffer::Buffer> &b, const std::vector<std::shared_ptr<texture::Texture>> &ts);
            virtual ~Resource();
			const std::vector<std::shared_ptr<texture::Texture>> &get_textures() const;
        };
    }
}
}

#endif
