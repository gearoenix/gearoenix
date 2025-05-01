#pragma once
#include "macro/gx-cr-mcr-assert.hpp"

namespace gearoenix::core
{
    template <typename Target>
    struct Singleton
    {
    private:
        static Target* instance = nullptr;

    protected:
        Singleton()
        {
            GX_ASSERT_D(instance == nullptr);
            instance = this;
        }

    public:
        virtual ~Singleton()
        {
            GX_ASSERT_D(instance == this);
            instance = nullptr;
        }

        [[nodiscard]] static Target& get()
        {
            return *instance;
        }
    };
}