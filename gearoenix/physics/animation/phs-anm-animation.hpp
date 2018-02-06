#include "../../core/cr-types.hpp"
#include <atomic>
#include <chrono>
#include <functional>

namespace gearoenix {
namespace math {
    class Transformable;
}
namespace physics {
    namespace animation {
        class Animation {
        public:
            typedef enum : core::Id {
                LOOP,
                ONCE,
            } Type;

        protected:
            static volatile std::atomic<core::Id> last_id;
            const core::Id my_id;
            const Type animation_type;
            // milliseconds from start, delta time
            std::function<void(core::Real, core::Real)> action;
            std::function<void()> on_delete;
            bool ended = false;
            std::chrono::steady_clock::time_point start;
            std::chrono::steady_clock::time_point::duration duration;
            std::chrono::steady_clock::time_point end;
            Animation(Type animation_type, const std::function<void(core::Real, core::Real)>& action, const std::chrono::milliseconds& duration, std::function<void()> on_delete);

        public:
            virtual ~Animation();
            virtual void set_start(const std::chrono::steady_clock::time_point& t);
            virtual void set_end(const std::chrono::steady_clock::time_point& t);
            virtual void set_duration(const std::chrono::steady_clock::time_point::duration& d);
            // true means: delete the animation and animation will not apply anymore
            // false means: animation is working and active
            virtual bool apply(const std::chrono::steady_clock::time_point& now, const core::Real delta_millisecond);
            core::Id get_id() const;
            Type get_type() const;
            bool is_ended() const;
            void set_on_delete(std::function<void()> f);
        };
    }
}
}
