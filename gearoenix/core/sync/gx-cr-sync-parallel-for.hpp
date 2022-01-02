#ifndef GEAROENIX_CORE_SYNC_PARALLEL_FOR_HPP
#define GEAROENIX_CORE_SYNC_PARALLEL_FOR_HPP
#include "../gx-cr-build-configuration.hpp"

#ifndef GX_THREAD_NOT_SUPPORTED
#include <functional>
#include <map>
#include <thread>

namespace gearoenix::core::sync {
struct KernelWorkers;
}

#endif

namespace gearoenix::core::sync {
struct ParallelFor final {
#ifndef GX_THREAD_NOT_SUPPORTED
public:
    typedef std::map<std::thread::id, std::pair<std::unique_ptr<KernelWorkers>, std::function<void(unsigned int, unsigned int)>>> Map;

private:
    static Map threads;

    static void exec(std::function<void(unsigned int, unsigned int)> fun) noexcept;
#endif
public:
    ParallelFor() = delete;
    ~ParallelFor() = delete;

    template <typename Iter, typename Fun>
    static void exec(Iter first, Iter end, Fun&& f)
    {
#ifdef GX_THREAD_NOT_SUPPORTED
        for (; first != end; ++first)
            f(*first, 0);
#else
        exec([f = std::move(f), first, end](const unsigned int kernels_count, const unsigned int kernel_index) noexcept {
            for (Iter iter = (first + kernel_index); iter != end; iter += kernels_count) {
                f(*iter, kernel_index);
            }
        });
#endif
    }

    template <typename Iter, typename Fun>
    static void execi(Iter first, Iter end, Fun&& f)
    {
#ifdef GX_THREAD_NOT_SUPPORTED
        for (unsigned int index = 0; first != end; ++first, ++index)
            f(*first, index, 0);
#else
        exec([f = std::move(f), first, end](const unsigned int kernels_count, const unsigned int kernel_index) noexcept {
            unsigned int index = kernel_index;
            for (Iter iter = (first + kernel_index); iter != end; iter += kernels_count, index += kernels_count) {
                f(*iter, index, kernel_index);
            }
        });
#endif
    }

    template <typename Iter, typename Fun>
    static void seq_ranges_exec(Iter first, Iter end, Fun&& f)
    {
#ifdef GX_THREAD_NOT_SUPPORTED
        for (unsigned int index = 0; first != end; ++first, ++index)
            f(*first, index, 0);
#else
        const auto threads_count = std::thread::hardware_concurrency();
        const auto count = static_cast<decltype(threads_count)>(end - first);
        const auto loop_count = threads_count - 1;
        const auto range_size = count / threads_count;
        std::vector<std::pair<Iter, Iter>> ranges;
        ranges.reserve(threads_count);
        auto first_index = decltype(threads_count) { 0 };
        for (auto thread_index = decltype(threads_count) { 0 }; thread_index < loop_count; ++thread_index) {
            auto end_index = first_index + range_size;
            ranges.emplace_back(first + first_index, first + end_index);
            first_index = end_index;
        }
        ranges.emplace_back(first + first_index, end);
        exec([f = std::move(f), ranges = std::move(ranges)](const unsigned int, const unsigned int kernel_index) noexcept {
            auto& range = ranges[kernel_index];
            for (Iter iter = range.first; iter != range.second; ++iter) {
                f(*iter, kernel_index);
            }
        });
#endif
    }
};
}
#endif
