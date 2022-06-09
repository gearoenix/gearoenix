#ifndef GEAROENIX_GL_TYPES_HPP
#define GEAROENIX_GL_TYPES_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include <cstdint>
#include <cstdlib>

namespace gearoenix::gl {
typedef std::uint8_t boolean;
typedef std::uint32_t bitfield;
typedef std::uint32_t enumerated;
typedef std::int32_t sint;
typedef std::uint32_t sizei;
typedef std::size_t sizeiptr;
typedef std::int16_t sshort;
typedef std::uint8_t ubyte;
typedef std::uint32_t uint;
typedef std::uint16_t ushort;
}

#endif
#endif