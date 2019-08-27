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

GtkWidget *dataDisplay[8];

extern "C" {
// called when window is closed
void on_window_main_destroy(GtkWidget *widget, gpointer userdata) {
    gtk_main_quit();
}
void gwemu_btn_step() {
}

void gwemu_btn_reset() {
    master_state.PC.pc=0;
    master_state.AF.af=0;
    master_state.BC.bc=0;
    master_state.DE.de=0;
    master_state.HL.hl=0;
    master_state.SP.sp=0;
}

gboolean gwemu_btn_examine_next() {
    master_state.PC.pc++;
    return false;
}

gboolean custom_draw(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    guint width, height;
    GdkRGBA color;
    GtkStyleContext *context;

    context = gtk_widget_get_style_context (widget);

    width = gtk_widget_get_allocated_width (widget);
    height = gtk_widget_get_allocated_height (widget);

    gtk_render_background (context, cr, 0, 0, width, height);

    cairo_arc (cr,
               width / 2.0, height / 2.0,
               MIN (width, height) / 2.0,
               0, 2 * G_PI);

    gtk_style_context_get_color (context,
                                 gtk_style_context_get_state (context),
                                 &color);
    gdk_cairo_set_source_rgba (cr, &color);

    cairo_fill (cr);

    return FALSE;
}
}

gboolean gwemu_loop(gpointer userdata) {
    static uint8_t cache_a=0xFF, cache_b=0xFF, cache_c=0xFF, cache_d=0xFF, cache_e=0xFF, cache_h=0xFF, cache_l=0xFF;
    static uint16_t cache_pc=0xFFFF, cache_sp=0xFFFF;

    if (master_state.AF.sub.a != cache_a) {
        cache_a = master_state.AF.sub.a;
        gwemu_setUint8_to_register_widgets(reg_a, master_state.AF.sub.a);
    }
    if (master_state.BC.sub.b != cache_b) {
        cache_b = master_state.BC.sub.b;
        gwemu_setUint8_to_register_widgets(reg_b, master_state.BC.sub.b);
    }
    if (master_state.BC.sub.c != cache_c) {
        cache_c = master_state.BC.sub.c;
        gwemu_setUint8_to_register_widgets(reg_c, master_state.BC.sub.c);
    }
    if (master_state.DE.sub.d != cache_d) {
        cache_d = master_state.DE.sub.d;
        gwemu_setUint8_to_register_widgets(reg_d, master_state.DE.sub.d);
    }
    if (master_state.DE.sub.e != cache_e) {
        cache_e = master_state.DE.sub.e;
        gwemu_setUint8_to_register_widgets(reg_e, master_state.DE.sub.e);
    }
    if (master_state.HL.sub.h != cache_h) {
        cache_h = master_state.HL.sub.h;
        gwemu_setUint8_to_register_widgets(reg_h, master_state.HL.sub.h);
    }
    if (master_state.HL.sub.l != cache_l) {
        cache_l = master_state.HL.sub.l;
        gwemu_setUint8_to_register_widgets(reg_l, master_state.HL.sub.l);
    }
    gwemu_setUint16_to_register_widgets(reg_sp, master_state.SP.sp);
    if (master_state.PC.pc != cache_pc) {
        cache_pc = master_state.PC.pc;
        gwemu_setUint16_to_register_widgets(reg_pc, master_state.PC.pc);
    }
    gwemu_setUint8_to_register_widgets(dataDisplay, master_state.memory[master_state.PC.pc]);

    return true;
}

int main(int argc, char *argv[]) {

    uint8_t *ram = master_state.memory;

    ram[0] = 0303;

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working dir: %s\n", cwd);
    } else {
        perror("getcwd() error");
        return 1;
    }

    GtkBuilder *builder;
    GtkWidget *window;
    GError *err = NULL;

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

    dataDisplay[0] = GTK_WIDGET(gtk_builder_get_object(builder, "d_data_0"));
    dataDisplay[1] = GTK_WIDGET(gtk_builder_get_object(builder, "d_data_1"));
    dataDisplay[2] = GTK_WIDGET(gtk_builder_get_object(builder, "d_data_2"));
    dataDisplay[3] = GTK_WIDGET(gtk_builder_get_object(builder, "d_data_3"));
    dataDisplay[4] = GTK_WIDGET(gtk_builder_get_object(builder, "d_data_4"));
    dataDisplay[5] = GTK_WIDGET(gtk_builder_get_object(builder, "d_data_5"));
    dataDisplay[6] = GTK_WIDGET(gtk_builder_get_object(builder, "d_data_6"));
    dataDisplay[7] = GTK_WIDGET(gtk_builder_get_object(builder, "d_data_7"));

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
    reg_sp[15] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_sp_15"));

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
    reg_pc[15] = GTK_WIDGET(gtk_builder_get_object(builder, "reg_pc_15"));

    g_object_unref(builder);

    gtk_widget_show(window);

    g_timeout_add(50, gwemu_loop, NULL);

    gtk_main();
    return 0;
}

