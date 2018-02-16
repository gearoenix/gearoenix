#include "cr-cache-file.hpp"
#include "../../../system/stream/sys-stm-stream.hpp"

gearoenix::core::cache::file::File::File(system::stream::Stream* stream)
    : file(stream)
{
}

void gearoenix::core::cache::file::File::read_offsets()
{
    file->read(offsets);
}
