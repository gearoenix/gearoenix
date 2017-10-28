#ifndef GEAROENIX_CORE_CACHE_FILE_HPP
#define GEAROENIX_CORE_CACHE_FILE_HPP
#include "../../../system/sys-file.hpp"
#include "../../../system/sys-log.hpp"
#include "../../cr-build-configuration.hpp"
#include "../cr-cache-cacher.hpp"
#include <vector>
namespace gearoenix {
namespace core {
    namespace cache {
        namespace file {
            class File {
            private:
                Cacher cacher;
                std::vector<Offset> offsets;
                system::File* file;

            public:
                File(system::File* file);
                void read_offsets();
                template <class T>
                std::shared_ptr<T> get(Id id, std::function<std::shared_ptr<T>()>);
                template <class T>
                std::shared_ptr<T> get(Id id) const;
            };
        } // namespace file
    } // namespace cache
} // namespace core
} // namespace gearoenix

template <class T>
std::shared_ptr<T> gearoenix::core::cache::file::File::get(Id id, std::function<std::shared_ptr<T>()> new_fun)
{
    std::function<std::shared_ptr<T>()> fn_new = [new_fun, this, id] {
#ifdef DEBUG_MODE
        if (id >= offsets.size()) {
            LOGF("object with id: " << id << ", not found in table of offsets.");
        }
#endif
        file->seek((unsigned int) offsets[(size_t)id]);
        return new_fun();
    };
    return cacher.get<T>(id, fn_new);
}

template <class T>
std::shared_ptr<T> gearoenix::core::cache::file::File::get(Id id) const
{
    return cacher.get<T>(id);
}

#endif // GEAROENIX_CORE_CACHE_FILE_HPP
