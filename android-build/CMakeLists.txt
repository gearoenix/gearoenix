cmake_minimum_required(VERSION 3.4.1)

set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Ofast -Wall -Werror")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
set(NDK_HELPER_DIR "${ANDROID_NDK}/sources/android/ndk_helper")
set(APP_GLUE "${ANDROID_NDK}/sources/android/native_app_glue")

add_library(
    native-app-glue
    STATIC
    ${APP_GLUE}/android_native_app_glue.c )

add_library(
    ndk-helper
    STATIC
    ${NDK_HELPER_DIR}/gestureDetector.cpp
    ${NDK_HELPER_DIR}/JNIHelper.cpp
    ${NDK_HELPER_DIR}/vecmath.cpp )

set(APP_SRC_DIR ../gearoenix)

add_library(
    native-activity
    SHARED
    ${APP_SRC_DIR}/audio/au-audio.cpp
    ${APP_SRC_DIR}/audio/au-music.cpp
    ${APP_SRC_DIR}/audio/au-speaker.cpp
    ${APP_SRC_DIR}/core/asset/cr-asset-manager.cpp
    ${APP_SRC_DIR}/core/asset/cr-asset.cpp
    ${APP_SRC_DIR}/core/cache/file/cr-cache-file-sparse.cpp
    ${APP_SRC_DIR}/core/cache/file/cr-cache-file.cpp
    ${APP_SRC_DIR}/core/cache/cr-cache-cached.cpp
    ${APP_SRC_DIR}/core/cache/cr-cache-cacher.cpp
    ${APP_SRC_DIR}/core/gc/cr-gc-object.cpp
    ${APP_SRC_DIR}/core/gc/cr-gc-range.cpp
    ${APP_SRC_DIR}/core/gc/cr-gc.cpp
    ${APP_SRC_DIR}/core/list/cr-list-node.cpp
    ${APP_SRC_DIR}/core/list/cr-list.cpp
    ${APP_SRC_DIR}/core/cr-application.cpp
    ${APP_SRC_DIR}/core/cr-build-configuration.cpp
    ${APP_SRC_DIR}/core/cr-end-caller.cpp
    ${APP_SRC_DIR}/core/cr-event.cpp
    ${APP_SRC_DIR}/core/cr-semaphore.cpp
    ${APP_SRC_DIR}/core/cr-static.cpp
    ${APP_SRC_DIR}/directx12/d12-engine.cpp
    ${APP_SRC_DIR}/gles2/buffer/gles2-buf-mesh.cpp
    ${APP_SRC_DIR}/gles2/buffer/gles2-buf-uniform.cpp
    ${APP_SRC_DIR}/gles2/pipeline/gles2-pip-pipeline.cpp
    ${APP_SRC_DIR}/gles2/shader/gles2-shader.cpp
    ${APP_SRC_DIR}/gles2/shader/gles2-shd-depth.cpp
    ${APP_SRC_DIR}/gles2/shader/gles2-shd-directional-colored-matte-nonreflective-shadowless-opaque.cpp
    ${APP_SRC_DIR}/gles2/shader/gles2-shd-directional-colored-speculated-baked-full-opaque.cpp
    ${APP_SRC_DIR}/gles2/shader/gles2-shd-directional-colored-speculated-baked-shadowless-opaque.cpp
    ${APP_SRC_DIR}/gles2/shader/gles2-shd-directional-colored-speculated-nonreflective-shadowless-opaque.cpp
    ${APP_SRC_DIR}/gles2/shader/gles2-shd-directional-d2-speculated-baked-full-opaque.cpp
    ${APP_SRC_DIR}/gles2/shader/gles2-shd-directional-d2-speculated-nonreflective-full-opaque.cpp
    ${APP_SRC_DIR}/gles2/shader/gles2-shd-directional-d2-speculated-nonreflective-shadowless-opaque.cpp
    ${APP_SRC_DIR}/gles2/shader/gles2-shd-resources.cpp
    ${APP_SRC_DIR}/gles2/shader/gles2-shd-shadeless-colored-matte-nonreflective-shadowless-opaque.cpp
    ${APP_SRC_DIR}/gles2/shader/gles2-shd-shadeless-cube-matte-nonreflective-shadowless-opaque.cpp
    ${APP_SRC_DIR}/gles2/shader/gles2-shd-shadeless-d2-matte-nonreflective-shadowless-opaque.cpp
    ${APP_SRC_DIR}/gles2/texture/gles2-txt-2d.cpp
    ${APP_SRC_DIR}/gles2/texture/gles2-txt-cube.cpp
    ${APP_SRC_DIR}/gles2/gles2-engine.cpp
    ${APP_SRC_DIR}/math/math-matrix.cpp
    ${APP_SRC_DIR}/math/math-ray.cpp
    ${APP_SRC_DIR}/math/math-triangle.cpp
    ${APP_SRC_DIR}/math/math-vector.cpp
    ${APP_SRC_DIR}/physics/phs-engine.cpp
    ${APP_SRC_DIR}/physics/phs-kernel.cpp
    ${APP_SRC_DIR}/render/buffer/rnd-buf-buffer.cpp
    ${APP_SRC_DIR}/render/buffer/rnd-buf-manager.cpp
    ${APP_SRC_DIR}/render/buffer/rnd-buf-mesh.cpp
    ${APP_SRC_DIR}/render/buffer/rnd-buf-sub-buffer.cpp
    ${APP_SRC_DIR}/render/buffer/rnd-buf-uniform.cpp
    ${APP_SRC_DIR}/render/camera/rnd-cmr-camera.cpp
    ${APP_SRC_DIR}/render/camera/rnd-cmr-orthographic.cpp
    ${APP_SRC_DIR}/render/camera/rnd-cmr-perspective.cpp
    ${APP_SRC_DIR}/render/light/rnd-lt-light.cpp
    ${APP_SRC_DIR}/render/light/rnd-lt-sun.cpp
    ${APP_SRC_DIR}/render/material/rnd-mat-depth.cpp
    ${APP_SRC_DIR}/render/material/rnd-mat-directional-colored-matte-nonreflective-shadowless-opaque.cpp
    ${APP_SRC_DIR}/render/material/rnd-mat-directional-colored-speculated-baked-full-opaque.cpp
    ${APP_SRC_DIR}/render/material/rnd-mat-directional-colored-speculated-baked-shadowless-opaque.cpp
    ${APP_SRC_DIR}/render/material/rnd-mat-directional-colored-speculated-nonreflective-shadowless-opaque.cpp
    ${APP_SRC_DIR}/render/material/rnd-mat-directional-d2-speculated-baked-full-opaque.cpp
    ${APP_SRC_DIR}/render/material/rnd-mat-directional-d2-speculated-nonreflective-full-opaque.cpp
    ${APP_SRC_DIR}/render/material/rnd-mat-directional-d2-speculated-nonreflective-shadowless-opaque.cpp
    ${APP_SRC_DIR}/render/material/rnd-mat-material.cpp
    ${APP_SRC_DIR}/render/material/rnd-mat-shadeless-colored-matte-nonreflective-shadowless-opaque.cpp
    ${APP_SRC_DIR}/render/material/rnd-mat-shadeless-cube-matte-nonreflective-shadowless-opaque.cpp
    ${APP_SRC_DIR}/render/material/rnd-mat-shadeless-d2-matte-nonreflective-shadowless-opaque.cpp
    ${APP_SRC_DIR}/render/mesh/rnd-msh-mesh.cpp
    ${APP_SRC_DIR}/render/mesh/rnd-msh-occ.cpp
    ${APP_SRC_DIR}/render/model/rnd-mdl-model.cpp
    ${APP_SRC_DIR}/render/pipeline/rnd-pip-manager.cpp
    ${APP_SRC_DIR}/render/pipeline/rnd-pip-pipeline.cpp
    ${APP_SRC_DIR}/render/scene/rnd-scn-scene.cpp
    ${APP_SRC_DIR}/render/shader/rnd-shd-resources.cpp
    ${APP_SRC_DIR}/render/shader/rnd-shd-shader.cpp
    ${APP_SRC_DIR}/render/texture/rnd-txt-lodepng.cpp
    ${APP_SRC_DIR}/render/texture/rnd-txt-png.cpp
    ${APP_SRC_DIR}/render/texture/rnd-txt-texture-2d.cpp
    ${APP_SRC_DIR}/render/texture/rnd-txt-texture-cube.cpp
    ${APP_SRC_DIR}/render/texture/rnd-txt-texture.cpp
    ${APP_SRC_DIR}/render/rnd-engine.cpp
    ${APP_SRC_DIR}/system/android/sys-and-app.cpp
    ${APP_SRC_DIR}/system/android/sys-and-log.cpp
    ${APP_SRC_DIR}/system/glfw/sys-glfw.cpp
    ${APP_SRC_DIR}/system/sdl/sys-sdl-app.cpp
    ${APP_SRC_DIR}/system/sys-app.cpp
    ${APP_SRC_DIR}/system/sys-file.cpp
    ${APP_SRC_DIR}/system/sys-log.cpp
    ${APP_SRC_DIR}/test/test-001-scene-free-camera.cpp
    ${APP_SRC_DIR}/test/test-002-memory-test.cpp
    ${APP_SRC_DIR}/test/test-003-math.cpp
    ${APP_SRC_DIR}/vulkan/buffer/vk-buf-buffer.cpp
    ${APP_SRC_DIR}/vulkan/buffer/vk-buf-manager.cpp
    ${APP_SRC_DIR}/vulkan/buffer/vk-buf-mesh.cpp
    ${APP_SRC_DIR}/vulkan/buffer/vk-buf-sub-buffer.cpp
    ${APP_SRC_DIR}/vulkan/buffer/vk-buf-uniform.cpp
    ${APP_SRC_DIR}/vulkan/command/vk-cmd-buffer.cpp
    ${APP_SRC_DIR}/vulkan/command/vk-cmd-pool.cpp
    ${APP_SRC_DIR}/vulkan/descriptor/vk-des-pool.cpp
    ${APP_SRC_DIR}/vulkan/descriptor/vk-des-set-layout.cpp
    ${APP_SRC_DIR}/vulkan/descriptor/vk-des-set.cpp
    ${APP_SRC_DIR}/vulkan/device/vk-dev-logical.cpp
    ${APP_SRC_DIR}/vulkan/device/vk-dev-physical.cpp
    ${APP_SRC_DIR}/vulkan/image/vk-img-image.cpp
    ${APP_SRC_DIR}/vulkan/image/vk-img-view.cpp
    ${APP_SRC_DIR}/vulkan/memory/vk-mem-manager.cpp
    ${APP_SRC_DIR}/vulkan/memory/vk-mem-memory.cpp
    ${APP_SRC_DIR}/vulkan/memory/vk-mem-sub-memory.cpp
    ${APP_SRC_DIR}/vulkan/pipeline/vk-pip-cache.cpp
    ${APP_SRC_DIR}/vulkan/pipeline/vk-pip-layout.cpp
    ${APP_SRC_DIR}/vulkan/pipeline/vk-pip-manager.cpp
    ${APP_SRC_DIR}/vulkan/pipeline/vk-pip-pipeline.cpp
    ${APP_SRC_DIR}/vulkan/shader/stage/vk-shader-stage-fragment.cpp
    ${APP_SRC_DIR}/vulkan/shader/stage/vk-shader-stage-vertex.cpp
    ${APP_SRC_DIR}/vulkan/shader/stage/vk-shader-stage.cpp
    ${APP_SRC_DIR}/vulkan/shader/vk-shd-resources.cpp
    ${APP_SRC_DIR}/vulkan/sync/vk-sync-fence.cpp
    ${APP_SRC_DIR}/vulkan/sync/vk-sync-semaphore.cpp
    ${APP_SRC_DIR}/vulkan/texture/vk-txt-sampler-2d.cpp
    ${APP_SRC_DIR}/vulkan/texture/vk-txt-texture-2d.cpp
    ${APP_SRC_DIR}/vulkan/texture/vk-txt-texture-3d.cpp
    ${APP_SRC_DIR}/vulkan/texture/vk-txt-texture-cube.cpp
    ${APP_SRC_DIR}/vulkan/vk-check.cpp
    ${APP_SRC_DIR}/vulkan/vk-engine.cpp
    ${APP_SRC_DIR}/vulkan/vk-framebuffer.cpp
    ${APP_SRC_DIR}/vulkan/vk-instance.cpp
    ${APP_SRC_DIR}/vulkan/vk-linker.cpp
    ${APP_SRC_DIR}/vulkan/vk-render-pass.cpp
    ${APP_SRC_DIR}/vulkan/vk-surface.cpp
    ${APP_SRC_DIR}/vulkan/vk-swapchain.cpp
)

target_include_directories(
    ndk-helper
    PRIVATE
    ${APP_GLUE} )

target_include_directories(
    native-activity
    PRIVATE
    ${APP_GLUE}
    ${NDK_HELPER_DIR} )

target_link_libraries(
    native-activity
    android
    native-app-glue
    ndk-helper
    EGL
    GLESv2
    log)
