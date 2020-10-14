#include "gx-rnd-cmd-manager.hpp"

gearoenix::render::command::Buffer* gearoenix::render::command::Manager::create_primary_command_buffer(std::optional<std::size_t> thread_id) noexcept
{
    return create_command_buffer(Type::Primary, thread_id);
}

gearoenix::render::command::Buffer* gearoenix::render::command::Manager::create_secondary_command_buffer(std::optional<std::size_t> thread_id) noexcept
{
    return create_command_buffer(Type::Secondary, thread_id);
}
