SET(GX_ENGINE_VULKAN_SRC_DIR ${GX_ENGINE_SRC_DIR}/vulkan)
SET(GX_ENGINE_VULKAN_SRC
        ${GX_ENGINE_VULKAN_SRC_DIR}/buffer/gx-vk-buf-buffer.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/buffer/gx-vk-buf-buffer.hpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/buffer/gx-vk-buf-dynamic.cpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/buffer/gx-vk-buf-dynamic.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/buffer/gx-vk-buf-manager.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/buffer/gx-vk-buf-manager.hpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/buffer/gx-vk-buf-uniform.cpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/buffer/gx-vk-buf-uniform.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/command/gx-vk-cmd-buffer.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/command/gx-vk-cmd-buffer.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/command/gx-vk-cmd-manager.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/command/gx-vk-cmd-manager.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/command/gx-vk-cmd-pool.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/command/gx-vk-cmd-pool.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/command/gx-vk-cmd-type.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/descriptor/gx-vk-des-manager.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/descriptor/gx-vk-des-manager.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/descriptor/gx-vk-des-pool.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/descriptor/gx-vk-des-pool.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/descriptor/gx-vk-des-set.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/descriptor/gx-vk-des-set.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/descriptor/gx-vk-des-set-layout.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/descriptor/gx-vk-des-set-layout.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/device/gx-vk-dev-logical.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/device/gx-vk-dev-logical.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/device/gx-vk-dev-physical.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/device/gx-vk-dev-physical.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/engine/gx-vk-eng-engine.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/engine/gx-vk-eng-engine.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/engine/gx-vk-eng-frame.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/engine/gx-vk-eng-frame.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/image/gx-vk-img-image.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/image/gx-vk-img-image.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/image/gx-vk-img-manager.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/image/gx-vk-img-manager.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/image/gx-vk-img-view.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/image/gx-vk-img-view.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/memory/gx-vk-mem-manager.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/memory/gx-vk-mem-manager.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/memory/gx-vk-mem-memory.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/memory/gx-vk-mem-memory.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/memory/gx-vk-mem-place.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/mesh/gx-vk-msh-accel.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/mesh/gx-vk-msh-accel.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/mesh/gx-vk-msh-accel-component.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/mesh/gx-vk-msh-accel-component.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/mesh/gx-vk-msh-accel-manager.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/mesh/gx-vk-msh-accel-manager.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/mesh/gx-vk-msh-manager.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/mesh/gx-vk-msh-manager.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/mesh/gx-vk-msh-raster.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/mesh/gx-vk-msh-raster.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/mesh/gx-vk-msh-raster-component.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/mesh/gx-vk-msh-raster-component.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/mesh/gx-vk-msh-raster-manager.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/mesh/gx-vk-msh-raster-manager.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/pipeline/gx-vk-pip-cache.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/pipeline/gx-vk-pip-cache.hpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/pipeline/gx-vk-pip-layout.cpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/pipeline/gx-vk-pip-layout.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/pipeline/gx-vk-pip-manager.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/pipeline/gx-vk-pip-manager.hpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/pipeline/gx-vk-pip-pipeline.cpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/pipeline/gx-vk-pip-pipeline.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/query/gx-vk-qry-pool.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/query/gx-vk-qry-pool.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/queue/gx-vk-qu-graph.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/queue/gx-vk-qu-graph.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/queue/gx-vk-qu-node.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/queue/gx-vk-qu-node.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/queue/gx-vk-qu-queue.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/queue/gx-vk-qu-queue.hpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/sampler/gx-vk-smp-manager.cpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/sampler/gx-vk-smp-manager.hpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/sampler/gx-vk-smp-sampler.cpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/sampler/gx-vk-smp-sampler.hpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/shader/stage/gx-vk-shader-stage-fragment.cpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/shader/stage/gx-vk-shader-stage-fragment.hpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/shader/stage/gx-vk-shader-stage-vertex.cpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/shader/stage/gx-vk-shader-stage-vertex.hpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/shader/stage/gx-vk-shader-stage.cpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/shader/stage/gx-vk-shader-stage.hpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/shader/gx-vk-shd-resources.cpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/shader/gx-vk-shd-resources.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/sync/gx-vk-sync-fence.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/sync/gx-vk-sync-fence.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/sync/gx-vk-sync-semaphore.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/sync/gx-vk-sync-semaphore.hpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/texture/gx-vk-txt-2d.cpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/texture/gx-vk-txt-2d.hpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/texture/gx-vk-txt-3d.cpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/texture/gx-vk-txt-3d.hpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/texture/gx-vk-txt-cube.cpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/texture/gx-vk-txt-cube.hpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/texture/gx-vk-txt-main-target.cpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/texture/gx-vk-txt-main-target.hpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/texture/gx-vk-txt-target.cpp
#        ${GX_ENGINE_VULKAN_SRC_DIR}/texture/gx-vk-txt-target.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/gx-vk-check.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/gx-vk-check.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/gx-vk-framebuffer.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/gx-vk-framebuffer.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/gx-vk-imgui-manager.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/gx-vk-imgui-manager.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/gx-vk-instance.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/gx-vk-instance.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/gx-vk-loader.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/gx-vk-loader.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/gx-vk-marker.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/gx-vk-marker.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/gx-vk-render-pass.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/gx-vk-render-pass.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/gx-vk-surface.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/gx-vk-surface.hpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/gx-vk-swapchain.cpp
        ${GX_ENGINE_VULKAN_SRC_DIR}/gx-vk-swapchain.hpp
        PARENT_SCOPE)