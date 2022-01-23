#include "gx-plt-mac-application.hpp"
#ifdef GX_PLATFORM_MACOS
#import "gx-plt-mac-app-delegate.hpp"
#import <Cocoa/Cocoa.h>

gearoenix::platform::Application::Application(GX_MAIN_ENTRY_ARGS_DEF, const RuntimeConfiguration& config) noexcept
    : base(GX_MAIN_ENTRY_ARGS, config)
    , app_delegate([[GearoenixPlatformAppDelegate alloc] init])
{
    [NSApplication sharedApplication];
    [NSApp setDelegate:app_delegate];
    [app_delegate start:this config:config];
}

gearoenix::platform::Application::~Application() noexcept 
{}

void gearoenix::platform::Application::run(core::Application* const core_app) noexcept
{
    base.initialize_core_application(*this, core_app);
    [NSApp run];
}

#endif
