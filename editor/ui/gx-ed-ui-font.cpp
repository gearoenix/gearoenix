#include "gx-ed-ui-font.hpp"

#include <algorithm>

namespace {
float current_size_px = gearoenix::editor::ui::font::default_size_px;
}

float gearoenix::editor::ui::font::get_size_px()
{
    return current_size_px;
}

void gearoenix::editor::ui::font::set_size_px(const float size)
{
    current_size_px = std::clamp(size, min_size_px, max_size_px);
}