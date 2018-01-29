#include "phs-anm-animation.hpp"

namespace gearoenix {
namespace physics {
    namespace animation {
        class Once : public Animation {
        protected:
        public:
            Once(const std::function<void(core::Real)>& action, const std::chrono::milliseconds& duration);
            virtual ~Once();
            virtual bool apply(const std::chrono::steady_clock::time_point& now);
        };
    }
}
}
