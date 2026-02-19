#pragma once
#include "gx-cr-job-end-caller.hpp"

#include <map>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace gearoenix::core::job {
template <typename T, typename Key = std::string>
struct SingleEnd final {
private:
    std::mutex lock;
    std::unordered_map<Key, std::weak_ptr<T>> data;
    std::map<Key, std::vector<EndCallerShared<T>>> ends;

public:
    SingleEnd() = default;
    ~SingleEnd() = default;
    SingleEnd(SingleEnd&&) = delete;
    SingleEnd(const SingleEnd&) = delete;

    void set(const Key& k, const std::shared_ptr<T>& v)
    {
        std::vector<EndCallerShared<T>> vec;
        {
            const std::lock_guard _lg(lock);

            if (auto search = ends.find(k); ends.end() != search) {
                std::swap(vec, search->second);
                ends.erase(search);
            }

            data[k] = v;
        }

        for (const auto& e : vec) {
            e.set_return(std::shared_ptr(v));
        }
    }

    /// Returns true if the resource is already cached or another caller is already creating it.
    /// Returns false if no one is creating this resource yet; the caller must initiate creation
    /// and pass the result to `set` once ready.
    [[nodiscard]] bool get(const Key& k, EndCallerShared<T>&& end)
    {
        const std::lock_guard _lg(lock);
        if (auto d = data[k].lock(); d) {
            end.set_return(std::move(d));
            GX_ASSERT_D(!ends.contains(k));
            return true;
        }
        auto& vec = ends[k];
        const auto result = !vec.empty();
        vec.push_back(std::move(end));
        return result;
    }
};
}