#include <gtk/gtk.h>

extern "C" {
#include "ui/ui_utils.h"
}

GtkWidget *reg_a[8];

extern "C" {
// called when window is closed
void on_window_main_destroy(GtkWidget *widget, gpointer userdata) {
    gtk_main_quit();
}
void gwemu_SS_click() {
    static uint8_t counter = 0;
    counter++;
    gwemu_setUint8(reg_a, counter);
}
}

gboolean gwemu_loop(gpointer userdata) {
    static uint8_t counter = 0;
    counter++;
    gwemu_setUint8(reg_a, counter);

    return true;
}

int main(int argc, char *argv[]) {
    GtkBuilder *builder;
    GtkWidget *window;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "glade/window_main.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    gtk_builder_connect_signals(builder, NULL);

    reg_a[0] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_a_0"));
    reg_a[1] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_a_1"));
    reg_a[2] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_a_2"));
    reg_a[3] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_a_3"));
    reg_a[4] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_a_4"));
    reg_a[5] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_a_5"));
    reg_a[6] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_a_6"));
    reg_a[7] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_a_7"));

    g_object_unref(G_OBJECT(builder));

    gtk_widget_show(window);
    gtk_window_set_keep_above(GTK_WINDOW(window), gtk_true());
    gtk_window_set_keep_above(GTK_WINDOW(window), gtk_false());

    g_timeout_add(16, gwemu_loop, NULL);

    gtk_main();
    return 0;
}

