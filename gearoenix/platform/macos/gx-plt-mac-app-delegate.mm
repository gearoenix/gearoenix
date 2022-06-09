#include "gx-plt-mac-app-delegate.hpp"
#ifdef GX_PLATFORM_MACOS
#import "../gx-plt-runtime-configuration.hpp"
#import "gx-plt-mac-application.hpp"

@implementation GearoenixPlatformAppDelegate {
}

@synthesize window;
@synthesize view_controller;

- (void)start:(nonnull gearoenix::platform::Application*)os_app config:(const gearoenix::platform::RuntimeConfiguration&)config
{
    const bool is_fullscreen = config.get_fullscreen();
    const NSRect frame_rect = is_fullscreen ? [[NSScreen mainScreen] frame] : NSRect { CGPoint { 0.0f, 0.0f }, CGSize {
                                                                                                                   static_cast<CGFloat>(config.get_window_width()),
                                                                                                                   static_cast<CGFloat>(config.get_window_height()),
                                                                                                               } };
    const std::string& title = config.get_application_name();
    const NSUInteger style_mask = NSWindowStyleMaskTitled
        | NSWindowStyleMaskClosable
        | NSWindowStyleMaskResizable
        | NSWindowStyleMaskMiniaturizable
        | NSWindowStyleMaskUnifiedTitleAndToolbar
        | (is_fullscreen ? NSWindowStyleMaskFullScreen : 0);
    const NSBackingStoreType backing = NSBackingStoreBuffered;
    window = [[NSWindow alloc] initWithContentRect:frame_rect styleMask:style_mask backing:backing defer:YES];
    window.collectionBehavior = NSWindowCollectionBehaviorFullScreenPrimary;
    window.acceptsMouseMovedEvents = YES;
    [window center];
    [window setTitle:[NSString stringWithCString:title.c_str() encoding:[NSString defaultCStringEncoding]]];
    view_controller = [[GearoenixPlatformViewController alloc] init];
    view_controller.main_window = window;
    view_controller.os_app = os_app;
    [window setContentViewController:view_controller];
    [window makeKeyAndOrderFront:self];
    const NSPoint mouse_pos = [window mouseLocationOutsideOfEventStream];
    os_app->get_base().initialize_mouse_position(mouse_pos.x, mouse_pos.y);
}

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
}

- (void)applicationWillTerminate:(NSNotification*)aNotification
{
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender
{
    return YES;
}

@end
#endif
