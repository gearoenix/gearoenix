#include "gx-plt-mac-view-controller.hpp"
#ifdef GX_PLATFORM_MACOS
#import "../gx-plt-log.hpp"
#import "gx-plt-mac-application.hpp"
#import <Metal/Metal.h>

@implementation GearoenixPlatformViewController

@synthesize main_window;
@synthesize metal_kit_view;
@synthesize os_app;

- (void)loadView {
    NSArray<id <MTLDevice>> *devices = MTLCopyAllDevices();
    if(0 == devices.count) {
        GX_LOG_F("No Metal device has been found.")
    }
    id <MTLDevice> device = nil;
    id <MTLDevice> raster_device = devices[0];
    id <MTLDevice> low_power_ray_tracer_device = nil;
    for (id <MTLDevice> potential_device in devices) {
        if (!potential_device.lowPower && potential_device.supportsRaytracing) {
            device = potential_device;
            break;
        } else if (potential_device.supportsRaytracing) {
            low_power_ray_tracer_device = potential_device;
        } else if (!potential_device.lowPower) {
            raster_device = potential_device;
        }
    }
    if (nil == device) device = low_power_ray_tracer_device;
    if (nil == device) device = raster_device;
    
    metal_kit_view = [[MTKView alloc] initWithFrame:[main_window contentLayoutRect] device:device];
    [metal_kit_view setClearColor:MTLClearColorMake(0.3f, 0.15f, 0.115f, 1.0f)];
    [metal_kit_view setColorPixelFormat:MTLPixelFormatBGRA8Unorm];
    [metal_kit_view setDepthStencilPixelFormat:MTLPixelFormatDepth32Float_Stencil8];
    [metal_kit_view setSampleCount:1];
    [metal_kit_view addTrackingArea:[
        [NSTrackingArea alloc] initWithRect:[[NSScreen mainScreen] frame]
        options:NSTrackingMouseMoved | NSTrackingActiveAlways | NSTrackingAssumeInside | NSTrackingEnabledDuringMouseDrag
        owner:self userInfo:nil]];
    metal_kit_view.delegate = self;
    metal_kit_view.preferredFramesPerSecond = 60;
    metal_kit_view.device = device;
    self.view = metal_kit_view;
    [self mtkView:metal_kit_view drawableSizeWillChange:metal_kit_view.bounds.size];
}

- (void)viewDidLoad {
    [super viewDidLoad];
}

- (BOOL)commitEditingAndReturnError:(NSError * _Nullable * _Nullable)error {
}

- (void)encodeWithCoder:(nonnull NSCoder *)coder {
}

- (void)drawInMTKView:(nonnull MTKView *)view {
    //NSLog(@"frame render has been started.");
    os_app->update();
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
}

- (void)mouseMoved:(NSEvent *)event {
    os_app->get_base().update_mouse_position(NSEvent.mouseLocation.x, NSEvent.mouseLocation.y);
}

- (void)mouseDragged:(NSEvent *)event {
    os_app->get_base().update_mouse_position(NSEvent.mouseLocation.x, NSEvent.mouseLocation.y);
}

- (void)mouseDown:(NSEvent *)event {
    gearoenix::platform::key::Id ki;
    switch(event.type)
    {
        case NSEventTypeLeftMouseDown:
            ki = gearoenix::platform::key::Id::Left;
            break;
        case NSEventTypeRightMouseDown:
            ki = gearoenix::platform::key::Id::Right;
            break;
        case NSEventTypeOtherMouseDown:
            ki = gearoenix::platform::key::Id::Middle;
            break;
        default:
            GX_LOG_E("Unrecognized mouse button.")
    }
    os_app->get_base().mouse_key(ki, gearoenix::platform::key::Action::Press);
}

- (void)mouseUp:(NSEvent *)event {
    gearoenix::platform::key::Id ki;
    switch(event.type)
    {
        case NSEventTypeLeftMouseUp:
            ki = gearoenix::platform::key::Id::Left;
            break;
        case NSEventTypeRightMouseUp:
            ki = gearoenix::platform::key::Id::Right;
            break;
        case NSEventTypeOtherMouseUp:
            ki = gearoenix::platform::key::Id::Middle;
            break;
        default:
            GX_LOG_E("Unrecognized mouse button.")
    }
    os_app->get_base().mouse_key(ki, gearoenix::platform::key::Action::Release);
}

- (void)keyDown:(NSEvent *)event {
    switch(event.keyCode) {
        case
    }
}

@end

#endif
