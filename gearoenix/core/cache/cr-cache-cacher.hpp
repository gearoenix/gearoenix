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
			template <class  T, class Key = Id, class Compare = std::less<Key>>
			class Cacher {
			private:
				std::map<Key, std::weak_ptr<T>, Compare> cacheds;

			public:
				template <class C> std::shared_ptr<C> get(const Key id, std::function<std::shared_ptr<C>()> new_fun);
				template <class C> std::shared_ptr<C> get(const Key id) const;
			};
		}
	}
}

template <class  T, class Key, class Compare>
template <class  C>
std::shared_ptr<C> gearoenix::core::cache::Cacher<T, Key, Compare>::get(const Key id, std::function<std::shared_ptr<C>()> new_fun)
{
	auto search = cacheds.find(id);
	if (search == cacheds.end()) {
		std::shared_ptr<C> new_item = new_fun();
		cacheds[id] = new_item;
		return new_item;
	}
	auto& found = search->second;
	if (auto cached = found.lock()) {
		return std::static_pointer_cast<C>(cached);
	}
	else {
		std::shared_ptr<C> new_item = new_fun();
		found = new_item;
		return new_item;
	}
}

template <class  T, class Key, class Compare>
template <class  C>
std::shared_ptr<C> gearoenix::core::cache::Cacher<T, Key, Compare>::get(const Key id) const
{
	auto search = cacheds.find(id);
	if (search == cacheds.end()) {
		GXLOGF("Object with id: " << id << ", has not been cached.");
		return nullptr;
	}
	auto& found = search->second;
	if (auto cached = found.lock()) {
		return std::static_pointer_cast<C>(cached);
	}
	else {
		GXLOGF("Object with id: " << id << ", cached but it has been expired.");
		return nullptr;
	}
}

#endif
