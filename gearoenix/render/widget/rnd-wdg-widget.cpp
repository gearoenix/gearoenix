#include "rnd-wdg-widget.hpp"
#include "../../system/sys-file.hpp"
#include "rnd-wdg-button.hpp"

gearoenix::render::widget::Widget::Widget(system::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c)
    : model::Model(ModelType::WIDGET, f, e, c)
{
}

gearoenix::render::widget::Widget* gearoenix::render::widget::Widget::read(system::File* f, Engine* e, core::EndCaller<core::EndCallerIgnore> c)
{
    core::Id t;
    f->read(t);
    switch (t) {
    case BUTTON:
        return new Button(f, e, c);
    default:
        UNEXPECTED;
    }
    return nullptr;
}

gearoenix::render::widget::Widget::~Widget()
{
}
