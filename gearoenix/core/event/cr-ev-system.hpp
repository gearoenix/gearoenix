#ifndef GEAROENIX_CORE_EVENT_SYSTEM_SYSTEM_HPP
#define GEAROENIX_CORE_EVENT_SYSTEM_SYSTEM_HPP
#include "../cr-types.hpp"
namespace gearoenix::core::event::system {
struct WindowSizeChangeData {
    Real pre_width = 0.0f;
    Real pre_height = 0.0f;
    Real cur_width = 0.0f;
    Real cur_height = 0.0f;
};
}
#endif