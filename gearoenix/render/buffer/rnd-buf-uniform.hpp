#ifndef GEAROENIX_RENDER_BUFFER_UNIFORM_HPP
#define GEAROENIX_RENDER_BUFFER_UNIFORM_HPP
#include "rnd-buf-buffer.hpp"
namespace gearoenix::render::buffer {
class Uniform : public Buffer {
protected:
    Uniform(const unsigned int s, engine::Engine* const e) noexcept
        : Buffer(s, e)
    {
    }

public:
    virtual ~Uniform() noexcept = default;
    virtual void update(const void*) noexcept = 0;
	virtual const void* get_data() const  noexcept = 0;
	virtual void* get_data() noexcept = 0;
};
}
#endif