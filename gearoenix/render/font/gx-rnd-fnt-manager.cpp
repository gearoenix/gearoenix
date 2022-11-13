#include "gx-rnd-fnt-manager.hpp"
#include "../../platform/gx-plt-application.hpp"
#include "../../platform/stream/gx-plt-stm-asset.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "gx-rnd-fnt-font.hpp"

gearoenix::render::font::Manager::Manager(engine::Engine& e) noexcept
    : e(e)
{
}

gearoenix::render::font::Manager::~Manager() noexcept = default;

std::shared_ptr<gearoenix::render::font::Font> gearoenix::render::font::Manager::get(const std::string& name) noexcept
{
    if (auto search = fonts.find(name); fonts.end() != search)
        return search->second;
    return fonts.emplace(name, std::make_shared<Font>(e, name)).first->second;
}
