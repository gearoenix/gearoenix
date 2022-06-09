#ifndef GEAROENIX_PLATFORM_MACOS_VIEW_CONTROLLER_HPP
#define GEAROENIX_PLATFORM_MACOS_VIEW_CONTROLLER_HPP
#include "../gx-plt-build-configuration.hpp"
#ifdef GX_PLATFORM_MACOS
#import <Cocoa/Cocoa.h>
#import <MetalKit/MetalKit.h>

namespace gearoenix::platform {
struct Application;
}

@interface GearoenixPlatformViewController : NSViewController <MTKViewDelegate>
@property (assign, nonnull) NSWindow* main_window;
@property (readonly, nonnull) MTKView* metal_kit_view;
@property (assign, nonnull) gearoenix::platform::Application* os_app;
@end

#endif
#endif
