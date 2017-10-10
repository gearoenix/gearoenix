#include "test-002-memory-test.hpp"
#ifdef TEST002

MainWindow::MainWindow()
    : b_create_main_memory("Create Main Memory")
    , b_create_submemory("Create Submemory")
    , b_deallocate_submemory("Deallocate Submemory")
    , l_create_main_memory("Main Memory size: ")
    , l_create_submemory("Submemory size: ")
    , l_deallocate_submemory("Submemory ID: ")
{
    l_create_main_memory.set_halign(Gtk::ALIGN_START);
    l_create_submemory.set_halign(Gtk::ALIGN_START);
    l_deallocate_submemory.set_halign(Gtk::ALIGN_START);

    grid.attach(l_create_main_memory, 0, 0, 1, 1);
    grid.attach(e_create_main_memory, 1, 0, 1, 1);
    grid.attach(b_create_main_memory, 2, 0, 1, 1);

    grid.attach(l_create_submemory, 0, 1, 1, 1);
    grid.attach(e_create_submemory, 1, 1, 1, 1);
    grid.attach(b_create_submemory, 2, 1, 1, 1);

    grid.attach(l_deallocate_submemory, 0, 2, 1, 1);
    grid.attach(e_deallocate_submemory, 1, 2, 1, 1);
    grid.attach(b_deallocate_submemory, 2, 2, 1, 1);

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
}

MainWindow::~MainWindow()
{
}

int main(int argc, char* argv[])
{
    auto app = Gtk::Application::create(argc, argv, "com.gearoenix.test002");
    MainWindow win;
    win.show_all();
    return app->run(win);
}

#endif
