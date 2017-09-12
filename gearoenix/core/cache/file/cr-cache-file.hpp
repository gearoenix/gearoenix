#ifndef GEAROENIX_CORE_CACHE_FILE_HPP
#define GEAROENIX_CORE_CACHE_FILE_HPP
#include "../../../system/sys-log.hpp"
#include "../../cr-build-configuration.hpp"
#include "../cr-cache-cacher.hpp"
#include <vector>
namespace gearoenix {
namespace system {
    class File;
}
namespace core {
    namespace cache {
        namespace file {
            class File {
            private:
                Cacher cacher;
                std::vector<Offset> offsets;
                std::shared_ptr<system::File> file;

            public:
                File(std::shared_ptr<system::File> file);
                void read_offsets();
                template <class T>
                std::shared_ptr<T> get(Id id, std::function<T()>);
            };
        } // namespace file
    } // namespace cache
} // namespace core
} // namespace gearoenix

template <class T>
std::shared_ptr<T> gearoenix::core::cache::file::File::get(Id id, std::function<T()> new_fun)
{
    return cacher.get(id, [new_fun, this] {
#ifdef DEBUG_MODE
        if (id >= offsets.size()) {
            LOGE("object with id: " << id << ", not found in table of offsets.");
        }
#endif
        file->seek(offsets[id]);
        return new_fun();
    });
}

#endif // GEAROENIX_CORE_CACHE_FILE_HPP
