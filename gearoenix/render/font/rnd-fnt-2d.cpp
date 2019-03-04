#include "rnd-fnt-2d.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../system/stream/sys-stm-local.hpp"
#include "../../system/stream/sys-stm-memory.hpp"
#include "../../system/sys-app.hpp"
#include "../rnd-engine.hpp"
#include "../texture/rnd-txt-texture-2d.hpp"
#include "../texture/rnd-txt-manager.hpp"
#include <cmath>
#include <string>

const int gearoenix::render::font::Font2D::baked_aspect = 1024;
const int gearoenix::render::font::Font2D::first_character = 32;
const int gearoenix::render::font::Font2D::last_character = 126;
const int gearoenix::render::font::Font2D::characters_count = (gearoenix::render::font::Font2D::last_character - gearoenix::render::font::Font2D::first_character) + 1;

gearoenix::render::font::Font2D::Font2D(core::Id my_id, system::stream::Stream* f, Engine* e, core::sync::EndCaller<core::sync::EndCallerIgnore> c)
    : Font(my_id, Shape::D2)
    , letters_properties(characters_count)
{
    f->read<core::Count>();
    f->read(&(letters_properties[0]), letters_properties.size() * sizeof(LetterProperties));
    baked_texture = e->get_system_application()->get_asset_manager()->get_texture_manager()->create<texture::Texture2D>(c);
}

gearoenix::render::font::Font2D::~Font2D()
{
}

const gearoenix::render::font::Font2D::LetterProperties& gearoenix::render::font::Font2D::get_letter_properties(char c) const
{
    return letters_properties[((unsigned int)c) - first_character];
}

const std::shared_ptr<gearoenix::render::texture::Texture2D> gearoenix::render::font::Font2D::get_baked_texture() const
{
    return baked_texture;
}
