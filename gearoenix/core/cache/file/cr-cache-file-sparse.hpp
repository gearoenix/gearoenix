#ifndef GEAROENIX_CORE_CACHE_FILE_SPARSE_HPP
#define GEAROENIX_CORE_CACHE_FILE_SPARSE_HPP
#include "../../../system/file/sys-fl-file.hpp"
#include "../../../system/sys-log.hpp"
#include "../../cr-build-configuration.hpp"
#include "../cr-cache-cacher.hpp"
namespace gearoenix {
namespace core {
    namespace cache {
        namespace file {
            class Sparse {
            private:
                Cacher cacher;
                std::map<Id, Offset> offsets;
                system::file::File* file;

            public:
                Sparse(system::file::File* file);
                void read_offsets();
                template <class T>
                std::shared_ptr<T> get(Id id, std::function<std::shared_ptr<T>()>);
                template <class T>
                std::shared_ptr<T> get(Id id) const;
            };
        } // namespace file
    } // namespace asset
} // namespace core
} // namespace gearoenix

template <class T>
std::shared_ptr<T> gearoenix::core::cache::file::Sparse::get(Id id, std::function<std::shared_ptr<T>()> new_fun)
{
    std::function<std::shared_ptr<T>()> fn_new = [new_fun, this, id] {
#ifdef DEBUG_MODE
        auto search = offsets.find(id);
        if (search == offsets.end()) {
            GXLOGF("object with id: " << id << ", not found in table of offsets.");
        }
        file->seek((unsigned int)search->second);
#else
        file->seek(offsets[id]);
#endif
        return new_fun();
    };
    return cacher.get<T>(id, fn_new);
}

template <class T>
std::shared_ptr<T> gearoenix::core::cache::file::Sparse::get(Id id) const
{
    return cacher.get<T>(id);
}

#endif // GEAROENIX_CORE_CACHE_FILE_SPARSE_HPP
