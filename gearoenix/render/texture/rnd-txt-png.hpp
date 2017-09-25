#ifndef GEAROEMIX_RENDER_TEXTURE_PNG_HPP
#define GEAROEMIX_RENDER_TEXTURE_PNG_HPP
#include <vector>
namespace gearoenix {
namespace system {
    class File;
}
namespace render {
    class Engine;
    namespace texture {
        class PNG {
        public:
            void decode(system::File* file, Engine* engine, std::vector<unsigned char>& decoded_data);
        };
    }
}
}
#endif
