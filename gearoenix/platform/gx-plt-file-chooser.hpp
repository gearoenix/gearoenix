#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace gearoenix::platform {
void file_chooser_open(std::function<void(std::string&&, std::vector<char>&&)>&& on_open, std::function<void()>&& on_cancel, std::string&& title, std::string&& filter);
void file_chooser_save(std::string&& name, std::string&& title, std::string&& filter, std::vector<char>&& content);
void file_chooser_update();
}