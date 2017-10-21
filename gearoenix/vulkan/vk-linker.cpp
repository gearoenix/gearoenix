#include "vk-linker.hpp"
#ifdef USE_VULKAN
#include "../system/sys-log.hpp"
#include <string>
#if defined(IN_LINUX) || defined(IN_ANDROID)
#include <dlfcn.h>
#elif defined(IN_WINDOWS)
#include <Windows.h>
#endif

gearoenix::render::Linker::Linker()
{
#ifdef IN_WINDOWS
    auto libvulkan = LoadLibrary("vulkan-1.dll");
#else
    void* libvulkan = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
#endif
    if (!libvulkan) {
        LOGF(std::string("Error no Vulkan is available."));
    }
#ifdef IN_WINDOWS
#define VKL(x) x = reinterpret_cast<PFN_##x>(GetProcAddress(libvulkan, #x))
#else
#define VKL(x) x = reinterpret_cast<PFN_##x>(dlsym(libvulkan, #x))
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
#ifdef USE_DEBUG_EXTENTIONS
    VKL(vkCreateDebugReportCallbackEXT);
    VKL(vkDestroyDebugReportCallbackEXT);
    VKL(vkDebugReportMessageEXT);
#endif
}
#undef VKL
#endif
