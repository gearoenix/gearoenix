#ifndef GEAROENIX_SYSTEM_RUNTIME_CONFIGURATION_HPP
#define GEAROENIX_SYSTEM_RUNTIME_CONFIGURATION_HPP
#include "../render/engine/gx-rnd-eng-configuration.hpp"

namespace gearoenix::system {
struct RuntimeConfiguration {
    GX_GET_REF_PRV(render::engine::Configuration, render)
};
}
#endif