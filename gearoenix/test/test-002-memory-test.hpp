#ifndef GEAROENIX_TEST_002
#define GEAROENIX_TEST_002
#include "config.hpp"
#ifdef TEST002
#include "../core/gc/cr-gc.hpp"
#include <gtkmm.h>

struct LObj {
    Gtk::Label* l;
    gearoenix::core::gc::Object* o;
};

class MemoryVisualizer : public Gtk::ScrolledWindow {
public:
    MemoryVisualizer(gearoenix::core::gc::Gc* mmgr);
    ~MemoryVisualizer();
    void remove(unsigned int id);
    void visualize();
    void create_submemory(unsigned int s);

private:
    Gtk::Grid grid;
    Gdk::RGBA ec[2];
    Gdk::RGBA fc[2];
    gearoenix::core::gc::Gc* mmgr;
    std::vector<LObj> idobj;
};

class MainWindow : public Gtk::Window {
public:
    MainWindow();
    ~MainWindow();

private:
    void on_create_main_memory_clicked();
    void on_create_submemory_clicked();
    void on_deallocate_submemory_clicked();

    Gtk::Button b_create_main_memory;
    Gtk::Button b_create_submemory;
    Gtk::Button b_deallocate_submemory;
    Gtk::Label l_create_main_memory;
    Gtk::Label l_create_submemory;
    Gtk::Label l_deallocate_submemory;
    Gtk::Entry e_create_main_memory;
    Gtk::Entry e_create_submemory;
    Gtk::Entry e_deallocate_submemory;
    Gtk::Grid grid;

    MemoryVisualizer memvis;
};

#endif
#endif
