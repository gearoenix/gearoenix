#ifndef GEAROENIX_VULKAN_TEXTURE_2D_HPP
#define GEAROENIX_VULKAN_TEXTURE_2D_HPP
#include "../../render/texture/rnd-txt-texture.hpp"
#include "../vk-linker.hpp"

namespace gearoenix {
namespace system {
    class File;
}
namespace render {
    namespace image {
        class View;
    }
    class Engine;
    namespace texture {
        class Sampler2D;
        class Texture2D : public Texture {
        private:
            image::View* iv;
            Sampler2D* smp;

        public:
            Texture2D(system::File* file, Engine* engine);
            ~Texture2D();
            static uint32_t get_memory_type_bits(Engine* engine);
            static void fill_info(VkImageCreateInfo& info, unsigned int img_width, unsigned int img_height, unsigned int channels);
        };
    }
}
}
#endif