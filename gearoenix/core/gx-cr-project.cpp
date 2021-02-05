#include "gx-cr-project.hpp"

#include <utility>

gearoenix::core::Project::Project(std::string name) noexcept
    : name(std::move(name))
    , file_address()
    , is_asset(false)
{
}
