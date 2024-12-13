#pragma once
#include "gx-cr-sync-thread.hpp"
#include <functional>
#include <thread>

namespace gearoenix::core::sync {
struct ParallelFor final {

    static void exec(const std::function<void(unsigned int, unsigned int)>& fun);

    ParallelFor() = delete;
    ~ParallelFor() = delete;

    template <typename Iter, typename Fun>
    static void exec(Iter first, Iter end, Fun&& f)
    {
        exec([&](const unsigned int kernels_count, const unsigned int kernel_index) {
            Iter iter = first;
            for (unsigned int i = 0; i < kernel_index; ++i, ++iter) {
                if (iter == end) {
                    return;
                }
            }
            while (iter != end) {
                f(*iter, kernel_index);
                for (unsigned int i = 0; i < kernels_count; ++i, ++iter) {
                    if (iter == end) {
                        return;
                    }
                }
            }
        });
    }

    template <typename Iter, typename Fun>
    static void execi(Iter first, Iter end, Fun&& f)
    {
        exec([&](const unsigned int kernels_count, const unsigned int kernel_index) {
            unsigned int index = kernel_index;
            Iter iter = first;
            for (unsigned int i = 0; i < kernel_index; ++i, ++iter) {
                if (iter == end) {
                    return;
                }
            }
            for (; iter != end; index += kernels_count) {
                f(*iter, index, kernel_index);
                for (unsigned int i = 0; i < kernels_count; ++i, ++iter) {
                    if (iter == end) {
                        return;
                    }
                }
            }
        });
    }

    template <typename Iter, typename Fun>
    static void seq_ranges_exec(Iter first, Iter end, Fun&& f)
    {
        const auto tc = threads_count();
        const auto count = static_cast<decltype(tc)>(end - first);
        const auto loop_count = tc - 1;
        const auto range_size = count / tc;
        std::vector<std::pair<Iter, Iter>> ranges;
        ranges.reserve(tc);
        auto first_index = decltype(tc) { 0 };
        for (auto thread_index = decltype(tc) { 0 }; thread_index < loop_count; ++thread_index) {
            auto end_index = first_index + range_size;
            ranges.emplace_back(first + first_index, first + end_index);
            first_index = end_index;
        }
        ranges.emplace_back(first + first_index, end);
        exec([&](const unsigned int, const unsigned int kernel_index) {
            auto& range = ranges[kernel_index];
            for (Iter iter = range.first; iter != range.second; ++iter) {
                f(*iter, kernel_index);
            }
        });
    }
};
}