#include "gx-gl-loader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/macro/gx-cr-mcr-assert.hpp"
#include "../platform/gx-plt-log.hpp"
#include "gx-gl-constants.hpp"

#include <boost/container/flat_set.hpp>

#include <string_view>

#ifdef GX_PLATFORM_INTERFACE_SDL
#include <SDL3/SDL.h>
#elif defined(GX_PLATFORM_INTERFACE_ANDROID)
#include <EGL/egl.h>
#else
#error "Not implemented for this platform."
#endif

static boost::container::flat_set<std::string> gearoenix_gl_extensions;

#define GX_GL_FUNCTION_DEF(gx_gl_function) gearoenix::gl::gx_gl_function##Fnp gearoenix::gl::gl##gx_gl_function = nullptr

GX_GL_FUNCTION_MAP(GX_GL_FUNCTION_DEF);

GX_GL_OPTIONAL_FUNCTION_MAP(GX_GL_FUNCTION_DEF);

static bool is_loaded = false;

bool gearoenix::gl::load_library()
{
    if (is_loaded)
        return true;

#ifdef GX_PLATFORM_INTERFACE_SDL
    if (!SDL_GL_LoadLibrary(nullptr)) {
        GX_LOG_D("Failed to load OpenGL shared library through SDL library loader.");
        return false;
    }
#define GX_GL_FUNCTION_LOAD_UNCHECKED(name) gl##name = reinterpret_cast<name##Fnp>(SDL_GL_GetProcAddress(GX_STRINGIFY(gl##name)))
#elif defined(GX_PLATFORM_INTERFACE_ANDROID)
#define GX_GL_FUNCTION_LOAD_UNCHECKED(name) gl##name = reinterpret_cast<name##Fnp>(eglGetProcAddress(GX_STRINGIFY(gl##name)))
#else
#error "Not implemented for this platform."
#endif

#define GX_GL_FUNCTION_LOAD_CHECK(name)                                  \
    if (nullptr == gl##name) {                                           \
        GX_LOG_D(GX_STRINGIFY(gl##name) " function pointer not found."); \
        return false;                                                    \
    }

#define GX_GL_FUNCTION_LOAD(name)        \
    GX_GL_FUNCTION_LOAD_UNCHECKED(name); \
    GX_GL_FUNCTION_LOAD_CHECK(name)

    GX_GL_FUNCTION_MAP(GX_GL_FUNCTION_LOAD)
    GX_GL_OPTIONAL_FUNCTION_MAP(GX_GL_FUNCTION_LOAD_UNCHECKED);

    if constexpr (GX_DEBUG_MODE) {
        if (nullptr != glDebugMessageCallback) {
            glEnable(GL_DEBUG_OUTPUT);
            glDebugMessageCallback(debug_callback, nullptr);
        }
    }

    sint num_ext = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &num_ext);
    GX_LOG_D("Number of OpenGL extensions: " << num_ext);
    if (num_ext <= 0) {
        return false;
    }

    for (sint i = 0; i < num_ext; ++i) {
        const auto* const ext = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, static_cast<uint>(i)));
        GX_ASSERT_D(ext != nullptr);
        GX_LOG_D("OpenGL extension [" << i << "] " << ext);
        gearoenix_gl_extensions.emplace(ext);
    }

    is_loaded = true;
    return true;
}

void gearoenix::gl::unload_library()
{
    is_loaded = false;

#ifdef GX_PLATFORM_INTERFACE_SDL
    SDL_GL_UnloadLibrary();
#endif

#define GX_GL_FUNCTION_UNLOAD(name) gl##name = nullptr

    GX_GL_FUNCTION_MAP(GX_GL_FUNCTION_UNLOAD);
}

bool gearoenix::gl::extension_exists(const std::string& ext_name) { return gearoenix_gl_extensions.contains(ext_name); }

#if GX_DEBUG_MODE
static const char* severity_to_string(const gearoenix::gl::enumerated severity)
{
    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        return "high-severity";
    case GL_DEBUG_SEVERITY_MEDIUM:
        return "medium-severity";
    case GL_DEBUG_SEVERITY_LOW:
        return "low-severity";
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        return nullptr;
    default:
        return "unknown";
    }
}

void GX_GL_APIENTRY_TYPE gearoenix::gl::debug_callback(
    const enumerated source, const enumerated t, const uint id, const enumerated severity, const sizei length, const char* const message, const void* const /*userParam*/)
{
    const auto* const srv = severity_to_string(severity);
    if (nullptr == srv) {
        return;
    }

    if (std::string_view(message).contains("Performance:glScissor::Submission has been flushed")) {
        // This is cause by something out of the engine.
        return;
    }

    GX_LOG_E("source: " << source << ", type: " << t << ", id: " << id << ", severity: " << srv << ", length: " << length << ", message: " << message);
}
#endif

#endif