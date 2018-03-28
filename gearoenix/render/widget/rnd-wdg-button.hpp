#ifndef GEAROEMIX_RENDER_WIDGET_BUTTON_HPP
#define GEAROEMIX_RENDER_WIDGET_BUTTON_HPP
#include "rnd-wdg-widget.hpp"
#include <chrono>
namespace gearoenix {
namespace physics {
    namespace animation {
        class Animation;
    }
}
namespace render {
    namespace widget {
        class Button : public Widget {
            friend class physics::Kernel;

        private:
            const static core::Real press_animation_time;
            const static core::Real press_animation_time_inversed;
            const static core::Real max_scale;
            const static core::Real max_scale_reduction;
            const static core::Real max_scale_inversed;
            const static core::Real max_scale_inversed_reduction;
            const static std::chrono::duration<core::Real> press_animation_time_duration;
            class EffectState {
            public:
                typedef enum {
                    NO_ANIM,
                    IN_MIDDLE_OF_PRESS,
                    PRESSED,
                    IN_MIDDLE_OF_RELEASE,
                } Type;
            };
            //std::mutex effect_locker;
            volatile core::Real current_size = 1.0f;
            EffectState::Type effect_state = EffectState::NO_ANIM;
            std::shared_ptr<physics::animation::Animation> anim;
            std::weak_ptr<physics::animation::Animation> latest_anim;
            void press_effect();
            void release_effect();
            void cancel_effect();

        public:
            Button(core::Id my_id, system::stream::Stream* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c);
            virtual ~Button();
        };
    }
}
}
#endif
