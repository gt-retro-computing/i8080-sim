#include <gtk/gtk.h>

GtkWidget *g_lbl_hello;
GtkWidget *g_lbl_count;

extern "C" {
void on_btn_hello_clicked(GtkButton *button, gpointer userdata) {
    static unsigned int count = 0;
    char str_count[30] = {0};

    gtk_label_set_text(GTK_LABEL(g_lbl_hello), "Hello, world aaaaaaaaaaaa!");
    count++;
    sprintf(str_count, "%d", count);
    gtk_label_set_text(GTK_LABEL(g_lbl_count), str_count);
}

// called when window is closed
void on_window_main_destroy(GtkWidget *widget, gpointer userdata) {
    gtk_main_quit();
}
}

int main(int argc, char *argv[]) {
    GtkBuilder *builder;
    GtkWidget *window;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "glade/window_main.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    gtk_builder_connect_signals(builder, NULL);

    g_lbl_hello = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_hello"));
    g_lbl_count = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_count"));

    g_object_unref(G_OBJECT(builder));

    gtk_widget_show(window);
    gtk_main();

    return 0;
}

