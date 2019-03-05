#ifndef GEAROEMIX_RENDER_COMMAND_BUFFER_HPP
#define GEAROEMIX_RENDER_COMMAND_BUFFER_HPP
#include <memory>
#include <vector>

namespace gearoenix {
namespace render {
    namespace command {
        class Buffer {
        protected:
            std::vector<std::shared_ptr<Buffer>> recored_secondaries;

        public:
            virtual void begin() = 0;
            virtual void record(const std::shared_ptr<Buffer>& o);
        };
    }
}
}

#endif