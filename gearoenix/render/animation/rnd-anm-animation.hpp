#include "../../core/cr-types.hpp"
#include <chrono>
#include <functional>

namespace gearoenix {
namespace math {
    class Transformable;
}
namespace render {
    namespace animation {
        class Animation {
        public:
            typedef enum : core::Id {
                LOOP,
                ONCE,
            } Type;

        protected:
            Type t;
            math::Transformable* o;
            std::function<void(core::Real)> a;
            std::chrono::steady_clock::time_point s;
            Animation(Type t, math::Transformable* object, std::function<void(core::Real)> actioner);

        public:
            virtual ~Animation();
            virtual void start(const std::chrono::steady_clock::time_point& now);
            // true means: delete the animation and animation will not apply anymore
            // false means: animation is working and active
            virtual bool apply(const std::chrono::steady_clock::time_point& now);
            Type get_type() const;
        };
    }
}
}
