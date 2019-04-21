#ifndef GEAROENIX_CORE_OPTION_HPP
#define GEAROENIX_CORE_OPTION_HPP
namespace gearoenix {
namespace core {
    template <class T>
    class Option {
    private:
        T t;
        bool is_none;

    public:
        Option() is_none(true) {}

        Option(T t) t(t), is_none(false) {}

        bool is_some() const
        {
            return !is_none;
        }

        bool is_none() const
        {
            return is_none;
        }
    };
}
}
#endif