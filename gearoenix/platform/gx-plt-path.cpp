#include "gx-plt-path.hpp"
#include "gx-plt-application.hpp"

gearoenix::platform::AssetPath::AssetPath(const Application& app, const std::string& p) noexcept
    : Path(app.get_base().get_arguments().get_process_directory() + "assets/" + p) // TODO: In some platforms like android and ios it may not work
{
}
