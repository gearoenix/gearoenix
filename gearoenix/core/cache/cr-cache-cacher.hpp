#ifndef GEAROENIX_CORE_CACHE_CACHER_HPP
#define GEAROENIX_CORE_CACHE_CACHER_HPP
#include "../../system/sys-log.hpp"
#include "../cr-static.hpp"
#include "../cr-types.hpp"
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <variant>

namespace gearoenix::core::cache {
template <class T, class Key = Id, class Compare = std::less<Key>>
class Cacher {
public:
    typedef std::map<Key, std::weak_ptr<T>, Compare> CacheMap;
    typedef std::map<std::string, Key> NameKeyMap;
    typedef std::map<Key, std::string> KeyNameMap;

    GX_GET_REF_PRV(CacheMap, cacheds)
    GX_GET_CREF_PRV(NameKeyMap, name_to_key)
    GX_GET_CREF_PRV(KeyNameMap, key_to_name)
public:
    template <class C>
    std::shared_ptr<C> get(const std::string& name, const std::function<std::shared_ptr<C>()>& new_fun) noexcept;
    template <class C>
    std::shared_ptr<C> get(const Key& id, const std::function<std::shared_ptr<C>()>& new_fun) noexcept;
    template <class C>
    std::shared_ptr<C> get(const std::string& name) const noexcept;
    template <class C>
    std::shared_ptr<C> get(const Key& id) const noexcept;
    void register_name(const std::string& name, const Key& k) noexcept;
    Key get_key(const std::string& name) const noexcept;
    std::optional<Key> try_get_key(const std::string& name) const noexcept;
};
}

template <class T, class Key, class Compare>
template <class C>
std::shared_ptr<C> gearoenix::core::cache::Cacher<T, Key, Compare>::get(const std::string& name, const std::function<std::shared_ptr<C>()>& new_fun) noexcept
{
    return get(get_key(name), new_fun);
}

template <class T, class Key, class Compare>
template <class C>
std::shared_ptr<C> gearoenix::core::cache::Cacher<T, Key, Compare>::get(const Key& id, const std::function<std::shared_ptr<C>()>& new_fun) noexcept
{
    auto search = cacheds.find(id);
    if (search == cacheds.end()) {
        auto new_item = new_fun();
        cacheds[id] = new_item;
        if (key_to_name.find(id) == key_to_name.end())
            register_name(new_item->get_name(), id);
        return new_item;
    }
    auto& found = search->second;
    if (auto cached = found.lock()) {
        return std::static_pointer_cast<C>(cached);
    } else {
        auto new_item = new_fun();
        found = new_item;
        return new_item;
    }
}

template <class T, class Key, class Compare>
template <class C>
std::shared_ptr<C> gearoenix::core::cache::Cacher<T, Key, Compare>::get(const std::string& name) const noexcept
{
    return get(get_key(name));
}

template <class T, class Key, class Compare>
template <class C>
std::shared_ptr<C> gearoenix::core::cache::Cacher<T, Key, Compare>::get(const Key& id) const noexcept
{
    const auto search = cacheds.find(id);
    if (search == cacheds.end()) {
        GXLOGF("Object with id: " << id << ", has not been cached.")
    }
    if (auto cached = search->second.lock()) {
        return std::static_pointer_cast<C>(cached);
    } else {
        GXLOGF("Object with id: " << id << ", cached but it has been expired.")
    }
}

template <class T, class Key, class Compare>
void gearoenix::core::cache::Cacher<T, Key, Compare>::register_name(const std::string& name, const Key& k) noexcept
{
#ifdef GX_DEBUG_MODE
    const auto search = name_to_key.find(name);
    if (search != name_to_key.end()) {
        if (!Compare()(search->second, k) && !Compare()(k, search->second)) {
            return;
        } else {
            GXLOGF("Already registered with name: " << name)
        }
    }
#endif
    name_to_key.emplace(name, k);
    key_to_name.emplace(k, name);
}

template <class T, class Key, class Compare>
Key gearoenix::core::cache::Cacher<T, Key, Compare>::get_key(const std::string& name) const noexcept
{
    const auto search = name_to_key.find(name);
    if (search == name_to_key.end())
        GXLOGF("Object with name: " << name << "not found.")
    return search->second;
}

template <class T, class Key, class Compare>
std::optional<Key>
gearoenix::core::cache::Cacher<T, Key, Compare>::try_get_key(const std::string& name) const noexcept
{
    const auto search = name_to_key.find(name);
    if (search == name_to_key.end())
        return std::nullopt;
    return search->second;
}

#endif
