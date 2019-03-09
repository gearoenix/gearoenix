#ifndef GEAROENIX_CORE_CACHE_CACHER_HPP
#define GEAROENIX_CORE_CACHE_CACHER_HPP
#include "../../system/sys-log.hpp"
#include "../cr-types.hpp"
#include <functional>
#include <map>
#include <memory>
#include <type_traits>
namespace gearoenix {
namespace core {
    namespace cache {
        template <class Base>
        class Cacher {
        private:
            std::map<Id, std::weak_ptr<Base>> cacheds;

        public:
            template <class Derived>
            class std::enable_if<std::is_base_of<Base, Derived>::value, std::shared_ptr<Derived>>::type
            get(const Id id, std::function<std::shared_ptr<Derived>()> new_fun)
            {
                auto search = cacheds.find(id);
                if (search == cacheds.end()) {
                    std::shared_ptr<Derived> new_item = new_fun();
                    cacheds[id] = new_item;
                    return new_item;
                }
                auto& found = search->second;
                if (auto cached = found.lock()) {
                    return std::static_pointer_cast<Derived>(cached);
                } else {
                    std::shared_ptr<Derived> new_item = new_fun();
                    found = new_item;
                    return new_item;
                }
            }

            template <class Derived>
            class std::enable_if<std::is_base_of<Base, Derived>::value, std::shared_ptr<Derived>>::type
            get(const Id id) const
            {
                auto search = cacheds.find(id);
                if (search == cacheds.end()) {
                    GXLOGF("Object with id: " << id << ", has not been cached.");
                    return nullptr;
                }
                auto& found = search->second;
                if (auto cached = found.lock()) {
                    return std::static_pointer_cast<Derived>(cached);
                } else {
                    GXLOGF("Object with id: " << id << ", cached but it has been expired.");
                    return nullptr;
                }
            }
        };
    } // namespace asset
} // namespace core
} // namespace gearoenix
#endif // GEAROENIX_CORE_CACHE_CACHER_HPP
