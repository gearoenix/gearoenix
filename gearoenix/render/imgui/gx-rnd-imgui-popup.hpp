#pragma once
#include <functional>

namespace gearoenix::render::imgui {
void show_sure_popup(const char* name, bool& is_open, const char* body_text, const std::function<void()>& sure_fun);
}