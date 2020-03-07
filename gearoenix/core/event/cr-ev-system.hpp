#ifndef GEAROENIX_CORE_EVENT_SYSTEM_SYSTEM_HPP
#define GEAROENIX_CORE_EVENT_SYSTEM_SYSTEM_HPP
#include "../cr-types.hpp"
namespace gearoenix::core::event::system {
struct WindowSizeChangeData {
    double pre_width = 0.0;
    double pre_height = 0.0;
    double cur_width = 0.0;
    double cur_height = 0.0;
};
}
#endif