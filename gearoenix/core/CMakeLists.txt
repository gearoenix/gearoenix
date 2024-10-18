SET(GX_ENGINE_CORE_SRC_DIR ${GX_ENGINE_SRC_DIR}/core)
SET(GX_ENGINE_CORE_SRC
        ${GX_ENGINE_CORE_SRC_DIR}/allocator/gx-cr-alc-range.cpp
        ${GX_ENGINE_CORE_SRC_DIR}/allocator/gx-cr-alc-range.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/allocator/gx-cr-alc-same-size-block.cpp
        ${GX_ENGINE_CORE_SRC_DIR}/allocator/gx-cr-alc-same-size-block.cpp
        ${GX_ENGINE_CORE_SRC_DIR}/allocator/gx-cr-alc-same-size-block.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/allocator/gx-cr-alc-shared-array.cpp
        ${GX_ENGINE_CORE_SRC_DIR}/allocator/gx-cr-alc-shared-array.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/allocator/gx-cr-alc-static-block.cpp
        ${GX_ENGINE_CORE_SRC_DIR}/allocator/gx-cr-alc-static-block.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/ecs/gx-cr-ecs-archetype.cpp
        ${GX_ENGINE_CORE_SRC_DIR}/ecs/gx-cr-ecs-archetype.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/ecs/gx-cr-ecs-component.cpp
        ${GX_ENGINE_CORE_SRC_DIR}/ecs/gx-cr-ecs-component.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/ecs/gx-cr-ecs-condition.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/ecs/gx-cr-ecs-entity.cpp
        ${GX_ENGINE_CORE_SRC_DIR}/ecs/gx-cr-ecs-entity.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/ecs/gx-cr-ecs-types.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/ecs/gx-cr-ecs-world.cpp
        ${GX_ENGINE_CORE_SRC_DIR}/ecs/gx-cr-ecs-world.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/event/gx-cr-ev-button.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/event/gx-cr-ev-engine.cpp
        ${GX_ENGINE_CORE_SRC_DIR}/event/gx-cr-ev-engine.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/event/gx-cr-ev-event.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/event/gx-cr-ev-gesture.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/event/gx-cr-ev-id.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/event/gx-cr-ev-listener.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/event/gx-cr-ev-movement.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/event/gx-cr-ev-point.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/event/gx-cr-ev-system.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/event/gx-cr-ev-touch.hpp
#        ${GX_ENGINE_CORE_SRC_DIR}/graph/gx-cr-gr-graph.cpp
#        ${GX_ENGINE_CORE_SRC_DIR}/graph/gx-cr-gr-graph.hpp
#        ${GX_ENGINE_CORE_SRC_DIR}/graph/gx-cr-gr-link.cpp
#        ${GX_ENGINE_CORE_SRC_DIR}/graph/gx-cr-gr-link.hpp
#        ${GX_ENGINE_CORE_SRC_DIR}/graph/gx-cr-gr-node.cpp
#        ${GX_ENGINE_CORE_SRC_DIR}/graph/gx-cr-gr-node.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/job/gx-cr-job-end-caller.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/job/gx-cr-job-manager.cpp
        ${GX_ENGINE_CORE_SRC_DIR}/job/gx-cr-job-manager.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/macro/gx-cr-mcr-assert.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/macro/gx-cr-mcr-characterifier.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/macro/gx-cr-mcr-concatenate.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/macro/gx-cr-mcr-counter.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/macro/gx-cr-mcr-flagger.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/macro/gx-cr-mcr-getter-setter.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/macro/gx-cr-mcr-loop.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/macro/gx-cr-mcr-profiler.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/macro/gx-cr-mcr-stringifier.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/macro/gx-cr-mcr-unused.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/macro/gx-cr-mcr-zeroer.hpp
#        ${GX_ENGINE_CORE_SRC_DIR}/sync/gx-cr-sync-channel.hpp
#        ${GX_ENGINE_CORE_SRC_DIR}/sync/gx-cr-sync-channeled-workers.cpp
#        ${GX_ENGINE_CORE_SRC_DIR}/sync/gx-cr-sync-channeled-workers.hpp
#        ${GX_ENGINE_CORE_SRC_DIR}/sync/gx-cr-sync-influence-manager.cpp
#        ${GX_ENGINE_CORE_SRC_DIR}/sync/gx-cr-sync-influence-manager.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/sync/gx-cr-sync-kernel-workers.cpp
        ${GX_ENGINE_CORE_SRC_DIR}/sync/gx-cr-sync-kernel-workers.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/sync/gx-cr-sync-parallel-for.cpp
        ${GX_ENGINE_CORE_SRC_DIR}/sync/gx-cr-sync-parallel-for.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/sync/gx-cr-sync-semaphore.cpp
        ${GX_ENGINE_CORE_SRC_DIR}/sync/gx-cr-sync-semaphore.hpp
#        ${GX_ENGINE_CORE_SRC_DIR}/sync/gx-cr-sync-stop-point.cpp
#        ${GX_ENGINE_CORE_SRC_DIR}/sync/gx-cr-sync-stop-point.hpp
#        ${GX_ENGINE_CORE_SRC_DIR}/sync/gx-cr-sync-update-manager.cpp
#        ${GX_ENGINE_CORE_SRC_DIR}/sync/gx-cr-sync-update-manager.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/sync/gx-cr-sync-work-waiter.cpp
        ${GX_ENGINE_CORE_SRC_DIR}/sync/gx-cr-sync-work-waiter.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/gx-cr-application.cpp
        ${GX_ENGINE_CORE_SRC_DIR}/gx-cr-application.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/gx-cr-build-configuration.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/gx-cr-function-loader.cpp
        ${GX_ENGINE_CORE_SRC_DIR}/gx-cr-function-loader.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/gx-cr-language.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/gx-cr-observed.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/gx-cr-pool.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/gx-cr-project.cpp
        ${GX_ENGINE_CORE_SRC_DIR}/gx-cr-project.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/gx-cr-range.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/gx-cr-string.cpp
        ${GX_ENGINE_CORE_SRC_DIR}/gx-cr-string.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/gx-cr-types.hpp
        PARENT_SCOPE)

SET(GX_ENGINE_CORE_TEST_SRC
        ${GX_ENGINE_CORE_SRC_DIR}/allocator/gx-cr-alc-range-test.hpp
        ${GX_ENGINE_CORE_SRC_DIR}/ecs/gx-cr-ecs-world-test.hpp
        PARENT_SCOPE)