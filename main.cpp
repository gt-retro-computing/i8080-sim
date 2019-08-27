#include <gtk/gtk.h>
#include <core/i8080_state.h>

extern "C" {
#include "ui/ui_utils.h"
#include "core/i8080_state.h"
}

struct i8080_state master_state{};
GtkWidget
        *reg_a[8], *reg_b[8], *reg_c[8], *reg_d[8], *reg_e[8], *reg_h[8], *reg_l[8],
        *reg_sp[16], *reg_pc[16];

extern "C" {
// called when window is closed
void on_window_main_destroy(GtkWidget *widget, gpointer userdata) {
    gtk_main_quit();
}
void gwemu_btn_step_click() {
}
}

gboolean gwemu_loop(gpointer userdata) {

    gwemu_setUint8_to_register_widgets(reg_a, master_state.AF.sub.a);
    gwemu_setUint8_to_register_widgets(reg_b, master_state.BC.sub.b);
    gwemu_setUint8_to_register_widgets(reg_c, master_state.BC.sub.c);
    gwemu_setUint8_to_register_widgets(reg_d, master_state.DE.sub.d);
    gwemu_setUint8_to_register_widgets(reg_e, master_state.DE.sub.e);
    gwemu_setUint8_to_register_widgets(reg_h, master_state.HL.sub.h);
    gwemu_setUint8_to_register_widgets(reg_l, master_state.HL.sub.l);
    gwemu_setUint16_to_register_widgets(reg_sp, master_state.SP.sp);
    gwemu_setUint16_to_register_widgets(reg_sp, master_state.PC.pc);

    return true;
}

int main(int argc, char *argv[]) {

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working dir: %s\n", cwd);
    } else {
        perror("getcwd() error");
        return 1;
    }

    GtkBuilder *builder;
    GtkWidget *window;
    GError *err;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "glade/window_main.glade", &err);
    if(err != NULL)
    {
        fprintf(stderr,"Unable to read file: %s\n", err->message);
        g_error_free(err);
        return 1;
    }

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    gtk_builder_connect_signals(builder, NULL);

    // Register A
    reg_a[0] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_a_0"));
    reg_a[1] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_a_1"));
    reg_a[2] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_a_2"));
    reg_a[3] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_a_3"));
    reg_a[4] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_a_4"));
    reg_a[5] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_a_5"));
    reg_a[6] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_a_6"));
    reg_a[7] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_a_7"));

    reg_b[0] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_b_0"));
    reg_b[1] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_b_1"));
    reg_b[2] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_b_2"));
    reg_b[3] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_b_3"));
    reg_b[4] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_b_4"));
    reg_b[5] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_b_5"));
    reg_b[6] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_b_6"));
    reg_b[7] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_b_7"));

    reg_c[0] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_c_0"));
    reg_c[1] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_c_1"));
    reg_c[2] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_c_2"));
    reg_c[3] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_c_3"));
    reg_c[4] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_c_4"));
    reg_c[5] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_c_5"));
    reg_c[6] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_c_6"));
    reg_c[7] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_c_7"));

    reg_d[0] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_d_0"));
    reg_d[1] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_d_1"));
    reg_d[2] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_d_2"));
    reg_d[3] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_d_3"));
    reg_d[4] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_d_4"));
    reg_d[5] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_d_5"));
    reg_d[6] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_d_6"));
    reg_d[7] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_d_7"));

    reg_e[0] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_e_0"));
    reg_e[1] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_e_1"));
    reg_e[2] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_e_2"));
    reg_e[3] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_e_3"));
    reg_e[4] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_e_4"));
    reg_e[5] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_e_5"));
    reg_e[6] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_e_6"));
    reg_e[7] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_e_7"));

    reg_h[0] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_h_0"));
    reg_h[1] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_h_1"));
    reg_h[2] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_h_2"));
    reg_h[3] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_h_3"));
    reg_h[4] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_h_4"));
    reg_h[5] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_h_5"));
    reg_h[6] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_h_6"));
    reg_h[7] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_h_7"));

    reg_l[0] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_l_0"));
    reg_l[1] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_l_1"));
    reg_l[2] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_l_2"));
    reg_l[3] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_l_3"));
    reg_l[4] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_l_4"));
    reg_l[5] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_l_5"));
    reg_l[6] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_l_6"));
    reg_l[7] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_l_7"));

    reg_sp[0] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_sp_0"));
    reg_sp[1] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_sp_1"));
    reg_sp[2] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_sp_2"));
    reg_sp[3] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_sp_3"));
    reg_sp[4] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_sp_4"));
    reg_sp[5] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_sp_5"));
    reg_sp[6] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_sp_6"));
    reg_sp[7] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_sp_7"));
    reg_sp[8] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_sp_8"));
    reg_sp[9] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_sp_9"));
    reg_sp[10] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_sp_10"));
    reg_sp[11] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_sp_11"));
    reg_sp[12] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_sp_12"));
    reg_sp[13] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_sp_13"));
    reg_sp[14] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_sp_14"));
    reg_sp[14] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_sp_15"));

    reg_pc[0] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_pc_0"));
    reg_pc[1] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_pc_1"));
    reg_pc[2] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_pc_2"));
    reg_pc[3] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_pc_3"));
    reg_pc[4] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_pc_4"));
    reg_pc[5] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_pc_5"));
    reg_pc[6] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_pc_6"));
    reg_pc[7] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_pc_7"));
    reg_pc[8] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_pc_8"));
    reg_pc[9] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_pc_9"));
    reg_pc[10] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_pc_10"));
    reg_pc[11] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_pc_11"));
    reg_pc[12] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_pc_12"));
    reg_pc[13] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_pc_13"));
    reg_pc[14] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_pc_14"));
    reg_pc[14] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_pc_15"));

    g_object_unref(builder);

    gtk_widget_show(window);

    g_timeout_add(30, gwemu_loop, NULL);

    gtk_main();
    return 0;
}

