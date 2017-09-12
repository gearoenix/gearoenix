#ifndef GEAROENIX_CORE_CACHE_FILE_HPP
#define GEAROENIX_CORE_CACHE_FILE_HPP
#include <vector>
#include "../../../system/sys-file.hpp"
#include "../cr-cache-cacher.hpp"
namespace gearoenix {
namespace core {
namespace cache {
namespace file {
class File
{
private:
    Cacher cacher;
    std::vector<Offset> offsets;
    std::shared_ptr<system::File> file;
public:
    File(std::shared_ptr<system::File> file);
    template<class T>
    std::shared_ptr<T> get(Id id, std::function<T()>);

};
} // namespace file
} // namespace cache
} // namespace core
} // namespace gearoenix

template<class T>
std::shared_ptr<T> gearoenix::core::cache::Cacher::get(Id id, std::function<T()> new_fun) {
    return cacher.get(id, [new_fun, this] {
        file->seek(offsets[id]);
        return new_fun();
    });
}

#endif // GEAROENIX_CORE_CACHE_FILE_HPP
