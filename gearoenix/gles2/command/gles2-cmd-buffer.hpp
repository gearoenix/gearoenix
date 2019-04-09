#ifndef GEAROEMIX_GLES2_COMMAND_BUFFER_HPP
#define GEAROEMIX_GLES2_COMMAND_BUFFER_HPP
#include "../../render/command/rnd-cmd-buffer.hpp"

namespace gearoenix {
namespace gles2 {
    namespace command {
        class Buffer: public render::command::Buffer {
        public:
			~Buffer() override final;
			void begin() override final;
			void end() override final;
            void record(const std::shared_ptr<render::command::Buffer>& o) override final;
			void bind(const std::shared_ptr<render::pipeline::ResourceSet> &r) override final;
			void bind(const std::shared_ptr<render::texture::Target> &t) override final;
			void play() const;
        };
    }
}
}

#endif