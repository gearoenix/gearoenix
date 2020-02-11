#ifndef GEAROENIX_RENDER_COMMAND_BUFFER_HPP
#define GEAROENIX_RENDER_COMMAND_BUFFER_HPP
#include <memory>
#include <vector>

namespace gearoenix::render {
namespace pipeline {
    class ResourceSet;
}
namespace texture {
    class Target;
}
namespace command {
    class Buffer {
    protected:
        std::vector<Buffer*> recorded_secondaries;
        std::vector<pipeline::ResourceSet*> bound_resource_sets;
        const texture::Target* render_target = nullptr;

    public:
        virtual ~Buffer() noexcept = default;
        virtual void begin() noexcept;
        virtual void end() noexcept;
        virtual void record(Buffer* o) noexcept;
        virtual void bind(pipeline::ResourceSet* r) noexcept;
        virtual void bind(const texture::Target* t) noexcept;
    };
}
}

#endif