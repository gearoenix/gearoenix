#pragma once
#include "macro/gx-cr-mcr-assert.hpp"

namespace gearoenix::core {
template <typename Target>
struct Singleton {
private:
    [[nodiscard]] constexpr static Target*& get_singleton_instance()
    {
        static Target* singleton_instance = nullptr;
        return singleton_instance;
    }

protected:
    explicit Singleton(Target* const ptr)
    {
        GX_ASSERT_D(get_singleton_instance() == nullptr);
        get_singleton_instance() = ptr;
    }

public:
    Singleton(Singleton&&) = delete;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(Singleton&&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    virtual ~Singleton()
    {
        get_singleton_instance() = nullptr;
    }

    [[nodiscard]] static Target& get()
    {
        return *get_singleton_instance();
    }
};
}