cmake_minimum_required(VERSION 3.4.1)
set(APP_GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)
include_directories(${APP_GLUE_DIR})
add_library( app-glue STATIC ${APP_GLUE_DIR}/android_native_app_glue.c)
# set(VK_VAL_LAYER_SRC_DIR ${ANDROID_NDK}/sources/third_party/vulkan/src)
set(APP_SRC_DIR ../)
add_library(
    main SHARED
    ${APP_SRC_DIR}/gearoenix/nufrag/core/application.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/core/build-configuration.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/core/static.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/directx12/d12-engine.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/render/render-engine.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/system/android/and-app.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/system/android/and-log.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/system/linux/linux-app.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/system/windows/win-app.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/system/windows/win-log.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/system/log.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/system/resource.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/system/sys-app.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/system/sys-file.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/buffer/vk-buf-buffer.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/buffer/vk-buf-uniform.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/command/vk-cmd-buffer.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/command/vk-cmd-pool.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/descriptor/vk-des-pool.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/descriptor/vk-des-set.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/device/vk-dev-logical.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/device/vk-dev-physical.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/image/vk-img-image.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/image/vk-img-view.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/memory/vk-mem-memory.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/memory/vk-mem-pool.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/pipeline/vk-pip-cache.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/pipeline/vk-pip-layout.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/pipeline/vk-pip-pipeline.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/shader/vk-shader-diffuse-colored.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/shader/vk-shader-manager.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/shader/vk-shader.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/sync/vk-sync-fence.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/sync/vk-sync-semaphore.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/vk-check.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/vk-engine.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/vk-framebuffer.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/vk-instance.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/vk-linker.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/vk-render-pass.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/vk-surface.cpp
    ${APP_SRC_DIR}/gearoenix/nufrag/vulkan/vk-swapchain.cpp
)
set(VK_VAL_LAYER_SRC_DIR ${ANDROID_NDK}/sources/third_party/vulkan/src)
include_directories(${VK_VAL_LAYER_SRC_DIR}/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3 -Wall -Werror \
  -Wno-unused-variable -frtti -fexceptions")
target_link_libraries(main app-glue log android vulkan)
