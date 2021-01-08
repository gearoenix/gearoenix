#ifndef GEAROENIX_CORE_CACHE_FILE_HPP
#define GEAROENIX_CORE_CACHE_FILE_HPP
#include "../../platform/gx-plt-log.hpp"
#include "../../platform/stream/gx-plt-stm-stream.hpp"
#include "../gx-cr-build-configuration.hpp"
#include "../gx-cr-static.hpp"
#include "gx-cr-cache-cacher.hpp"

namespace gearoenix::core::cache {
template <typename T>
struct File {
public:
    typedef std::map<Id, Offset> IdOffsetMap;
    GX_GET_REF_PRV(Cacher<T>, cacher)
    GX_GET_UCPTR_PRV(platform::stream::Stream, file)
    GX_GET_CREF_PRV(IdOffsetMap, offsets)

public:
    explicit File(std::unique_ptr<platform::stream::Stream> file) noexcept;
    template <typename C>
    std::shared_ptr<C> get(Id id, std::function<std::shared_ptr<C>(std::string)> new_fun) noexcept;
    template <typename C>
    std::shared_ptr<C> get(Id id) const noexcept;
};
}

template <typename T>
gearoenix::core::cache::File<T>::File(std::unique_ptr<platform::stream::Stream> f) noexcept
    : file(std::move(f))
{
    if (file != nullptr) {
        const auto c = file->read<Count>();
        GXLOGD("Number of entries is " << c)
        for (Count i = 0; i < c; ++i) {
            const Id id = file->read<Id>();
            const auto off = file->read<Offset>();
            const auto name = file->read_string();
            offsets[id] = off;
            cacher.register_name(name, id);
        }
    }
}

template <typename T>
template <typename C>
std::shared_ptr<C> gearoenix::core::cache::File<T>::get(const Id id, std::function<std::shared_ptr<C>(std::string)> new_fun) noexcept
{
    std::function<std::shared_ptr<C>()> fn_new = [new_fun, this, id] {
#ifdef GX_DEBUG_MODE
        auto search = offsets.find(id);
        if (search == offsets.end()) {
            GXLOGF("object with id: " << id << ", not found in table of offsets.")
        }
        file->seek(static_cast<unsigned int>(search->second));
#else
        file->seek(offsets[id]);
#endif
        return new_fun(cacher.get_key_to_name().find(id)->second);
    };
    return cacher.get(id, fn_new);
}

template <typename T>
template <typename C>
inline std::shared_ptr<C> gearoenix::core::cache::File<T>::get(const Id id) const noexcept
{
    return cacher.template get<C>(id);
}

#endif
