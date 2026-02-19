#pragma once
#include "allocator/gx-cr-alc-shared-array.hpp"
#include "gx-cr-cast.hpp"
#include "gx-cr-object-type-indices.hpp"
#include "job/gx-cr-job-end-caller.hpp"
#include "macro/gx-cr-mcr-getter-setter.hpp"

#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/core/demangle.hpp>

#include <any>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace gearoenix::platform::stream {
struct Stream;
}

namespace gearoenix::core {
struct Object;
struct ObjectStreamer;

typedef boost::container::flat_set<object_type_index_t> ObjectTypeIndexSet;

template <typename T>
concept ObjectTypeHasRead = requires(std::shared_ptr<T>&& self, std::shared_ptr<platform::stream::Stream>&& stream, std::shared_ptr<ObjectStreamer>&& os, job::EndCaller<>&& end) {
    { T::read(std::move(self), std::move(stream), std::move(os), std::move(end)) } -> std::same_as<void>;
} && requires(object_id_t id, std::string&& name) { T { id, std::move(name) }; };

#define GEAROENIX_OBJECT_STRUCT_DEF        \
    friend struct gearoenix::core::Object; \
    template <typename T, std::int64_t S>  \
    friend struct gearoenix::core::allocator::SharedArray

struct Object {
    friend struct ObjectStreamer;

    constexpr static std::array<object_type_index_t, 0> all_parent_object_type_indices {};
    constexpr static std::array<object_type_index_t, 0> immediate_parent_object_type_indices {};

    template <typename T>
    using ObjectAllocator = std::shared_ptr<allocator::SharedArray<T, T::max_count>>;

    typedef std::function<void(object_id_t, std::string&&, std::shared_ptr<platform::stream::Stream>&&, std::shared_ptr<ObjectStreamer>&&, job::EndCallerShared<Object>&&)> ObjectStreamConstructor;

    struct ObjectTypeInfo final {
        GX_GET_CREF_PRV(std::string, name);
        GX_GET_CREF_PRV(std::any, allocator);
        GX_GET_CREF_PRV(ObjectTypeIndexSet, all_parents);
        GX_GET_CREF_PRV(ObjectTypeIndexSet, immediate_parents);
        GX_GET_VAL_PRV(bool, is_final, false);
        GX_GET_VAL_PRV(std::uint32_t, max_count, 1);
        GX_GET_CREF_PRV(ObjectStreamConstructor, construct_stream);

    public:
        ObjectTypeInfo() = default;

        ObjectTypeInfo(
            std::string&& name,
            std::any&& allocator,
            ObjectTypeIndexSet&& all_parents,
            ObjectTypeIndexSet&& immediate_parents,
            const bool is_final,
            const std::uint32_t max_count,
            ObjectStreamConstructor&& construct_stream)
            : name(std::move(name))
            , allocator(std::move(allocator))
            , all_parents(std::move(all_parents))
            , immediate_parents(std::move(immediate_parents))
            , is_final(is_final)
            , max_count(max_count)
            , construct_stream(std::move(construct_stream))
        {
        }
    };

    typedef std::vector<ObjectTypeInfo> ObjectTypeInfos;

    GX_GET_REFC_PRT(object_type_index_t, object_final_type_index);
    GX_GET_CVAL_PRT(object_id_t, object_id);
    GX_GET_REFC_PRT(std::string, object_name);
    GX_GET_CREF_PRT(std::weak_ptr<Object>, object_self);

    static ObjectTypeInfos object_type_infos;
    static boost::container::flat_map<std::string, object_type_index_t> object_type_name_to_index;
    static std::atomic<object_id_t> latest_id;
    static std::mutex all_objects_lock;
    static std::vector<std::weak_ptr<Object>> all_objects;

    static void register_type(object_type_index_t t, ObjectTypeInfo&&);

    template <std::uintptr_t N>
    [[nodiscard]] static ObjectTypeIndexSet create_object_type_index_set(const std::array<object_type_index_t, N>& indices)
    {
        ObjectTypeIndexSet set;
        for (const auto i : indices) {
            set.emplace(i);
        }
        return set;
    }

    Object(object_type_index_t final_type_index, std::string&& name);
    Object(object_type_index_t final_type_index, object_id_t id, std::string&& name);

    /// The serialisation function for all Object-inherited types.
    ///
    /// @note Implementor must call the parent and then in the end-callback write their own
    /// @note This function must only be called inside @code Object::write@endcode
    virtual void write(std::shared_ptr<platform::stream::Stream>&& stream, std::shared_ptr<ObjectStreamer>&& object_streamer, job::EndCaller<>&& end_caller);

private:
    /// This function should only be called in the ObjectStreamer.
    static void read(std::shared_ptr<platform::stream::Stream>&& stream, std::shared_ptr<ObjectStreamer>&& stream_context, job::EndCallerShared<Object>&& end_callback);

    /// This function should only be called in the ObjectStreamer.
    void write(std::shared_ptr<platform::stream::Stream>&& stream, std::shared_ptr<ObjectStreamer>&& object_streamer);

public:
    virtual ~Object();
    Object(Object&&) = delete;
    Object(const Object&) = delete;
    Object& operator=(Object&&) = delete;
    Object& operator=(const Object&) = delete;
    virtual void show_debug_gui();

    template <typename T>
    [[nodiscard]] static std::string create_type_name() { return boost::core::demangle(typeid(std::remove_cvref_t<T>).name()); }

    template <typename T, typename... Args>
    [[nodiscard]] static std::shared_ptr<T> construct(Args&&... args)
    {
        static_assert(std::is_base_of_v<Object, T>);
        static_assert(std::is_final_v<T>);
        auto ptr = std::any_cast<const ObjectAllocator<T>&>(get_type_info<T>().get_allocator())->make_shared(std::forward<Args>(args)...);
        register_object(ptr);
        return ptr;
    }

    static void register_object(const std::shared_ptr<Object>& object);

    template <typename T, typename DerivedType = T>
    static void register_type()
    {
        static_assert(std::is_base_of_v<Object, DerivedType>);
        static_assert(!std::is_same_v<T, Object>);
        static_assert(!std::is_same_v<DerivedType, Object>);
        static_assert(T::max_count > 0);
        register_type(
            T::object_type_index,
            ObjectTypeInfo(
                create_type_name<T>(),
                std::conditional_t<std::is_final_v<T>, allocator::SharedArray<T, T::max_count>, allocator::SharedArray<char, 0>>::construct(),
                create_object_type_index_set(T::all_parent_object_type_indices),
                create_object_type_index_set(T::immediate_parent_object_type_indices),
                std::is_final_v<T>,
                T::max_count,
                [](
                    [[maybe_unused]] const object_id_t id,
                    [[maybe_unused]] std::string&& name,
                    [[maybe_unused]] std::shared_ptr<platform::stream::Stream>&& stream,
                    [[maybe_unused]] std::shared_ptr<ObjectStreamer>&& object_streamer,
                    [[maybe_unused]] job::EndCallerShared<Object>&& end) {
                    if constexpr (ObjectTypeHasRead<T> && std::is_final_v<T>) {
                        auto self = Object::construct<T>(id, std::move(name));
                        end.set_return(self);
                        T::read(std::move(self), std::move(stream), std::move(object_streamer), job::EndCaller([e = std::move(end)] { }));
                    } else {
                        GX_LOG_F("Type [" << create_type_name<T>() << "] does not provide read constructor.");
                    }
                }));
    }

    [[nodiscard]] static const ObjectTypeInfo& get_type_info(object_type_index_t ti);
    [[nodiscard]] static bool is_registered(object_type_index_t ti);
    [[nodiscard]] static std::optional<object_type_index_t> get_index(const std::string& type_name);
    [[nodiscard]] static const std::string& get_object_type_name(object_type_index_t ti);
    [[nodiscard]] static const ObjectStreamConstructor& get_object_stream_constructor(object_type_index_t ti);
    [[nodiscard]] static bool check_object_type_registration_state(object_type_index_t ti);

    template <typename T>
    [[nodiscard]] static const ObjectTypeInfo& get_type_info() { return get_type_info(T::object_type_index); }

    [[nodiscard]] const ObjectTypeIndexSet& get_all_parent_types() const;
    [[nodiscard]] static std::shared_ptr<Object> find_object(object_id_t);

    template <typename T>
    [[nodiscard]] static std::shared_ptr<T> find_object_t(const object_id_t id) { return cast_shared<T>(find_object(id)); }

    [[nodiscard]] virtual bool is_castable_to(object_type_index_t ti) const;
};
}
