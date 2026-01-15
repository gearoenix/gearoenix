#pragma once
#include "../render/gx-rnd-build-configuration.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../core/gx-cr-build-configuration.hpp"

#if GX_PLATFORM_ANDROID
#define VK_USE_PLATFORM_ANDROID_KHR true
#elif GX_PLATFORM_LINUX
#define VK_USE_PLATFORM_XLIB_KHR true
#elif GX_PLATFORM_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR true
#endif

#if GX_DEBUG_MODE
#define GX_USE_DEBUG_EXTENSIONS true
#endif

#define VK_NO_PROTOTYPES 1
#include <vulkan/vulkan.h>

#if VK_USE_PLATFORM_XLIB_KHR
#define GX_VULKAN_XLIB_FUNCTIONS_MAP(GX_VULKAN_LOADER_MAPPED_FUNCTION) \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateXlibSurfaceKHR);          \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetPhysicalDeviceXlibPresentationSupportKHR);
#else
#define GX_VULKAN_XLIB_FUNCTIONS_MAP(GX_VULKAN_LOADER_MAPPED_FUNCTION)
#endif

#if VK_USE_PLATFORM_XCB_KHR
#define GX_VULKAN_XCB_FUNCTIONS_MAP(GX_VULKAN_LOADER_MAPPED_FUNCTION) \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateXcbSurfaceKHR);          \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetPhysicalDeviceXcbPresentationSupportKHR);
#else
#define GX_VULKAN_XCB_FUNCTIONS_MAP(GX_VULKAN_LOADER_MAPPED_FUNCTION)
#endif

#if VK_USE_PLATFORM_WAYLAND_KHR
#define GX_VULKAN_WAYLAND_FUNCTIONS_MAP(GX_VULKAN_LOADER_MAPPED_FUNCTION) \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateWaylandSurfaceKHR);          \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetPhysicalDeviceWaylandPresentationSupportKHR);
#else
#define GX_VULKAN_WAYLAND_FUNCTIONS_MAP(GX_VULKAN_LOADER_MAPPED_FUNCTION)
#endif

#if VK_USE_PLATFORM_MIR_KHR
#define GX_VULKAN_MIR_FUNCTIONS_MAP(GX_VULKAN_LOADER_MAPPED_FUNCTION) \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateMirSurfaceKHR);          \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetPhysicalDeviceMirPresentationSupportKHR);
#else
#define GX_VULKAN_MIR_FUNCTIONS_MAP(GX_VULKAN_LOADER_MAPPED_FUNCTION)
#endif

#if VK_USE_PLATFORM_ANDROID_KHR
#define GX_VULKAN_ANDROID_FUNCTIONS_MAP(GX_VULKAN_LOADER_MAPPED_FUNCTION) \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateAndroidSurfaceKHR);
#else
#define GX_VULKAN_ANDROID_FUNCTIONS_MAP(GX_VULKAN_LOADER_MAPPED_FUNCTION)
#endif

#if VK_USE_PLATFORM_WIN32_KHR
#define GX_VULKAN_WIN32_FUNCTIONS_MAP(GX_VULKAN_LOADER_MAPPED_FUNCTION) \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateWin32SurfaceKHR);          \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetPhysicalDeviceWin32PresentationSupportKHR);
#else
#define GX_VULKAN_WIN32_FUNCTIONS_MAP(GX_VULKAN_LOADER_MAPPED_FUNCTION)
#endif

#if GX_USE_DEBUG_EXTENSIONS
#define GX_VULKAN_DEBUG_FUNCTIONS_MAP(GX_VULKAN_LOADER_MAPPED_FUNCTION)      \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateDebugUtilsMessengerEXT);       \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkDestroyDebugUtilsMessengerEXT);      \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkSubmitDebugUtilsMessageEXT);         \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkSetDebugUtilsObjectTagEXT);          \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkSetDebugUtilsObjectNameEXT);         \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdBeginDebugUtilsLabelEXT);         \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdEndDebugUtilsLabelEXT);           \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdInsertDebugUtilsLabelEXT);        \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkQueueBeginDebugUtilsLabelEXT);       \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkQueueEndDebugUtilsLabelEXT);         \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkQueueInsertDebugUtilsLabelEXT);
#else
#define GX_VULKAN_DEBUG_FUNCTIONS_MAP(GX_VULKAN_LOADER_MAPPED_FUNCTION)
#endif

#define GX_VULKAN_FUNCTIONS_MAP(GX_VULKAN_LOADER_MAPPED_FUNCTION)                     \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateInstance);                               \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateAccelerationStructureKHR);               \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkDestroyInstance);                              \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkDestroyAccelerationStructureKHR);              \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkEnumerateInstanceVersion);                     \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkEnumeratePhysicalDevices);                     \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetAccelerationStructureDeviceAddressKHR);     \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetPhysicalDeviceFeatures);                    \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetPhysicalDeviceFeatures2);                   \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetPhysicalDeviceFormatProperties);            \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetPhysicalDeviceImageFormatProperties);       \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetPhysicalDeviceProperties);                  \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetPhysicalDeviceProperties2);                 \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties);       \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetPhysicalDeviceMemoryProperties);            \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetInstanceProcAddr);                          \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetDeviceProcAddr);                            \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateDevice);                                 \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkDestroyDevice);                                \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkEnumerateInstanceExtensionProperties);         \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkEnumerateDeviceExtensionProperties);           \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkEnumerateInstanceLayerProperties);             \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkEnumerateDeviceLayerProperties);               \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetDeviceQueue);                               \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkQueueSubmit);                                  \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkQueueWaitIdle);                                \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkDeviceWaitIdle);                               \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkAllocateMemory);                               \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkFreeMemory);                                   \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkMapMemory);                                    \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkUnmapMemory);                                  \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkFlushMappedMemoryRanges);                      \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkInvalidateMappedMemoryRanges);                 \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetDeviceMemoryCommitment);                    \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkBindBufferMemory);                             \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkBindImageMemory);                              \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetBufferMemoryRequirements);                  \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetImageMemoryRequirements);                   \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetImageSparseMemoryRequirements);             \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetPhysicalDeviceSparseImageFormatProperties); \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkQueueBindSparse);                              \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateFence);                                  \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkDestroyFence);                                 \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkResetFences);                                  \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetFenceStatus);                               \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkWaitForFences);                                \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateSemaphore);                              \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkDestroySemaphore);                             \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateEvent);                                  \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkDestroyEvent);                                 \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetEventStatus);                               \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkSetEvent);                                     \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkResetEvent);                                   \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateQueryPool);                              \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkDestroyQueryPool);                             \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetQueryPoolResults);                          \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateBuffer);                                 \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkDestroyBuffer);                                \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateBufferView);                             \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkDestroyBufferView);                            \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateImage);                                  \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkDestroyImage);                                 \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetImageSubresourceLayout);                    \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateImageView);                              \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkDestroyImageView);                             \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateShaderModule);                           \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkDestroyShaderModule);                          \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreatePipelineCache);                          \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkDestroyPipelineCache);                         \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetPipelineCacheData);                         \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkMergePipelineCaches);                          \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateGraphicsPipelines);                      \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateComputePipelines);                       \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateRayTracingPipelinesKHR);                 \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkDestroyPipeline);                              \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreatePipelineLayout);                         \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkDestroyPipelineLayout);                        \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateSampler);                                \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkDestroySampler);                               \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateDescriptorSetLayout);                    \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkDestroyDescriptorSetLayout);                   \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateDescriptorPool);                         \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkDestroyDescriptorPool);                        \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkResetDescriptorPool);                          \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkAllocateDescriptorSets);                       \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkFreeDescriptorSets);                           \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkUpdateDescriptorSets);                         \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateFramebuffer);                            \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkDestroyFramebuffer);                           \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateRenderPass);                             \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkDestroyRenderPass);                            \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetRenderAreaGranularity);                     \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateCommandPool);                            \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkDestroyCommandPool);                           \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkResetCommandPool);                             \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkAllocateCommandBuffers);                       \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkFreeCommandBuffers);                           \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkBeginCommandBuffer);                           \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkEndCommandBuffer);                             \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkResetCommandBuffer);                           \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdBindPipeline);                              \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdSetViewport);                               \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdSetScissor);                                \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdSetLineWidth);                              \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdSetDepthBias);                              \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdSetBlendConstants);                         \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdSetDepthBounds);                            \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdSetStencilCompareMask);                     \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdSetStencilWriteMask);                       \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdSetStencilReference);                       \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdBindDescriptorSets);                        \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdBindIndexBuffer);                           \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdBindVertexBuffers);                         \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdDraw);                                      \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdDrawIndexed);                               \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdDrawIndirect);                              \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdDrawIndexedIndirect);                       \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdDispatch);                                  \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdDispatchIndirect);                          \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdCopyAccelerationStructureKHR);              \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdCopyBuffer);                                \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdCopyImage);                                 \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdBlitImage);                                 \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdBuildAccelerationStructuresKHR);            \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdCopyBufferToImage);                         \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdCopyImageToBuffer);                         \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdUpdateBuffer);                              \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdFillBuffer);                                \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdClearColorImage);                           \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdClearDepthStencilImage);                    \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdClearAttachments);                          \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdResolveImage);                              \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdSetEvent);                                  \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdResetEvent);                                \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdWaitEvents);                                \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdPipelineBarrier);                           \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdBeginQuery);                                \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdEndQuery);                                  \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdResetQueryPool);                            \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdCopyQueryPoolResults);                      \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdPushConstants);                             \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdBeginRenderPass);                           \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdNextSubpass);                               \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdEndRenderPass);                             \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdBeginRendering);                            \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdEndRendering);                              \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdExecuteCommands);                           \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdWriteAccelerationStructuresPropertiesKHR);  \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCmdWriteTimestamp);                            \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkDestroySurfaceKHR);                            \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetAccelerationStructureBuildSizesKHR);        \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetBufferDeviceAddress);                       \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR);           \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);      \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR);           \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR);      \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateSwapchainKHR);                           \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkDestroySwapchainKHR);                          \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetSwapchainImagesKHR);                        \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkAcquireNextImageKHR);                          \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkQueuePresentKHR);                              \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetPhysicalDeviceDisplayPropertiesKHR);        \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetPhysicalDeviceDisplayPlanePropertiesKHR);   \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetDisplayPlaneSupportedDisplaysKHR);          \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetDisplayModePropertiesKHR);                  \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateDisplayModeKHR);                         \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkGetDisplayPlaneCapabilitiesKHR);               \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateDisplayPlaneSurfaceKHR);                 \
    GX_VULKAN_LOADER_MAPPED_FUNCTION(vkCreateSharedSwapchainsKHR);                    \
    GX_VULKAN_XLIB_FUNCTIONS_MAP(GX_VULKAN_LOADER_MAPPED_FUNCTION)                    \
    GX_VULKAN_XCB_FUNCTIONS_MAP(GX_VULKAN_LOADER_MAPPED_FUNCTION)                     \
    GX_VULKAN_WAYLAND_FUNCTIONS_MAP(GX_VULKAN_LOADER_MAPPED_FUNCTION)                 \
    GX_VULKAN_MIR_FUNCTIONS_MAP(GX_VULKAN_LOADER_MAPPED_FUNCTION)                     \
    GX_VULKAN_ANDROID_FUNCTIONS_MAP(GX_VULKAN_LOADER_MAPPED_FUNCTION)                 \
    GX_VULKAN_WIN32_FUNCTIONS_MAP(GX_VULKAN_LOADER_MAPPED_FUNCTION)                   \
    GX_VULKAN_DEBUG_FUNCTIONS_MAP(GX_VULKAN_LOADER_MAPPED_FUNCTION)

#define GX_VULKAN_LOADER_DEF_FUNCTIONS(GX_VULKAN_LOADER_FUNCTION) \
    extern PFN_##GX_VULKAN_LOADER_FUNCTION GX_VULKAN_LOADER_FUNCTION

GX_VULKAN_FUNCTIONS_MAP(GX_VULKAN_LOADER_DEF_FUNCTIONS)

#undef GX_VULKAN_LOADER_DEF_FUNCTIONS

namespace gearoenix::vulkan {
struct Loader {
    Loader() = delete;
    [[nodiscard]] static bool is_loaded();
    [[nodiscard]] static bool load();
    static void load(VkInstance instance);
    static void load(VkDevice device);
    static void unload();
    [[nodiscard]] static PFN_vkVoidFunction get(const char*);
};
}

#endif