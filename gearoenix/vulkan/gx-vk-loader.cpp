#include "gx-vk-loader.hpp"
#ifdef GX_USE_VULKAN
#include "../system/gx-sys-log.hpp"
#include <string>
#if defined(GX_IN_LINUX) || defined(GX_IN_ANDROID)
#include <dlfcn.h>
#elif defined(GX_IN_WINDOWS)
#include <Windows.h>
#endif

void* gearoenix::vulkan::Loader::lib = nullptr;

void gearoenix::vulkan::Loader::load() noexcept
{
#ifdef GX_IN_WINDOWS
    lib = LoadLibrary("vulkan-1.dll");
#else
    lib = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
#endif
    if (nullptr == lib) {
        GXLOGF("Vulkan library is not available.")
    }
#ifdef GX_IN_WINDOWS
#define VKL(x) x = static_cast<PFN_##x>(GetProcAddress(libvulkan, #x))
#else
#define VKL(x) x = reinterpret_cast<PFN_##x>(dlsym(lib, #x))
#endif
    VKL(vkCreateInstance);
    VKL(vkDestroyInstance);
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
}

#undef VKL
#endif
