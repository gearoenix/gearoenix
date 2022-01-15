SET(GX_ENGINE_MATH_SRC_DIR ${GX_ENGINE_SRC_DIR}/math)
SET(GX_ENGINE_MATH_SRC
        ${GX_ENGINE_MATH_SRC_DIR}/gx-math-aabb.hpp
        ${GX_ENGINE_MATH_SRC_DIR}/gx-math-bezier-curve.hpp
        ${GX_ENGINE_MATH_SRC_DIR}/gx-math-frustum.hpp
        ${GX_ENGINE_MATH_SRC_DIR}/gx-math-intersection-status.hpp
        ${GX_ENGINE_MATH_SRC_DIR}/gx-math-matrix-2d.hpp
        ${GX_ENGINE_MATH_SRC_DIR}/gx-math-matrix-4d.hpp
        ${GX_ENGINE_MATH_SRC_DIR}/gx-math-numeric.hpp
        ${GX_ENGINE_MATH_SRC_DIR}/gx-math-plane.hpp
        ${GX_ENGINE_MATH_SRC_DIR}/gx-math-plotter.hpp
        ${GX_ENGINE_MATH_SRC_DIR}/gx-math-quaternion.hpp
        ${GX_ENGINE_MATH_SRC_DIR}/gx-math-ray.hpp
        ${GX_ENGINE_MATH_SRC_DIR}/gx-math-sphere.hpp
        ${GX_ENGINE_MATH_SRC_DIR}/gx-math-triangle.hpp
        ${GX_ENGINE_MATH_SRC_DIR}/gx-math-vector-2d.hpp
        ${GX_ENGINE_MATH_SRC_DIR}/gx-math-vector-3d.hpp
        ${GX_ENGINE_MATH_SRC_DIR}/gx-math-vector-4d.hpp
        PARENT_SCOPE)

SET(GX_ENGINE_MATH_TEST_SRC
        ${GX_ENGINE_MATH_SRC_DIR}/gx-math-matrix-4d-test.hpp
        PARENT_SCOPE)