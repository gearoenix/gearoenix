SET(GX_ENGINE_BGFX_SRC_DIR ${GX_ENGINE_SRC_DIR}/bgfx)
SET(GX_ENGINE_BGFX_SRC
        ${GX_ENGINE_BGFX_SRC_DIR}/gx-bgfx-engine.cpp
        ${GX_ENGINE_BGFX_SRC_DIR}/gx-bgfx-engine.hpp
        PARENT_SCOPE)

#SET(GX_ENGINE_BGFX_TEST_SRC
#        ${GX_ENGINE_CORE_SRC_DIR}/ecs/gx-cr-ecs-world-test.hpp
#        PARENT_SCOPE)