#ifndef GEAROEMIX_RENDER_PIPELINE_RESOURCE_HPP
#define GEAROEMIX_RENDER_PIPELINE_RESOURCE_HPP
#include <vector>
#include <memory>

namespace gearoenix {
namespace render {
	namespace texture {
		class Texture;
	}
    namespace pipeline {
        class Resource {
		private:
			const std::vector<std::shared_ptr<texture::Texture>> ts;
        public:
			Resource(const std::vector<std::shared_ptr<texture::Texture>> &ts);
			const std::vector<std::shared_ptr<texture::Texture>> &get_textures() const;
        };
    }
}
}

#endif