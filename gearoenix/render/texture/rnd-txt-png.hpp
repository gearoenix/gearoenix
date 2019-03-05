#ifndef GEAROEMIX_RENDER_TEXTURE_PNG_HPP
#define GEAROEMIX_RENDER_TEXTURE_PNG_HPP
#include <vector>
#include <memory>
namespace gearoenix {
namespace system {
    namespace stream {
        class Stream;
    }
}
namespace render {
    class Engine;
    namespace texture {
        class PNG {
        public:
            static void decode(
				const std::shared_ptr<system::stream::Stream> &file,
                std::vector<unsigned char>& decoded_data,
                unsigned int& width,
                unsigned int& height);
        };
    }
}
}
#endif
