#include "gx-rnd-txt-face.hpp"
#include "../../platform/gx-plt-log.hpp"

std::string std::to_string(const gearoenix::render::texture::Face f) noexcept
{
    switch (f) {
    case gearoenix::render::texture::Face::PositiveX:
        return "PositiveX";
    case gearoenix::render::texture::Face::NegativeX:
        return "NegativeX";
    case gearoenix::render::texture::Face::PositiveY:
        return "PositiveY";
    case gearoenix::render::texture::Face::NegativeY:
        return "NegativeY";
    case gearoenix::render::texture::Face::PositiveZ:
        return "PositiveZ";
    case gearoenix::render::texture::Face::NegativeZ:
        return "NegativeZ";
    }
    GX_UNEXPECTED;
}