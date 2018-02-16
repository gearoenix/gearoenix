#include "cr-cache-file-sparse.hpp"
#include "../../../system/stream/sys-stm-stream.hpp"

gearoenix::core::cache::file::Sparse::Sparse(system::stream::Stream* file)
    : file(file)
{
}

void gearoenix::core::cache::file::Sparse::read_offsets()
{
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
