#pragma once
#include "../gx-plt-build-configuration.hpp"
#if GX_PLATFORM_INTERFACE_APPKIT
#include "../gx-plt-runtime-configuration.hpp"
#import "gx-plt-mac-view-controller.hpp"
#import <Cocoa/Cocoa.h>

namespace gearoenix::platform {
struct Application;
}

@interface GearoenixPlatformAppDelegate : NSObject <NSApplicationDelegate>
- (void)start:(nonnull gearoenix::platform::Application*)os_app config:(const gearoenix::platform::RuntimeConfiguration&)config;

@property (readonly, nonnull) NSWindow* window;
@property (readonly, nonnull) GearoenixPlatformViewController* view_controller;
@end

#endif