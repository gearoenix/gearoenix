#include "gx-gl-loader.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../core/macro/gx-cr-mcr-stringifier.hpp"
#include "../platform/gx-plt-log.hpp"
#include "gx-gl-constants.hpp"
#include <boost/container/flat_set.hpp>

#ifdef GX_PLATFORM_INTERFACE_SDL2
#include <SDL2/SDL.h>
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

bool gearoenix::gl::load_library() noexcept
{
    if (is_loaded)
        return true;

#ifdef GX_PLATFORM_INTERFACE_SDL2
    if (SDL_GL_LoadLibrary(nullptr) != 0) {
        GX_LOG_D("Failed to load OpenGL shared library through SDL2 library loader.");
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

#ifdef GX_DEBUG_MODE
    if (nullptr != glDebugMessageCallback) {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(debug_callback, nullptr);
    }
#endif

    const char* const extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
    if (nullptr == extensions)
        return false;
    std::istringstream iss_extentions(extensions);
    std::string str_ext;
    while (iss_extentions >> str_ext) {
        GX_LOG_D(str_ext);
        gearoenix_gl_extensions.emplace(str_ext);
    }
    is_loaded = true;
    return true;
}

void gearoenix::gl::unload_library() noexcept
{
    is_loaded = false;

#ifdef GX_PLATFORM_INTERFACE_SDL2
    SDL_GL_UnloadLibrary();
#endif

#define GX_GL_FUNCTION_UNLOAD(name) gl##name = nullptr

    GX_GL_FUNCTION_MAP(GX_GL_FUNCTION_UNLOAD);
}

bool gearoenix::gl::extension_exists(const std::string& ext_name) noexcept
{
    return gearoenix_gl_extensions.contains(ext_name);
}

#ifdef GX_DEBUG_MODE
void GX_GL_APIENTRY_TYPE gearoenix::gl::debug_callback(
    const enumerated source,
    const enumerated t,
    const uint id,
    const enumerated severity,
    const sizei length,
    const char* const message,
    const void* const /*userParam*/)
{
    GX_LOG_E("source: " << source << ", type: " << t << ", id: " << id << ", severity: " << severity << ", length: " << length << ", message: " << message);
}
#endif

#endif