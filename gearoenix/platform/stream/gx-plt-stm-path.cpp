#include "gx-plt-stm-path.hpp"
#include "../gx-plt-log.hpp"

const std::string& gearoenix::platform::stream::Path::get_raw_data() const
{
    switch (data.index()) {
    case 0:
        return std::get<0>(data).data;
    case 1:
        return std::get<1>(data).data;
    default:
        GX_UNEXPECTED;
    }
}
