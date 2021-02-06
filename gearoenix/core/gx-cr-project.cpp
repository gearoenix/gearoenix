#include "gx-cr-project.hpp"

#include <utility>

gearoenix::core::Project::Project(std::string name) noexcept
    : name(std::move(name))
    , file_address()
    , is_asset(false)
{
}

gearoenix::core::Project::Project(Project&& o) noexcept
    : name(std::move(o.name))
    , file_address(std::move(o.file_address))
    , is_asset(o.is_asset)
    , scenes(std::move(o.scenes))
{
}
