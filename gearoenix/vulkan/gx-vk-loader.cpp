#include "gx-vk-loader.hpp"
#ifdef GX_RENDER_VULKAN_ENABLED
#include "../platform/gx-plt-library.hpp"
#include "../platform/gx-plt-log.hpp"

std::unique_ptr<gearoenix::platform::Library> gearoenix::vulkan::Loader::lib = nullptr;

#define GX_HELPER(x) PFN_##x gearoenix::vulkan::Loader::x = nullptr
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
// VK_EXT_debug_report
GX_HELPER(vkCreateDebugReportCallbackEXT);
GX_HELPER(vkDestroyDebugReportCallbackEXT);
GX_HELPER(vkDebugReportMessageEXT);
#endif

#undef GX_HELPER

bool gearoenix::vulkan::Loader::is_loaded() noexcept
{
    return vkCreateInstance != nullptr;
}

void gearoenix::vulkan::Loader::load() noexcept
{
    if (is_loaded())
        return;
    lib = std::unique_ptr<platform::Library>(platform::Library::construct(
#ifdef GX_PLATFORM_WINDOWS
        "vulkan-1.dll"
#else
        "libvulkan.so"
#endif
        ));

    if (nullptr == lib) {
        GX_LOG_D("Vulkan library is not available.")
        return;
    }

#define VKL(x) x = lib->load<PFN_##x>(#x)

    VKL(vkCreateInstance);
    VKL(vkDestroyInstance);
    VKL(vkEnumerateInstanceVersion);
    VKL(vkEnumeratePhysicalDevices);
    VKL(vkGetPhysicalDeviceFeatures);
    VKL(vkGetPhysicalDeviceFormatProperties);
    VKL(vkGetPhysicalDeviceImageFormatProperties);
    VKL(vkGetPhysicalDeviceProperties);
    VKL(vkGetPhysicalDeviceQueueFamilyProperties);
    VKL(vkGetPhysicalDeviceMemoryProperties);
    VKL(vkGetInstanceProcAddr);
    VKL(vkGetDeviceProcAddr);
    VKL(vkCreateDevice);
    VKL(vkDestroyDevice);
    VKL(vkEnumerateInstanceExtensionProperties);
    VKL(vkEnumerateDeviceExtensionProperties);
    VKL(vkEnumerateInstanceLayerProperties);
    VKL(vkEnumerateDeviceLayerProperties);
    VKL(vkGetDeviceQueue);
    VKL(vkQueueSubmit);
    VKL(vkQueueWaitIdle);
    VKL(vkDeviceWaitIdle);
    VKL(vkAllocateMemory);
    VKL(vkFreeMemory);
    VKL(vkMapMemory);
    VKL(vkUnmapMemory);
    VKL(vkFlushMappedMemoryRanges);
    VKL(vkInvalidateMappedMemoryRanges);
    VKL(vkGetDeviceMemoryCommitment);
    VKL(vkBindBufferMemory);
    VKL(vkBindImageMemory);
    VKL(vkGetBufferMemoryRequirements);
    VKL(vkGetImageMemoryRequirements);
    VKL(vkGetImageSparseMemoryRequirements);
    VKL(vkGetPhysicalDeviceSparseImageFormatProperties);
    VKL(vkQueueBindSparse);
    VKL(vkCreateFence);
    VKL(vkDestroyFence);
    VKL(vkResetFences);
    VKL(vkGetFenceStatus);
    VKL(vkWaitForFences);
    VKL(vkCreateSemaphore);
    VKL(vkDestroySemaphore);
    VKL(vkCreateEvent);
    VKL(vkDestroyEvent);
    VKL(vkGetEventStatus);
    VKL(vkSetEvent);
    VKL(vkResetEvent);
    VKL(vkCreateQueryPool);
    VKL(vkDestroyQueryPool);
    VKL(vkGetQueryPoolResults);
    VKL(vkCreateBuffer);
    VKL(vkDestroyBuffer);
    VKL(vkCreateBufferView);
    VKL(vkDestroyBufferView);
    VKL(vkCreateImage);
    VKL(vkDestroyImage);
    VKL(vkGetImageSubresourceLayout);
    VKL(vkCreateImageView);
    VKL(vkDestroyImageView);
    VKL(vkCreateShaderModule);
    VKL(vkDestroyShaderModule);
    VKL(vkCreatePipelineCache);
    VKL(vkDestroyPipelineCache);
    VKL(vkGetPipelineCacheData);
    VKL(vkMergePipelineCaches);
    VKL(vkCreateGraphicsPipelines);
    VKL(vkCreateComputePipelines);
    VKL(vkDestroyPipeline);
    VKL(vkCreatePipelineLayout);
    VKL(vkDestroyPipelineLayout);
    VKL(vkCreateSampler);
    VKL(vkDestroySampler);
    VKL(vkCreateDescriptorSetLayout);
    VKL(vkDestroyDescriptorSetLayout);
    VKL(vkCreateDescriptorPool);
    VKL(vkDestroyDescriptorPool);
    VKL(vkResetDescriptorPool);
    VKL(vkAllocateDescriptorSets);
    VKL(vkFreeDescriptorSets);
    VKL(vkUpdateDescriptorSets);
    VKL(vkCreateFramebuffer);
    VKL(vkDestroyFramebuffer);
    VKL(vkCreateRenderPass);
    VKL(vkDestroyRenderPass);
    VKL(vkGetRenderAreaGranularity);
    VKL(vkCreateCommandPool);
    VKL(vkDestroyCommandPool);
    VKL(vkResetCommandPool);
    VKL(vkAllocateCommandBuffers);
    VKL(vkFreeCommandBuffers);
    VKL(vkBeginCommandBuffer);
    VKL(vkEndCommandBuffer);
    VKL(vkResetCommandBuffer);
    VKL(vkCmdBindPipeline);
    VKL(vkCmdSetViewport);
    VKL(vkCmdSetScissor);
    VKL(vkCmdSetLineWidth);
    VKL(vkCmdSetDepthBias);
    VKL(vkCmdSetBlendConstants);
    VKL(vkCmdSetDepthBounds);
    VKL(vkCmdSetStencilCompareMask);
    VKL(vkCmdSetStencilWriteMask);
    VKL(vkCmdSetStencilReference);
    VKL(vkCmdBindDescriptorSets);
    VKL(vkCmdBindIndexBuffer);
    VKL(vkCmdBindVertexBuffers);
    VKL(vkCmdDraw);
    VKL(vkCmdDrawIndexed);
    VKL(vkCmdDrawIndirect);
    VKL(vkCmdDrawIndexedIndirect);
    VKL(vkCmdDispatch);
    VKL(vkCmdDispatchIndirect);
    VKL(vkCmdCopyBuffer);
    VKL(vkCmdCopyImage);
    VKL(vkCmdBlitImage);
    VKL(vkCmdCopyBufferToImage);
    VKL(vkCmdCopyImageToBuffer);
    VKL(vkCmdUpdateBuffer);
    VKL(vkCmdFillBuffer);
    VKL(vkCmdClearColorImage);
    VKL(vkCmdClearDepthStencilImage);
    VKL(vkCmdClearAttachments);
    VKL(vkCmdResolveImage);
    VKL(vkCmdSetEvent);
    VKL(vkCmdResetEvent);
    VKL(vkCmdWaitEvents);
    VKL(vkCmdPipelineBarrier);
    VKL(vkCmdBeginQuery);
    VKL(vkCmdEndQuery);
    VKL(vkCmdResetQueryPool);
    VKL(vkCmdWriteTimestamp);
    VKL(vkCmdCopyQueryPoolResults);
    VKL(vkCmdPushConstants);
    VKL(vkCmdBeginRenderPass);
    VKL(vkCmdNextSubpass);
    VKL(vkCmdEndRenderPass);
    VKL(vkCmdExecuteCommands);
    VKL(vkDestroySurfaceKHR);
    VKL(vkGetPhysicalDeviceSurfaceSupportKHR);
    VKL(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    VKL(vkGetPhysicalDeviceSurfaceFormatsKHR);
    VKL(vkGetPhysicalDeviceSurfacePresentModesKHR);
    VKL(vkCreateSwapchainKHR);
    VKL(vkDestroySwapchainKHR);
    VKL(vkGetSwapchainImagesKHR);
    VKL(vkAcquireNextImageKHR);
    VKL(vkQueuePresentKHR);
    VKL(vkGetPhysicalDeviceDisplayPropertiesKHR);
    VKL(vkGetPhysicalDeviceDisplayPlanePropertiesKHR);
    VKL(vkGetDisplayPlaneSupportedDisplaysKHR);
    VKL(vkGetDisplayModePropertiesKHR);
    VKL(vkCreateDisplayModeKHR);
    VKL(vkGetDisplayPlaneCapabilitiesKHR);
    VKL(vkCreateDisplayPlaneSurfaceKHR);
    VKL(vkCreateSharedSwapchainsKHR);
#ifdef VK_USE_PLATFORM_XLIB_KHR
    VKL(vkCreateXlibSurfaceKHR);
    VKL(vkGetPhysicalDeviceXlibPresentationSupportKHR);
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
    VKL(vkCreateXcbSurfaceKHR);
    VKL(vkGetPhysicalDeviceXcbPresentationSupportKHR);
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    VKL(vkCreateWaylandSurfaceKHR);
    VKL(vkGetPhysicalDeviceWaylandPresentationSupportKHR);
#endif
#ifdef VK_USE_PLATFORM_MIR_KHR
    VKL(vkCreateMirSurfaceKHR);
    VKL(vkGetPhysicalDeviceMirPresentationSupportKHR);
#endif
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    VKL(vkCreateAndroidSurfaceKHR);
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
    VKL(vkCreateWin32SurfaceKHR);
    VKL(vkGetPhysicalDeviceWin32PresentationSupportKHR);
#endif
#ifdef GX_USE_DEBUG_EXTENSIONS
    VKL(vkCreateDebugReportCallbackEXT);
    VKL(vkDestroyDebugReportCallbackEXT);
    VKL(vkDebugReportMessageEXT);
#endif
#undef VKL
}

void gearoenix::vulkan::Loader::load([[maybe_unused]] VkInstance instance) noexcept
{
#define VKL(fun_name)                                                                            \
    if (fun_name == nullptr) {                                                                   \
        fun_name = reinterpret_cast<PFN_##fun_name>(vkGetInstanceProcAddr(instance, #fun_name)); \
    }
#ifdef GX_USE_DEBUG_EXTENSIONS
    VKL(vkCreateDebugReportCallbackEXT)
    VKL(vkDestroyDebugReportCallbackEXT)
    VKL(vkDebugReportMessageEXT)
#endif
#undef VKL
}

void gearoenix::vulkan::Loader::load([[maybe_unused]] VkDevice device) noexcept
{
#define VKL(fun_name)                                                                        \
    if (fun_name == nullptr) {                                                               \
        fun_name = reinterpret_cast<PFN_##fun_name>(vkGetDeviceProcAddr(device, #fun_name)); \
    }
#ifdef GX_USE_DEBUG_EXTENSIONS
    VKL(vkDebugMarkerSetObjectTagEXT)
    VKL(vkDebugMarkerSetObjectNameEXT)
    VKL(vkCmdDebugMarkerBeginEXT)
    VKL(vkCmdDebugMarkerEndEXT)
    VKL(vkCmdDebugMarkerInsertEXT)
#endif
}

void gearoenix::vulkan::Loader::unload() noexcept
{
    lib = nullptr;
}

#endif
