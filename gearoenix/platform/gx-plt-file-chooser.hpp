#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace gearoenix::platform::stream {
struct Path;
struct Stream;
}

namespace gearoenix::platform {
void file_chooser_open(std::function<void(stream::Path&&, std::shared_ptr<stream::Stream>&&)>&& on_open, std::function<void()>&& on_cancel, std::string&& title, std::string&& filter);
void file_chooser_save(std::string&& name, std::string&& title, std::string&& filter, std::vector<std::uint8_t>&& content, std::function<void()>&& on_cancel);
void file_chooser_update();
}