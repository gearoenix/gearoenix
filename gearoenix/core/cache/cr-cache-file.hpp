#ifndef GEAROENIX_CORE_CACHE_FILE_HPP
#define GEAROENIX_CORE_CACHE_FILE_HPP
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"
#include "../cr-build-configuration.hpp"
#include "cr-cache-cacher.hpp"

namespace gearoenix::core::cache {
        template <class T>
        class File {
        private:
            Cacher<T> cacher;
            std::map<Id, Offset> offsets;
            system::stream::Stream*const file;

        public:
            explicit File(system::stream::Stream* file) noexcept;
            ~File() noexcept;
            template <class C>
            std::shared_ptr<C> get(Id id, std::function<std::shared_ptr<C>()> new_fun) noexcept;
            template <class C>
            std::shared_ptr<C> get(Id id) const noexcept;
            const system::stream::Stream* get_file() const noexcept;
            system::stream::Stream* get_file() noexcept;
            const Cacher<T>& get_cacher() const noexcept;
            Cacher<T>& get_cacher() noexcept;
        };
    }

template <class T>
gearoenix::core::cache::File<T>::File(system::stream::Stream*const file) noexcept
    : file(file)
{
    if (file != nullptr) {
        const auto c = file->read<Count>();
        GXLOGD("Number of entries is " << c)
        for (Count i = 0; i < c; ++i) {
            const Id id = file->read<Id>();
            const auto off = file->read<Offset>();
            offsets[id] = off;
        }
    }
}

template<class T>
gearoenix::core::cache::File<T>::~File() noexcept {
    delete file;
}

template <class T>
template <class C>
std::shared_ptr<C> gearoenix::core::cache::File<T>::get(const Id id, std::function<std::shared_ptr<C>()> new_fun) noexcept
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
        return new_fun();
    };
    return cacher.get(id, fn_new);
}

template <class T>
template <class C>
inline std::shared_ptr<C> gearoenix::core::cache::File<T>::get(const Id id) const noexcept
{
    return cacher.template get<C>(id);
}

template <class T>
inline const gearoenix::system::stream::Stream* gearoenix::core::cache::File<T>::get_file() const noexcept
{
    return file;
}

template <class T>
inline gearoenix::system::stream::Stream* gearoenix::core::cache::File<T>::get_file() noexcept
{
    return file;
}

template <class T>
inline const gearoenix::core::cache::Cacher<T>& gearoenix::core::cache::File<T>::get_cacher() const noexcept
{
    return cacher;
}

template <class T>
inline gearoenix::core::cache::Cacher<T>& gearoenix::core::cache::File<T>::get_cacher() noexcept
{
    return cacher;
}

#endif
