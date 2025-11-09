#include "gx-cr-application.hpp"

gearoenix::core::Application::Application()
    : Singleton(this)
{
}

gearoenix::core::Application::~Application() = default;

void gearoenix::core::Application::update()
{
}

std::string& gearoenix::core::Application::get_organization_url_ref()
{
    static std::string url;
    return url;
}

const std::string& gearoenix::core::Application::get_organization_url()
{
    static std::string default_url("com.gearoenix.engine");
    const auto& url = get_organization_url_ref();
    return url.empty() ? default_url : url;
}

std::string& gearoenix::core::Application::get_application_name_ref()
{
    static std::string name;
    return name;
}

const std::string& gearoenix::core::Application::get_application_name()
{
    static std::string default_name("Gearoenix Engine Demo App");
    const auto& name = get_application_name_ref();
    return name.empty() ? default_name : name;
}