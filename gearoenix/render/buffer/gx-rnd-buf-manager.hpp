#pragma once
#include "../../core/gx-cr-singleton.hpp"

#include <cstddef>
#include <memory>

namespace gearoenix::render::buffer {
struct Uniform;

struct Manager : core::Singleton<Manager> {
protected:
    Manager();

public:
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;
    ~Manager() override;
};
}