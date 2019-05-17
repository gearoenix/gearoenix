#ifndef GEAROENIX_GLES2_COMMAND_BUFFER_HPP
#define GEAROENIX_GLES2_COMMAND_BUFFER_HPP
#include "../../gl/gl-types.hpp"
#include "../../render/command/rnd-cmd-buffer.hpp"

namespace gearoenix {
namespace gles2 {
    namespace command {
        class Buffer : public render::command::Buffer {
        public:
            ~Buffer() override final;
            void begin() override final;
            void end() override final;
            void record(const std::shared_ptr<render::command::Buffer>& o) override final;
            void bind(const std::shared_ptr<render::pipeline::ResourceSet>& r) override final;
            void bind(const std::shared_ptr<render::texture::Target>& t) override final;
            gl::uint play(gl::uint bound_shader_program = static_cast<gl::uint>(-1)) const;
        };
    }
}
}

#endif