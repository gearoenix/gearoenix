#ifndef GEAROENIX_CORE_CACHE_CACHER_HPP
#define GEAROENIX_CORE_CACHE_CACHER_HPP
#include "../../system/sys-log.hpp"
#include "../cr-types.hpp"
#include <functional>
#include <map>
#include <memory>
namespace gearoenix {
namespace core {
    namespace cache {
        class Cached;
        class Cacher {
        private:
            std::map<Id, std::weak_ptr<Cached>> cacheds;

        public:
            template <class T>
            std::shared_ptr<T> get(Id id, std::function<T()>);
            template <class T>
            std::shared_ptr<T> get(Id id) const;
        };
    } // namespace asset
} // namespace core
} // namespace gearoenix

template <class T>
std::shared_ptr<T> gearoenix::core::cache::Cacher::get(Id id, std::function<T()> new_fun)
{
    auto search = cacheds.find(id);
    if (search == cached.end()) {
        std::shared_ptr<T> cached = new_fun();
        cacheds[id] = cached;
        return cached;
    }
    auto& found = search->second;
    if (auto cached = found.lock()) {
        return std::static_pointer_cast<T>(cached);
    } else {
        std::shared_ptr<T> cached = new_fun();
        found = cached;
        return cached;
    }
}

template <class T>
std::shared_ptr<T> gearoenix::core::cache::Cacher::get(Id id) const
{
    auto search = cacheds.find(id);
    if (search == cached.end()) {
        LOGF("Object with id: " << id << ", has not been cached.");
        return nullptr;
    }
    auto& found = search->second;
    if (auto cached = found.lock()) {
        return std::static_pointer_cast<T>(cached);
    } else {
        LOGF("Object with id: " << id << ", cached but it has been expired.");
        return nullptr;
    }
}

#endif // GEAROENIX_CORE_CACHE_CACHER_HPP
