#ifndef GEAROENIX_CORE_SYNC_PARALLEL_FOR_HPP
#define GEAROENIX_CORE_SYNC_PARALLEL_FOR_HPP
#include "../gx-cr-build-configuration.hpp"

#ifndef GX_THREAD_NOT_SUPPORTED
#include <algorithm>
#include <execution>
#include <functional>
#endif

namespace gearoenix::core::sync {
struct ParallelFor final {
public:
    ParallelFor() = delete;
    ~ParallelFor() = delete;

    template <typename Iter, typename Fun>
    static void map(Iter begin, Iter end, Fun&& f)
    {
#ifdef GX_THREAD_NOT_SUPPORTED
        for (; begin != end; ++begin)
            f(*begin);
#else
        std::for_each(std::execution::par_unseq, std::move(begin), std::move(end), std::move(f));
#endif
    }
};
}
#endif
