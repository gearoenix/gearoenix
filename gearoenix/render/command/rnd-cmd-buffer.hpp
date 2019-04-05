#ifndef GEAROEMIX_RENDER_COMMAND_BUFFER_HPP
#define GEAROEMIX_RENDER_COMMAND_BUFFER_HPP
#include <memory>
#include <vector>

namespace gearoenix {
namespace render {
	namespace pipeline {
		class ResourceSet;
	}
    namespace command {
        class Buffer {
        protected:
			std::vector<std::shared_ptr<Buffer>> recored_secondaries;
			std::vector<std::shared_ptr<pipeline::ResourceSet>> bound_resource_sets;

        public:
			virtual ~Buffer();
			virtual void begin();
			virtual void end();
            virtual void record(const std::shared_ptr<Buffer>& o);
			virtual void bind(const std::shared_ptr<pipeline::ResourceSet> &r);
        };
    }
}
}

#endif