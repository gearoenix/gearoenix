#ifndef GEAROENIX_RENDER_MATERIAL_TRANSPARENCY_HPP
#define GEAROENIX_RENDER_MATERIAL_TRANSPARENCY_HPP
namespace gearoenix::render::material {
enum struct Transparency : unsigned char {
    Opaque = 1,
    Transparent = 2,
};
}
#endif