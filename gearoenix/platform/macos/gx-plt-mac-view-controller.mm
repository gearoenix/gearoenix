#import "gx-plt-mac-view-controller.hpp"
#ifdef GX_PLATFORM_MACOS
#include "../gx-plt-log.hpp"
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
    [metal_kit_view setClearColor:MTLClearColorMake(1, 1, 0, 1)];
    [metal_kit_view setColorPixelFormat:MTLPixelFormatBGRA8Unorm];
    [metal_kit_view setDepthStencilPixelFormat:MTLPixelFormatDepth32Float];
    [metal_kit_view addTrackingArea:[
        [NSTrackingArea alloc] initWithRect:[[NSScreen mainScreen] frame]
        options:NSTrackingMouseMoved | NSTrackingActiveAlways | NSTrackingAssumeInside | NSTrackingEnabledDuringMouseDrag
        owner:self userInfo:nil]];
    metal_kit_view.delegate = self;
    metal_kit_view.preferredFramesPerSecond = 120;
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
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
}

@end

#endif
