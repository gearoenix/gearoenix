#ifndef GEAROENIX_CORE_PROJECT_HPP
#define GEAROENIX_CORE_PROJECT_HPP

#include "ecs/gx-cr-ecs-entity.hpp"
#include "macro/gx-cr-mcr-getter-setter.hpp"

namespace gearoenix::core {
struct Project final {
    GX_GET_CREF_PRV(std::string, name);
    GX_GET_CREF_PRV(std::string, file_address);
    GX_GET_CVAL_PRV(bool, is_asset);
    GX_GET_CREF_PRV(std::vector<ecs::Entity::id_t>, scenes);

public:
    explicit Project(std::string name) noexcept;

    Project(const Project&) = delete;
    Project(Project&&) noexcept;
    Project& operator=(const Project&) = delete;
    Project& operator=(Project&&) = delete;
};
}

#endif