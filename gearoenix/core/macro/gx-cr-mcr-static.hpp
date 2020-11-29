#ifndef GEAROENIX_CORE_MACROS_STATIC_HPP
#define GEAROENIX_CORE_MACROS_STATIC_HPP
#include <cstring>
#include <memory>

#define GX_MAX(x, y) (((x) > (y)) ? (x) : (y))
#define GX_MIN(x, y) (((x) < (y)) ? (x) : (y))

#define GX_CEIL_2(x) (((x) >> 1) + ((x)&1))
#define GX_CEIL_4(x) (((x) >> 2) + ((((x) >> 1) & 1) | ((x)&1)))
#define GX_CEIL_8(x) (((x) >> 3) + ((((x) >> 2) & 1) | (((x) >> 1) & 1) | ((x)&1)))

#define GX_START_MULTITHREADED_TASKS \
    unsigned int task_number = 0;

#define GX_DO_MULTITHREADED_TASK(expr)                   \
    {                                                    \
        if (task_number == kernel_index) {               \
            expr;                                        \
        }                                                \
        task_number = (task_number + 1) % kernels_count; \
    }

#endif
