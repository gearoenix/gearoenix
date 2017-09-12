#ifndef GEAROENIX_CORE_CACHE_FILE_SPARSE_HPP
#define GEAROENIX_CORE_CACHE_FILE_SPARSE_HPP
#include "../../../system/sys-log.hpp"
#include "../../cr-build-configuration.hpp"
#include "../cr-cache-cacher.hpp"
namespace gearoenix {
namespace system {
    class File;
}
namespace core {
    namespace cache {
        namespace file {
            class Sparse {
            private:
                Cacher cacher;
                std::map<Id, Offset> offsets;
                std::shared_ptr<system::File> file;

            public:
                Sparse(std::shared_ptr<system::File> file);
                void read_offsets();
                template <class T>
                std::shared_ptr<T> get(Id id, std::function<T()>);
            };
        } // namespace file
    } // namespace asset
} // namespace core
} // namespace gearoenix

template <class T>
std::shared_ptr<T> gearoenix::core::cache::file::Sparse::get(Id id, std::function<T()> new_fun)
{
    return cacher.get(id, [new_fun, this] {
#ifdef DEBUG_MODE
        auto search = offsets.find(id);
        if (search == offsets.end()) {
            LOGE("object with id: " << id << ", not found in table of offsets.");
        }
        file->seek(search->second);
#else
        file->seek(offsets[id]);
#endif
        return new_fun();
    });
}

#endif // GEAROENIX_CORE_CACHE_FILE_SPARSE_HPP
