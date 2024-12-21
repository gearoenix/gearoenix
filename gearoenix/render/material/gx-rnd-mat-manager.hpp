#pragma once
#include "../../core/job/gx-cr-job-end-caller.hpp"
#include <map>
#include <string>

namespace gearoenix::render::engine {
struct Engine;
}

namespace gearoenix::render::material {
struct Material;
struct Pbr;
struct Unlit;
struct Sprite;
struct Manager {
    engine::Engine& e;

protected:
    std::mutex materials_lock;
    std::map<std::string, std::weak_ptr<Material>> materials;

    explicit Manager(engine::Engine&);
    virtual void construct_pbr(const std::string& name, core::job::EndCallerShared<Pbr>&& c) = 0;
    virtual void construct_unlit(const std::string& name, core::job::EndCallerShared<Unlit>&& c) = 0;
    virtual void construct_sprite(const std::string& name, core::job::EndCallerShared<Sprite>&& c) = 0;

public:
    virtual ~Manager();
    void get_pbr(const std::string& name, core::job::EndCallerShared<Pbr>&& c);
    void get_unlit(const std::string& name, core::job::EndCallerShared<Unlit>&& c);
    void get_sprite(const std::string& name, core::job::EndCallerShared<Sprite>&& c);
};
}