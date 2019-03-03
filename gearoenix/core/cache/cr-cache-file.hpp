#ifndef GEAROENIX_CORE_CACHE_FILE_HPP
#define GEAROENIX_CORE_CACHE_FILE_HPP
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-log.hpp"
#include "../cr-build-configuration.hpp"
#include "cr-cache-cacher.hpp"
namespace gearoenix {
namespace core {
    namespace cache {
            template <class T>
            class File {
            private:
                Cacher<T> cacher;
                std::map<Id, Offset> offsets;
                std::shared_ptr<system::stream::Stream> file;

            public:
                File(const std::string &) {
                    GXUNIMPLEMENTED;
                }

                void read_offsets() {
                    Count c;
                    file->read(c);
                    for (Count i = 0; i < c; ++i) {
                        Id id;
                        Offset o;
                        file->read(id);
                        file->read(o);
                        offsets[id] = o;
                    }
                }

                std::shared_ptr<T> get(Id id, std::function<std::shared_ptr<T>()> new_fun){
                    std::function<std::shared_ptr<T>()> fn_new = [new_fun, this, id] {
                #ifdef GX_DEBUG_MODE
                        auto search = offsets.find(id);
                        if (search == offsets.end()) {
                            GXLOGF("object with id: " << id << ", not found in table of offsets.");
                        }
                        file->seek(static_cast<unsigned int>(search->second));
                #else
                        file->seek(offsets[id]);
                #endif
                        return new_fun();
                    };
                    return cacher.get(id, fn_new);
                }

                std::shared_ptr<T> get(Id id) const {
                    return cacher.get(id);
                }
            };
    } // namespace cache
} // namespace core
} // namespace gearoenix
#endif // GEAROENIX_CORE_CACHE_FILE_HPP
