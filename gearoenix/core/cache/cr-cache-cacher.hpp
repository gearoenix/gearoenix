#ifndef GEAROENIX_CORE_CACHE_CACHER_HPP
#define GEAROENIX_CORE_CACHE_CACHER_HPP
#include "../../system/sys-log.hpp"
#include "../cr-types.hpp"
#include "cr-cache-cached.hpp"
#include <functional>
#include <map>
#include <memory>
namespace gearoenix {
namespace core {
    namespace cache {
        class Cacher {
        private:
            std::map<Id, std::weak_ptr<Cached>> cacheds;

        public:
            template <class T>
            std::shared_ptr<T> get(Id id, std::function<std::shared_ptr<T>()>);
            template <class T>
            std::shared_ptr<T> get(Id id) const;
        };
    } // namespace asset
} // namespace core
} // namespace gearoenix

template <class T>
std::shared_ptr<T> gearoenix::core::cache::Cacher::get(Id id, std::function<std::shared_ptr<T>()> new_fun)
{
    auto search = cacheds.find(id);
    if (search == cacheds.end()) {
        std::shared_ptr<T> new_item = new_fun();
        cacheds[id] = new_item;
        return new_item;
    }
    auto& found = search->second;
    if (auto cached = found.lock()) {
        return std::static_pointer_cast<T>(cached);
    } else {
        std::shared_ptr<T> new_item = new_fun();
        found = new_item;
        return new_item;
    }
}

template <class T>
std::shared_ptr<T> gearoenix::core::cache::Cacher::get(Id id) const
{
    auto search = cacheds.find(id);
    if (search == cacheds.end()) {
        GXLOGF("Object with id: " << id << ", has not been cached.");
        return nullptr;
    }
    auto& found = search->second;
    if (auto cached = found.lock()) {
        return std::static_pointer_cast<T>(cached);
    } else {
        GXLOGF("Object with id: " << id << ", cached but it has been expired.");
        return nullptr;
    }
}

#endif // GEAROENIX_CORE_CACHE_CACHER_HPP
