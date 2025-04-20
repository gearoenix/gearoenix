SET(GX_ENGINE_PHYSICS_SRC_DIR ${GX_ENGINE_SRC_DIR}/physics)
SET(GX_ENGINE_PHYSICS_SRC
        ${GX_ENGINE_PHYSICS_SRC_DIR}/accelerator/gx-phs-acc-bvh.hpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/animation/gx-phs-anm-animation.cpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/animation/gx-phs-anm-animation.hpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/animation/gx-phs-anm-armature.cpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/animation/gx-phs-anm-armature.hpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/animation/gx-phs-anm-bone.cpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/animation/gx-phs-anm-bone.hpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/animation/gx-phs-anm-interpolation.hpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/animation/gx-phs-anm-keyframe.hpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/animation/gx-phs-anm-manager.cpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/animation/gx-phs-anm-manager.hpp
#        ${GX_ENGINE_PHYSICS_SRC_DIR}/body/gx-phs-bd-body.cpp
#        ${GX_ENGINE_PHYSICS_SRC_DIR}/body/gx-phs-bd-body.hpp
#        ${GX_ENGINE_PHYSICS_SRC_DIR}/body/gx-phs-bd-car.cpp
#        ${GX_ENGINE_PHYSICS_SRC_DIR}/body/gx-phs-bd-car.hpp
#        ${GX_ENGINE_PHYSICS_SRC_DIR}/body/gx-phs-bd-rigid.cpp
#        ${GX_ENGINE_PHYSICS_SRC_DIR}/body/gx-phs-bd-rigid.hpp
#        ${GX_ENGINE_PHYSICS_SRC_DIR}/body/gx-phs-bd-static.cpp
#        ${GX_ENGINE_PHYSICS_SRC_DIR}/body/gx-phs-bd-static.hpp
#        ${GX_ENGINE_PHYSICS_SRC_DIR}/body/gx-phs-bd-type.hpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/collider/gx-phs-cld-aabb.cpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/collider/gx-phs-cld-aabb.hpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/collider/gx-phs-cld-collider.cpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/collider/gx-phs-cld-collider.hpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/collider/gx-phs-cld-frustum.cpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/collider/gx-phs-cld-frustum.hpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/collider/gx-phs-cld-mesh.cpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/collider/gx-phs-cld-mesh.hpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/collider/gx-phs-cld-sphere.hpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/constraint/gx-phs-cns-constraint.cpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/constraint/gx-phs-cns-constraint.hpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/constraint/gx-phs-cns-jet-controller.cpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/constraint/gx-phs-cns-jet-controller.hpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/constraint/gx-phs-cns-manager.cpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/constraint/gx-phs-cns-manager.hpp
#        ${GX_ENGINE_PHYSICS_SRC_DIR}/constraint/gx-phs-cns-window-placer.cpp
#        ${GX_ENGINE_PHYSICS_SRC_DIR}/constraint/gx-phs-cns-window-placer.hpp
#        ${GX_ENGINE_PHYSICS_SRC_DIR}/constraint/gx-phs-cns-window-scaler.cpp
#        ${GX_ENGINE_PHYSICS_SRC_DIR}/constraint/gx-phs-cns-window-scaler.hpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/gx-phs-constants.hpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/gx-phs-engine.cpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/gx-phs-engine.hpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/gx-phs-transformation.cpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/gx-phs-transformation.hpp
        PARENT_SCOPE)

SET(GX_ENGINE_PHYSICS_TEST_SRC
        ${GX_ENGINE_PHYSICS_SRC_DIR}/accelerator/gx-phs-acc-bvh-test.hpp
        ${GX_ENGINE_PHYSICS_SRC_DIR}/gx-phs-transformation-test.hpp
        PARENT_SCOPE)