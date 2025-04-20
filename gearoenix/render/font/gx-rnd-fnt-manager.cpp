#include "gx-rnd-fnt-manager.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "gx-rnd-fnt-font.hpp"

namespace {
gearoenix::render::font::Manager* instance = nullptr;
}

gearoenix::render::font::Manager::Manager()
{
    GX_ASSERT_D(!instance);
    instance = this;
}

gearoenix::render::font::Manager::~Manager()
{
    GX_ASSERT_D(instance);
    instance = nullptr;
}

gearoenix::render::font::Manager& gearoenix::render::font::Manager::get()
{
    GX_ASSERT_D(instance);
    return *instance;
}

std::shared_ptr<gearoenix::render::font::Font> gearoenix::render::font::Manager::get(const std::string& name)
{
    if (const auto search = fonts.find(name); fonts.end() != search) {
        return search->second;
    }
    return fonts.emplace(name, std::make_shared<Font>(name)).first->second;
}
