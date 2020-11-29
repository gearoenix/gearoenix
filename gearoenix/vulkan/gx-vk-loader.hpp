#ifndef GEAROENIX_VULKAN_LINKER_HPP
#define GEAROENIX_VULKAN_LINKER_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../core/gx-cr-build-configuration.hpp"
#ifdef GX_PLATFORM_ANDROID
#define VK_USE_PLATFORM_ANDROID_KHR
#elif defined(GX_PLATFORM_LINUX)
#define VK_USE_PLATFORM_XCB_KHR
#elif defined(GX_PLATFORM_WINDOWS)
#define VK_USE_PLATFORM_WIN32_KHR
#else
#error "It is not implemented yet!"
#endif

#ifdef GX_DEBUG_MODE
#define GX_USE_DEBUG_EXTENSIONS
#endif

#define VK_NO_PROTOTYPES 1
#include <vulkan/vulkan.h>

#include <memory>

namespace gearoenix::platform {
struct Library;
}

namespace gearoenix::vulkan {
struct Loader {
private:
    static std::unique_ptr<platform::Library> lib;

public:
    Loader() noexcept = delete;

    [[nodiscard]] static bool is_loaded() noexcept;
    static void load() noexcept;
    static void unload() noexcept;

#define GX_HELPER(x) static PFN_##x x

    GX_HELPER(vkCreateInstance);
    GX_HELPER(vkDestroyInstance);
    GX_HELPER(vkEnumerateInstanceVersion);
    GX_HELPER(vkEnumeratePhysicalDevices);
    GX_HELPER(vkGetPhysicalDeviceFeatures);
    GX_HELPER(vkGetPhysicalDeviceFormatProperties);
    GX_HELPER(vkGetPhysicalDeviceImageFormatProperties);
    GX_HELPER(vkGetPhysicalDeviceProperties);
    GX_HELPER(vkGetPhysicalDeviceQueueFamilyProperties);
    GX_HELPER(vkGetPhysicalDeviceMemoryProperties);
    GX_HELPER(vkGetInstanceProcAddr);
    GX_HELPER(vkGetDeviceProcAddr);
    GX_HELPER(vkCreateDevice);
    GX_HELPER(vkDestroyDevice);
    GX_HELPER(vkEnumerateInstanceExtensionProperties);
    GX_HELPER(vkEnumerateDeviceExtensionProperties);
    GX_HELPER(vkEnumerateInstanceLayerProperties);
    GX_HELPER(vkEnumerateDeviceLayerProperties);
    GX_HELPER(vkGetDeviceQueue);
    GX_HELPER(vkQueueSubmit);
    GX_HELPER(vkQueueWaitIdle);
    GX_HELPER(vkDeviceWaitIdle);
    GX_HELPER(vkAllocateMemory);
    GX_HELPER(vkFreeMemory);
    GX_HELPER(vkMapMemory);
    GX_HELPER(vkUnmapMemory);
    GX_HELPER(vkFlushMappedMemoryRanges);
    GX_HELPER(vkInvalidateMappedMemoryRanges);
    GX_HELPER(vkGetDeviceMemoryCommitment);
    GX_HELPER(vkBindBufferMemory);
    GX_HELPER(vkBindImageMemory);
    GX_HELPER(vkGetBufferMemoryRequirements);
    GX_HELPER(vkGetImageMemoryRequirements);
    GX_HELPER(vkGetImageSparseMemoryRequirements);
    GX_HELPER(vkGetPhysicalDeviceSparseImageFormatProperties);
    GX_HELPER(vkQueueBindSparse);
    GX_HELPER(vkCreateFence);
    GX_HELPER(vkDestroyFence);
    GX_HELPER(vkResetFences);
    GX_HELPER(vkGetFenceStatus);
    GX_HELPER(vkWaitForFences);
    GX_HELPER(vkCreateSemaphore);
    GX_HELPER(vkDestroySemaphore);
    GX_HELPER(vkCreateEvent);
    GX_HELPER(vkDestroyEvent);
    GX_HELPER(vkGetEventStatus);
    GX_HELPER(vkSetEvent);
    GX_HELPER(vkResetEvent);
    GX_HELPER(vkCreateQueryPool);
    GX_HELPER(vkDestroyQueryPool);
    GX_HELPER(vkGetQueryPoolResults);
    GX_HELPER(vkCreateBuffer);
    GX_HELPER(vkDestroyBuffer);
    GX_HELPER(vkCreateBufferView);
    GX_HELPER(vkDestroyBufferView);
    GX_HELPER(vkCreateImage);
    GX_HELPER(vkDestroyImage);
    GX_HELPER(vkGetImageSubresourceLayout);
    GX_HELPER(vkCreateImageView);
    GX_HELPER(vkDestroyImageView);
    GX_HELPER(vkCreateShaderModule);
    GX_HELPER(vkDestroyShaderModule);
    GX_HELPER(vkCreatePipelineCache);
    GX_HELPER(vkDestroyPipelineCache);
    GX_HELPER(vkGetPipelineCacheData);
    GX_HELPER(vkMergePipelineCaches);
    GX_HELPER(vkCreateGraphicsPipelines);
    GX_HELPER(vkCreateComputePipelines);
    GX_HELPER(vkDestroyPipeline);
    GX_HELPER(vkCreatePipelineLayout);
    GX_HELPER(vkDestroyPipelineLayout);
    GX_HELPER(vkCreateSampler);
    GX_HELPER(vkDestroySampler);
    GX_HELPER(vkCreateDescriptorSetLayout);
    GX_HELPER(vkDestroyDescriptorSetLayout);
    GX_HELPER(vkCreateDescriptorPool);
    GX_HELPER(vkDestroyDescriptorPool);
    GX_HELPER(vkResetDescriptorPool);
    GX_HELPER(vkAllocateDescriptorSets);
    GX_HELPER(vkFreeDescriptorSets);
    GX_HELPER(vkUpdateDescriptorSets);
    GX_HELPER(vkCreateFramebuffer);
    GX_HELPER(vkDestroyFramebuffer);
    GX_HELPER(vkCreateRenderPass);
    GX_HELPER(vkDestroyRenderPass);
    GX_HELPER(vkGetRenderAreaGranularity);
    GX_HELPER(vkCreateCommandPool);
    GX_HELPER(vkDestroyCommandPool);
    GX_HELPER(vkResetCommandPool);
    GX_HELPER(vkAllocateCommandBuffers);
    GX_HELPER(vkFreeCommandBuffers);
    GX_HELPER(vkBeginCommandBuffer);
    GX_HELPER(vkEndCommandBuffer);
    GX_HELPER(vkResetCommandBuffer);
    GX_HELPER(vkCmdBindPipeline);
    GX_HELPER(vkCmdSetViewport);
    GX_HELPER(vkCmdSetScissor);
    GX_HELPER(vkCmdSetLineWidth);
    GX_HELPER(vkCmdSetDepthBias);
    GX_HELPER(vkCmdSetBlendConstants);
    GX_HELPER(vkCmdSetDepthBounds);
    GX_HELPER(vkCmdSetStencilCompareMask);
    GX_HELPER(vkCmdSetStencilWriteMask);
    GX_HELPER(vkCmdSetStencilReference);
    GX_HELPER(vkCmdBindDescriptorSets);
    GX_HELPER(vkCmdBindIndexBuffer);
    GX_HELPER(vkCmdBindVertexBuffers);
    GX_HELPER(vkCmdDraw);
    GX_HELPER(vkCmdDrawIndexed);
    GX_HELPER(vkCmdDrawIndirect);
    GX_HELPER(vkCmdDrawIndexedIndirect);
    GX_HELPER(vkCmdDispatch);
    GX_HELPER(vkCmdDispatchIndirect);
    GX_HELPER(vkCmdCopyBuffer);
    GX_HELPER(vkCmdCopyImage);
    GX_HELPER(vkCmdBlitImage);
    GX_HELPER(vkCmdCopyBufferToImage);
    GX_HELPER(vkCmdCopyImageToBuffer);
    GX_HELPER(vkCmdUpdateBuffer);
    GX_HELPER(vkCmdFillBuffer);
    GX_HELPER(vkCmdClearColorImage);
    GX_HELPER(vkCmdClearDepthStencilImage);
    GX_HELPER(vkCmdClearAttachments);
    GX_HELPER(vkCmdResolveImage);
    GX_HELPER(vkCmdSetEvent);
    GX_HELPER(vkCmdResetEvent);
    GX_HELPER(vkCmdWaitEvents);
    GX_HELPER(vkCmdPipelineBarrier);
    GX_HELPER(vkCmdBeginQuery);
    GX_HELPER(vkCmdEndQuery);
    GX_HELPER(vkCmdResetQueryPool);
    GX_HELPER(vkCmdWriteTimestamp);
    GX_HELPER(vkCmdCopyQueryPoolResults);
    GX_HELPER(vkCmdPushConstants);
    GX_HELPER(vkCmdBeginRenderPass);
    GX_HELPER(vkCmdNextSubpass);
    GX_HELPER(vkCmdEndRenderPass);
    GX_HELPER(vkCmdExecuteCommands);

    // VK_KHR_surface
    GX_HELPER(vkDestroySurfaceKHR);
    GX_HELPER(vkGetPhysicalDeviceSurfaceSupportKHR);
    GX_HELPER(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    GX_HELPER(vkGetPhysicalDeviceSurfaceFormatsKHR);
    GX_HELPER(vkGetPhysicalDeviceSurfacePresentModesKHR);

    // VK_KHR_swapchain
    GX_HELPER(vkCreateSwapchainKHR);
    GX_HELPER(vkDestroySwapchainKHR);
    GX_HELPER(vkGetSwapchainImagesKHR);
    GX_HELPER(vkAcquireNextImageKHR);
    GX_HELPER(vkQueuePresentKHR);

    // VK_KHR_display
    GX_HELPER(vkGetPhysicalDeviceDisplayPropertiesKHR);
    GX_HELPER(vkGetPhysicalDeviceDisplayPlanePropertiesKHR);
    GX_HELPER(vkGetDisplayPlaneSupportedDisplaysKHR);
    GX_HELPER(vkGetDisplayModePropertiesKHR);
    GX_HELPER(vkCreateDisplayModeKHR);
    GX_HELPER(vkGetDisplayPlaneCapabilitiesKHR);
    GX_HELPER(vkCreateDisplayPlaneSurfaceKHR);

    // VK_KHR_display_swapchain
    GX_HELPER(vkCreateSharedSwapchainsKHR);

#ifdef VK_USE_PLATFORM_XLIB_KHR
    // VK_KHR_xlib_surface
    GX_HELPER(vkCreateXlibSurfaceKHR);
    GX_HELPER(vkGetPhysicalDeviceXlibPresentationSupportKHR);
#endif

#ifdef VK_USE_PLATFORM_XCB_KHR
    // VK_KHR_xcb_surface
    GX_HELPER(vkCreateXcbSurfaceKHR);
    GX_HELPER(vkGetPhysicalDeviceXcbPresentationSupportKHR);
#endif

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    // VK_KHR_wayland_surface
    GX_HELPER(vkCreateWaylandSurfaceKHR);
    GX_HELPER(vkGetPhysicalDeviceWaylandPresentationSupportKHR);
#endif

#ifdef VK_USE_PLATFORM_MIR_KHR
    // VK_KHR_mir_surface
    GX_HELPER(vkCreateMirSurfaceKHR);
    GX_HELPER(vkGetPhysicalDeviceMirPresentationSupportKHR);
#endif

#ifdef VK_USE_PLATFORM_ANDROID_KHR
    // VK_KHR_android_surface
    GX_HELPER(vkCreateAndroidSurfaceKHR);
#endif

#ifdef VK_USE_PLATFORM_WIN32_KHR
    // VK_KHR_win32_surface
    GX_HELPER(vkCreateWin32SurfaceKHR);
    GX_HELPER(vkGetPhysicalDeviceWin32PresentationSupportKHR);
#endif

#ifdef GX_USE_DEBUG_EXTENSIONS
#include <vulkan/vk_sdk_platform.h>
    // VK_EXT_debug_report
    GX_HELPER(vkCreateDebugReportCallbackEXT);
    GX_HELPER(vkDestroyDebugReportCallbackEXT);
    GX_HELPER(vkDebugReportMessageEXT);
#endif
};
}

#undef GX_HELPER

#endif
#endif
