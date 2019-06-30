#ifndef GEAROENIX_GLES3_HPP
#define GEAROENIX_GLES3_HPP
#include "../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES3
#ifdef GX_DEBUG_MODE
#define GX_DEBUG_GLES3
#endif
#ifdef GX_PROFILING_MODE
#define GX_GLES3_PROFILING
#endif
#endif
#endif