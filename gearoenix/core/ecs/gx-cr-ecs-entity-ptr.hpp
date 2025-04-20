#pragma once
#include <memory>

namespace gearoenix::core::ecs {
struct Entity;

struct EntityPtr final {
    friend struct Entity;

private:
    std::shared_ptr<Entity> ptr;

public:
    explicit EntityPtr(std::shared_ptr<Entity>&& ptr = nullptr)
        : ptr(std::move(ptr))
    {
    }
    ~EntityPtr();

    EntityPtr(const EntityPtr&);
    EntityPtr(EntityPtr&&) noexcept;
    EntityPtr& operator=(const EntityPtr&);
    EntityPtr& operator=(EntityPtr&&) noexcept;

    [[nodiscard]] bool operator==(const EntityPtr& other) const { return ptr == other.ptr; }
    [[nodiscard]] bool operator==(const Entity* const other) const { return ptr.get() == other; }

    [[nodiscard]] const Entity* get() const { return ptr.get(); }
    [[nodiscard]] Entity* get() { return ptr.get(); }
    const Entity* operator->() const { return ptr.get(); }
    Entity* operator->() { return ptr.get(); }

    void clear();
};
}