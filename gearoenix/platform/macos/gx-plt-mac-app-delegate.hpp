#ifndef GEAROENIX_PLATFORM_MACOS_APP_DELEGATE_HPP
#define GEAROENIX_PLATFORM_MACOS_APP_DELEGATE_HPP
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_MACOS
#include "../gx-plt-runtime-configuration.hpp"
#import "gx-plt-mac-view-controller.hpp"
#import <Cocoa/Cocoa.h>

namespace gearoenix::platform {
struct Application;
}

@interface GearoenixPlatformAppDelegate : NSObject <NSApplicationDelegate>
- (void)start:(nonnull gearoenix::platform::Application*)os_app config:(const gearoenix::platform::RuntimeConfiguration&)config;

@property (readonly, nonnull) NSWindow * window;
@property (readonly, nonnull) GearoenixPlatformViewController * view_controller;
@end

#endif
#endif
