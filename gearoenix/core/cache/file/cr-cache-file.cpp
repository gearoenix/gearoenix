#include "cr-cache-file.hpp"
#include "../../../system/file/sys-fl-file.hpp"

gearoenix::core::cache::file::File::File(system::file::File* file)
    : file(file)
{
}

void gearoenix::core::cache::file::File::read_offsets()
{
    file->read(offsets);
}
