#include "gx-rnd-fnt-manager.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "gx-rnd-fnt-font.hpp"

gearoenix::render::font::Manager::Manager(engine::Engine& e)
    : e(e)
{
}

gearoenix::render::font::Manager::~Manager() = default;

std::shared_ptr<gearoenix::render::font::Font> gearoenix::render::font::Manager::get(const std::string& name)
{
    if (const auto search = fonts.find(name); fonts.end() != search) {
        return search->second;
    }
    return fonts.emplace(name, std::make_shared<Font>(e, name)).first->second;
}
