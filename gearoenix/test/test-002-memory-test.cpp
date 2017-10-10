#include "test-002-memory-test.hpp"
#ifdef TEST002

#include "../core/list/cr-list-node.hpp"
#include "../core/list/cr-list.hpp"

#include <iostream>

#define MIN_WID 10.0

MemoryVisualizer::MemoryVisualizer(gearoenix::core::gc::Gc* mmgr)
    : mmgr(mmgr)
{
    set_size_request(400, 100);

    ec[0].set_rgba_u(0xffff, 0xffff, 0);
    ec[1].set_rgba_u(0, 0xffff, 0xffff);

    fc[0].set_rgba_u(0xffff, 0, 0);
    fc[1].set_rgba_u(0, 0xffff, 0);

    set_vexpand(true);
    set_hexpand(true);

    grid.set_vexpand(true);
    grid.set_hexpand(true);
    add(grid);
}

MemoryVisualizer::~MemoryVisualizer()
{
    cleanup();
}

void MemoryVisualizer::remove(unsigned int oid)
{
    delete idobj[oid].o;
    for (auto& o : idobj)
        delete o.l;
    idobj.clear();
    visualize();
}

void MemoryVisualizer::visualize()
{
    double min_size = get_width();
    grid.remove_row(0);
    for (LObj& lo : idobj) {
        delete lo.l;
    }
    idobj.clear();
    double total_size = (double)mmgr->get_size();
    unsigned int last_end = 0;
    unsigned int smallest_rng = mmgr->get_size() + 1;
    unsigned int grid_column = 0;
    std::vector<unsigned int> sizes;
    gearoenix::core::list::List<gearoenix::core::gc::Object*>* objs = mmgr->get_objects();
    for (
        gearoenix::core::list::Node<gearoenix::core::gc::Object*>* n = objs->get_front();
        n != nullptr;
        n = n->get_next()) {
        gearoenix::core::gc::Object* o = n->get_value();
        const unsigned int offset = o->get_offset();
        const unsigned int osize = o->get_size();
        if (offset != last_end) {
            unsigned int rng = offset - last_end;
            if (smallest_rng > rng) {
                smallest_rng = rng;
            }
            Gtk::Label* l = new Gtk::Label;
            grid.attach(*l, grid_column++, 0, 1, 1);
            LObj lo = { l, nullptr };
            idobj.push_back(lo);
            sizes.push_back(rng);
        }
        if (osize != 0) {
            if (smallest_rng > osize) {
                smallest_rng = osize;
            }
            Gtk::Label* l = new Gtk::Label;
            grid.attach(*l, grid_column++, 0, 1, 1);
            LObj lo = { l, o };
            idobj.push_back(lo);
            sizes.push_back(osize);
        }
        last_end = offset + osize;
    }
    double coefsize = min_size / total_size;
    if (coefsize * smallest_rng < MIN_WID) {
        coefsize = MIN_WID / smallest_rng;
    }
    for (size_t i = 0; i < sizes.size(); ++i) {
        unsigned int ws = (unsigned int)(double(sizes[i]) * coefsize);
        idobj[i].l->set_size_request(ws, 95);
        if (nullptr == idobj[i].o) {
            idobj[i].l->set_tooltip_text(Glib::ustring::compose("Free space\nSize: %1", sizes[i]));
            idobj[i].l->override_background_color(ec[i & 1]);
        } else {
            idobj[i].l->set_tooltip_text(Glib::ustring::compose(
                "Allocated submemory\nId: %1\nSize: %2\nOffset: %3\nEnd: %4", i,
                idobj[i].o->get_size(), idobj[i].o->get_offset(), idobj[i].o->get_end()));
            idobj[i].l->override_background_color(fc[i & 1]);
        }
    }
    show_all();
}

void MemoryVisualizer::set_memory_manager(gearoenix::core::gc::Gc* mmgr)
{
    cleanup();
    this->mmgr = mmgr;
    visualize();
}

gearoenix::core::gc::Gc* MemoryVisualizer::get_memory_manager()
{
    return mmgr;
}

void MemoryVisualizer::cleanup()
{
    for (auto& o : idobj)
        delete o.l;
    idobj.clear();
    if (nullptr != mmgr)
        delete mmgr;
}

void MemoryVisualizer::create_submemory(unsigned int s)
{
    gearoenix::core::gc::Object* gobj = new gearoenix::core::gc::Object(s);
    mmgr->allocate(gobj);
    for (auto& o : idobj)
        delete o.l;
    idobj.clear();
    visualize();
}

MainWindow::MainWindow()
    : b_create_main_memory("Create Main Memory")
    , b_create_submemory("Create Submemory")
    , b_deallocate_submemory("Deallocate Submemory")
    , l_create_main_memory("Main Memory size: ")
    , l_create_submemory("Submemory size: ")
    , l_deallocate_submemory("Submemory ID: ")
    , memvis(new gearoenix::core::gc::Gc(1024))
{
    e_create_main_memory.set_text("1024");

    l_create_main_memory.set_halign(Gtk::ALIGN_START);
    l_create_submemory.set_halign(Gtk::ALIGN_START);
    l_deallocate_submemory.set_halign(Gtk::ALIGN_START);

    b_create_main_memory.signal_clicked().connect(sigc::mem_fun(this, &MainWindow::on_create_main_memory_clicked));
    b_create_submemory.signal_clicked().connect(sigc::mem_fun(this, &MainWindow::on_create_submemory_clicked));
    b_deallocate_submemory.signal_clicked().connect(sigc::mem_fun(this, &MainWindow::on_deallocate_submemory_clicked));

    grid.attach(l_create_main_memory, 0, 0, 1, 1);
    grid.attach(e_create_main_memory, 1, 0, 1, 1);
    grid.attach(b_create_main_memory, 2, 0, 1, 1);

    grid.attach(l_create_submemory, 0, 1, 1, 1);
    grid.attach(e_create_submemory, 1, 1, 1, 1);
    grid.attach(b_create_submemory, 2, 1, 1, 1);

    grid.attach(l_deallocate_submemory, 0, 2, 1, 1);
    grid.attach(e_deallocate_submemory, 1, 2, 1, 1);
    grid.attach(b_deallocate_submemory, 2, 2, 1, 1);

    grid.attach(memvis, 0, 3, 3, 1);

    grid.set_hexpand(true);
    grid.set_vexpand(false);
    grid.set_column_spacing(5);
    grid.set_row_spacing(5);

    add(grid);
    set_border_width(10);
    set_title("Gearoenix memory-manager visual debugger and tester");
    set_hexpand(false);
    set_vexpand(false);
    set_resizable(false);
    show_all();
    memvis.visualize();
    show_all();
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_create_main_memory_clicked()
{
    unsigned int memsize = atoi(e_create_main_memory.get_text().c_str());
    memvis.set_memory_manager(new gearoenix::core::gc::Gc(memsize));
    show_all();
}

void MainWindow::on_create_submemory_clicked()
{
    unsigned int memsize = atoi(e_create_submemory.get_text().c_str());
    memvis.create_submemory(memsize);
    show_all();
}

void MainWindow::on_deallocate_submemory_clicked()
{
    unsigned int memid = atoi(e_deallocate_submemory.get_text().c_str());
    memvis.remove(memid);
    show_all();
}

int main(int argc, char* argv[])
{
    auto app = Gtk::Application::create(argc, argv, "com.gearoenix.test002");
    MainWindow win;
    return app->run(win);
}

#endif
