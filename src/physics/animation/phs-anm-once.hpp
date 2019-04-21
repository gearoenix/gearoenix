#include "phs-anm-animation.hpp"

namespace gearoenix {
namespace physics {
    namespace animation {
        class Once : public Animation {
        protected:
        public:
            Once(
                const std::function<void(core::Real, core::Real)>& action, const std::chrono::milliseconds& duration, std::function<void()> on_delete = []() -> void {});
            virtual ~Once();
            virtual bool apply(const std::chrono::system_clock::time_point& now, const core::Real delta_millisecond);
        };
    }
}
}
