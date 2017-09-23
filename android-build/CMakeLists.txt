#cmake_minimum_required(VERSION 3.4.1)
#set(APP_GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)
#include_directories(${APP_GLUE_DIR})
#add_library( app-glue STATIC ${APP_GLUE_DIR}/android_native_app_glue.c)
## set(VK_VAL_LAYER_SRC_DIR ${ANDROID_NDK}/sources/third_party/vulkan/src)
#set(APP_SRC_DIR ../)
#add_library(
#    main SHARED
#    ${APP_SRC_DIR}/gearoenix/core/application.cpp
#    ${APP_SRC_DIR}/gearoenix/core/build-configuration.cpp
#    ${APP_SRC_DIR}/gearoenix/core/static.cpp
#    ${APP_SRC_DIR}/gearoenix/directx12/d12-engine.cpp
#    ${APP_SRC_DIR}/gearoenix/render/render-engine.cpp
#    ${APP_SRC_DIR}/gearoenix/system/android/and-app.cpp
#    ${APP_SRC_DIR}/gearoenix/system/android/and-log.cpp
#    ${APP_SRC_DIR}/gearoenix/system/linux/linux-app.cpp
#    ${APP_SRC_DIR}/gearoenix/system/windows/win-app.cpp
#    ${APP_SRC_DIR}/gearoenix/system/windows/win-log.cpp
#    ${APP_SRC_DIR}/gearoenix/system/log.cpp
#    ${APP_SRC_DIR}/gearoenix/system/resource.cpp
#    ${APP_SRC_DIR}/gearoenix/system/sys-app.cpp
#    ${APP_SRC_DIR}/gearoenix/system/sys-file.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/buffer/vk-buf-buffer.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/buffer/vk-buf-uniform.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/command/vk-cmd-buffer.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/command/vk-cmd-pool.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/descriptor/vk-des-pool.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/descriptor/vk-des-set.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/device/vk-dev-logical.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/device/vk-dev-physical.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/image/vk-img-image.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/image/vk-img-view.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/memory/vk-mem-memory.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/memory/vk-mem-pool.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/pipeline/vk-pip-cache.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/pipeline/vk-pip-layout.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/pipeline/vk-pip-pipeline.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/shader/vk-shader-diffuse-colored.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/shader/vk-shader-manager.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/shader/vk-shader.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/sync/vk-sync-fence.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/sync/vk-sync-semaphore.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/vk-check.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/vk-engine.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/vk-framebuffer.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/vk-instance.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/vk-linker.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/vk-render-pass.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/vk-surface.cpp
#    ${APP_SRC_DIR}/gearoenix/vulkan/vk-swapchain.cpp
#)
#set(VK_VAL_LAYER_SRC_DIR ${ANDROID_NDK}/sources/third_party/vulkan/src)
#include_directories(${VK_VAL_LAYER_SRC_DIR}/include)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3 -Wall -Werror \
#  -Wno-unused-variable -frtti -fexceptions")
#target_link_libraries(main app-glue log android vulkan)
