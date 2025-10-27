#include "../gx-plt-build-configuration.hpp"
#if GX_PLATFORM_INTERFACE_SDL
#include "../gx-plt-arguments.hpp"
#include "../gx-plt-main-entry.hpp"

#include <SDL3/SDL_main.h>

int main(int argc, char* argv[])
{
    gearoenix::platform::Arguments::get().parse(argc, argv);
    gearoenix::platform::main_entry();
    return 0;
}

#endif