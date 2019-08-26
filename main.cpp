#include <iostream>
#include <gtk/gtk.h>
#include "core/i8080_state.h"

static void activate(GtkApplication*, gpointer);

int main(int argc, char *argv[]) {
    i8080_state state{};
    printf("SysSize: %li bytes\n", sizeof(state));

    static_assert(sizeof(state.AF) == 2, "");
    static_assert(sizeof(state.BC) == 2, "");
    static_assert(sizeof(state.DE) == 2, "");
    static_assert(sizeof(state.HL) == 2, "");
    static_assert(sizeof(state.SP) == 2, "");
    static_assert(sizeof(state.PC) == 2, "");

    printf("AF=0x%04X\n", state.AF.af);


    GtkApplication *app;
    int status;

    app = gtk_application_new("dev.imsai.i8080sim", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}

static void activate(GtkApplication* app, gpointer user_data) {
    GtkWidget *window;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Intel 8080 Emulator");
    gtk_window_set_default_size(GTK_WINDOW(window), 640, 640);

    gtk_widget_show_all(window);
}
