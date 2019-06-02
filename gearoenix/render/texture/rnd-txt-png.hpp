#ifndef GEAROENIX_RENDER_TEXTURE_PNG_HPP
#define GEAROENIX_RENDER_TEXTURE_PNG_HPP
#include <vector>
namespace gearoenix {
namespace system::stream {
    class Stream;
}
namespace render::texture {
    class PNG {
    public:
        static void decode(
            system::stream::Stream* file,
            std::vector<unsigned char>& decoded_data,
            unsigned int& width,
            unsigned int& height);
    };
}
}
#endif
