#pragma once
#include "../../core/gx-cr-singleton.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include <map>
#include <string>

namespace gearoenix::render::material {
struct Material;
struct Pbr;
struct Unlit;
struct Sprite;
struct Manager : core::Singleton<Manager> {
protected:
    std::mutex materials_lock;
    std::map<std::string, std::weak_ptr<Material>> materials;

    Manager();
    virtual void construct_pbr(std::string&& name, core::job::EndCallerShared<Pbr>&& c) = 0;
    virtual void construct_unlit(std::string&& name, core::job::EndCallerShared<Unlit>&& c) = 0;
    virtual void construct_sprite(std::string&& name, core::job::EndCallerShared<Sprite>&& c) = 0;

public:
    ~Manager() override;
    void get_pbr(std::string&& name, core::job::EndCallerShared<Pbr>&& c);
    void get_unlit(std::string&& name, core::job::EndCallerShared<Unlit>&& c);
    void get_sprite(std::string&& name, core::job::EndCallerShared<Sprite>&& c);
};
}