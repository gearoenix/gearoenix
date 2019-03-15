#ifndef GEAROENIX_RENDER_BUFFER_BUFFER_HPP
#define GEAROENIX_RENDER_BUFFER_BUFFER_HPP
#include "../../core/cr-build-configuration.hpp"
#include <memory>
namespace gearoenix {
	namespace system {
		namespace stream {
			class Stream;
		}
	}
	namespace render {
		namespace engine {
			class Engine;
		}
		namespace buffer {
			class Buffer {
			protected:
				const std::shared_ptr<engine::Engine> e;
				const unsigned int size;

				Buffer(const unsigned int size, const std::shared_ptr<engine::Engine>& e);
			public:
				virtual ~Buffer();
				unsigned int get_size() const;
			};
		}
	}
}
#endif
