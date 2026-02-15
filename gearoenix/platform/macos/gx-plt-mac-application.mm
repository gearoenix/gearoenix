#include "gx-plt-mac-application.hpp"
#if GX_PLATFORM_INTERFACE_APPKIT
#import "gx-plt-mac-app-delegate.hpp"
#import <Cocoa/Cocoa.h>

gearoenix::platform::Application::Application(GX_MAIN_ENTRY_ARGS_DEF, const RuntimeConfiguration& config)
    : base(GX_MAIN_ENTRY_ARGS, config)
    , app_delegate([[GearoenixPlatformAppDelegate alloc] init])
{
    [NSApplication sharedApplication];
    [NSApp setDelegate:app_delegate];
    [app_delegate start:this config:config];
    base.initialize_engine(*this);
}

gearoenix::platform::Application::~Application() { }

void gearoenix::platform::Application::run(core::Application* const core_app)
{
    base.initialize_core_application(*this, core_app);
    [NSApp run];
}

void gearoenix::platform::Application::update() { base.update(); }

#endif
