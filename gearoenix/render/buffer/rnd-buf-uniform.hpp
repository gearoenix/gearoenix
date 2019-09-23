#ifndef GEAROENIX_RENDER_BUFFER_UNIFORM_HPP
#define GEAROENIX_RENDER_BUFFER_UNIFORM_HPP
#include "../../core/cr-build-configuration.hpp"
#include "../../system/sys-log.hpp"
#include "rnd-buf-buffer.hpp"
namespace gearoenix::render::buffer {
class Uniform : public Buffer {
protected:
    Uniform(const std::size_t s, engine::Engine* const e) noexcept
        : Buffer(s, e)
    {
    }

    virtual const void* get_data() const noexcept = 0;
    virtual void* get_data() noexcept = 0;
    virtual void update(const void*) noexcept = 0;

public:
    virtual ~Uniform() noexcept = default;

    template <typename T>
    [[nodiscard]] const T* get_ptr() const noexcept;
    template <typename T>
    void set_data(const T&) noexcept;
};

template <typename T>
inline const T* Uniform::get_ptr() const noexcept
{
#ifdef GX_DEBUG_MODE
    if (buffer_size != sizeof(T))
        GXUNEXPECTED
#endif
    return reinterpret_cast<const T*>(get_data());
}
template <typename T>
inline void Uniform::set_data(const T& d) noexcept
{
#ifdef GX_DEBUG_MODE
    if (buffer_size != sizeof(T))
        GXUNEXPECTED
#endif
    update(&d);
}
}
#endif