#ifndef GEAROENIX_VULKAN_TEXTURE_2D_HPP
#define GEAROENIX_VULKAN_TEXTURE_2D_HPP
#include "../../render/texture/rnd-txt-texture.hpp"
#include "../vk-linker.hpp"
#include <memory>
namespace gearoenix {
namespace core {
    class EndCaller;
}
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

        public:
            Texture2D(system::File* file, Engine* engine, std::shared_ptr<core::EndCaller> end);
            ~Texture2D();
            image::View* get_view();
            static uint32_t get_memory_type_bits(Engine* engine);
            static void fill_info(VkImageCreateInfo& info, unsigned int img_width, unsigned int img_height, unsigned int channels);
        };
    }
}
}
#endif
