#include <gtk/gtk.h>
#include "stdlib.h"
#include "ui/ui_utils.h"
#include "core/i8080_state.h"

struct i8080_state master_state;
GtkWidget
        *reg_a[8], *reg_b[8], *reg_c[8], *reg_d[8], *reg_e[8], *reg_h[8], *reg_l[8],
        *reg_sp[16], *reg_pc[16];

GtkWidget *flag_s, *flag_z, *flag_ac, *flag_p, *flag_c, *flag_run;
GtkWidget *console, *console_scroll;
GtkWidget *dataDisplay[8];
GtkWidget *window;

struct consoleAppend {
    char *buffer;
    int len;
};

gboolean _gwemu_appendToConsole(void *userdata) {
    static GtkTextBuffer *buffer = NULL;
    if (buffer == NULL) {
        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(console));
    }
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);
    gtk_text_buffer_insert(buffer, &end, ((struct consoleAppend *) userdata)->buffer,
                           ((struct consoleAppend *) userdata)->len);

    GtkAdjustment *adj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(console_scroll));
    gtk_adjustment_set_value(adj, gtk_adjustment_get_upper(adj));
    free(((struct consoleAppend*)userdata)->buffer);
    free(userdata);
    return FALSE;
}

void gwemu_btn_run() {
    pthread_mutex_lock(&master_state.lock);
    master_state.halt = FALSE;
    pthread_mutex_unlock(&master_state.lock);
}

void gwemu_btn_stop() {
    pthread_mutex_lock(&master_state.lock);
    master_state.halt = TRUE;
    pthread_mutex_unlock(&master_state.lock);
}

void gwemu_appendToConsole(char *spBuffer, int len) {
    struct consoleAppend *dah = malloc(sizeof(struct consoleAppend));
    dah->buffer = spBuffer;
    dah->len = len;
    usleep(1000);
    g_main_context_invoke(NULL, _gwemu_appendToConsole, dah);
}

void gwemu_out_port(uint8_t port, uint8_t data) {
    char *spBuffer = malloc(sizeof(char)*30);
    int len = sprintf(spBuffer, "OUT:(0x%02X) => 0x%02X\n", port, data);
    gwemu_appendToConsole(spBuffer, len);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void *gwemu_run_loop(void *arg) {
    uint16_t counter;
    time_t lastCheck;
    for (;;) {
        if (!master_state.halt) {
            gwemu_exec_step(&master_state);
        } else {
            usleep(1000);
        }
        g_usleep(0);
    }
}

#pragma clang diagnostic pop

// called when window is closed
void on_window_main_destroy(GtkWidget *widget, gpointer userdata) {
    gtk_main_quit();
}

void gwemu_btn_step() {
    gwemu_exec_step(&master_state);
}

void gwemu_btn_reset() {
    pthread_mutex_lock(&master_state.lock);
    master_state.a = 0;
    *(uint8_t *) &master_state.f = 0; // TOTAL HACK
    master_state.b = 0;
    master_state.c = 0;
    master_state.d = 0;
    master_state.e = 0;
    master_state.h = 0;
    master_state.l = 0;
    master_state.sp = 0;
    master_state.pc = 0;
    pthread_mutex_unlock(&master_state.lock);
}

void gwemu_btn_load_bin() {
    GtkWidget *dialog;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Open File",
                                         GTK_WINDOW(window),
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "_Cancel",
                                         GTK_RESPONSE_CANCEL,
                                         "_Open",
                                         GTK_RESPONSE_ACCEPT,
                                         NULL);

    res = gtk_dialog_run(GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
        filename = gtk_file_chooser_get_filename(chooser);
        g_print("file: %s\n", filename);
//        gwemu_appendToConsole("Loading bin...\n", -1);

        FILE *file = fopen(filename, "r");
        if (file == NULL) {
            return;
        }
        uint16_t loc = 0;
        for (;;) {
            uint8_t byte;
            size_t size = fread(&byte, 1, 1, file);
            if (size == 0) {
                break;
            }
            master_state.memory[loc++] = byte;
            if (loc == 0) {
                g_print("File Too large");
//                gwemu_appendToConsole("File Too Large\n", -1);
                break;
            }
        }
        fclose(file);
//        gwemu_appendToConsole("Done\n", -1);

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

gboolean gwemu_btn_examine_next() {
    pthread_mutex_lock(&master_state.lock);
    master_state.pc++;
    pthread_mutex_unlock(&master_state.lock);
    return FALSE;
}

gboolean gwemu_loop(gpointer userdata) {
    pthread_mutex_lock(&master_state.lock);
    static uint8_t cache_a = 0xFF, cache_b = 0xFF, cache_c = 0xFF, cache_d = 0xFF, cache_e = 0xFF, cache_h = 0xFF, cache_l = 0xFF;
    static uint16_t cache_pc = 0xFFFF, cache_sp = 0xFFFF;

    if (master_state.a != cache_a) {
        cache_a = master_state.a;
        gwemu_setUint8_to_register_widgets(reg_a, master_state.a);
    }
    if (master_state.b != cache_b) {
        cache_b = master_state.b;
        gwemu_setUint8_to_register_widgets(reg_b, master_state.b);
    }
    if (master_state.c != cache_c) {
        cache_c = master_state.c;
        gwemu_setUint8_to_register_widgets(reg_c, master_state.c);
    }
    if (master_state.d != cache_d) {
        cache_d = master_state.d;
        gwemu_setUint8_to_register_widgets(reg_d, master_state.d);
    }
    if (master_state.e != cache_e) {
        cache_e = master_state.e;
        gwemu_setUint8_to_register_widgets(reg_e, master_state.e);
    }
    if (master_state.h != cache_h) {
        cache_h = master_state.h;
        gwemu_setUint8_to_register_widgets(reg_h, master_state.h);
    }
    if (master_state.l != cache_l) {
        cache_l = master_state.l;
        gwemu_setUint8_to_register_widgets(reg_l, master_state.l);
    }
    if (master_state.sp != cache_sp) {
        cache_sp = master_state.sp;
        gwemu_setUint16_to_register_widgets(reg_sp, master_state.sp);
    }
    if (master_state.pc != cache_pc) {
        cache_pc = master_state.pc;
        gwemu_setUint16_to_register_widgets(reg_pc, master_state.pc);
    }
    gwemu_setUint8_to_register_widgets(dataDisplay, master_state.memory[master_state.pc]);


    gtk_image_set_from_icon_name(GTK_IMAGE(flag_z),
                                 (master_state.f.z) ? "gtk-yes" : "gtk-no",
                                 GTK_ICON_SIZE_LARGE_TOOLBAR);
    gtk_image_set_from_icon_name(GTK_IMAGE(flag_p),
                                 (master_state.f.p) ? "gtk-yes" : "gtk-no",
                                 GTK_ICON_SIZE_LARGE_TOOLBAR);
    gtk_image_set_from_icon_name(GTK_IMAGE(flag_ac),
                                 (master_state.f.ac) ? "gtk-yes" : "gtk-no",
                                 GTK_ICON_SIZE_LARGE_TOOLBAR);
    gtk_image_set_from_icon_name(GTK_IMAGE(flag_s),
                                 (master_state.f.s) ? "gtk-yes" : "gtk-no",
                                 GTK_ICON_SIZE_LARGE_TOOLBAR);
    gtk_image_set_from_icon_name(GTK_IMAGE(flag_c),
                                 (master_state.f.c) ? "gtk-yes" : "gtk-no",
                                 GTK_ICON_SIZE_LARGE_TOOLBAR);
    gtk_image_set_from_icon_name(GTK_IMAGE(flag_run),
                                 (master_state.halt) ? "gtk-stop" : "gtk-yes",
                                 GTK_ICON_SIZE_LARGE_TOOLBAR);
    pthread_mutex_unlock(&master_state.lock);
    return TRUE;
}

int main(int argc, char *argv[]) {
    if (pthread_mutex_init(&master_state.lock, NULL) != 0) {
        exit - 1;
    };

    master_state.halt = TRUE;
    master_state.out_port = gwemu_out_port;
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working dir: %s\n", cwd);
    } else {
        perror("getcwd() error");
        return 1;
    }

    GtkBuilder *builder;
    GError *err = NULL;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "glade/window_main.glade", &err);
    if (err != NULL) {
        fprintf(stderr, "Unable to read file: %s\n", err->message);
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


    flag_s = GTK_WIDGET(gtk_builder_get_object(builder, "flag_s"));
    flag_z = GTK_WIDGET(gtk_builder_get_object(builder, "flag_z"));
    flag_ac = GTK_WIDGET(gtk_builder_get_object(builder, "flag_ac"));
    flag_p = GTK_WIDGET(gtk_builder_get_object(builder, "flag_p"));
    flag_c = GTK_WIDGET(gtk_builder_get_object(builder, "flag_c"));
    flag_run = GTK_WIDGET(gtk_builder_get_object(builder, "status_light"));

    console = GTK_WIDGET(gtk_builder_get_object(builder, "console"));
    console_scroll = GTK_WIDGET(gtk_builder_get_object(builder, "console_scroll"));

    g_object_unref(builder);

    gtk_widget_show_all(window);

    g_idle_add(gwemu_loop, NULL);
//    g_timeout_add(50, gwemu_loop, NULL);

    pthread_t cThread;
    pthread_create(&cThread, NULL, gwemu_run_loop, NULL);

    gtk_main();
    return 0;
}

