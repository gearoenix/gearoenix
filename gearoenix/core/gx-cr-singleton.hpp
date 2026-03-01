#pragma once
#include "macro/gx-cr-mcr-assert.hpp"

namespace gearoenix::core {
template <typename Target>
struct Singleton {
protected:
    inline static Target* singleton_instance_ptr = nullptr;

    explicit Singleton(Target* const ptr)
    {
        GX_ASSERT_D(singleton_instance_ptr == nullptr);
        singleton_instance_ptr = ptr;
    }

public:
    Singleton(Singleton&&) = delete;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(Singleton&&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    virtual ~Singleton() { singleton_instance_ptr = nullptr; }

    [[nodiscard]] static Target& get() { return *singleton_instance_ptr; }
    [[nodiscard]] static bool singleton_is_valid() { return nullptr != singleton_instance_ptr; }
    [[nodiscard]] static bool singleton_is_invalid() { return nullptr == singleton_instance_ptr; }
};
}