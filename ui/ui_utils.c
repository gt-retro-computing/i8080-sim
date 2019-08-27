//
// Created by codetector on 8/26/19.
//

#include "ui_utils.h"

//__attribute__((optimize("unroll-loops")))
void gwemu_setUint8_to_register_widgets(GtkWidget **widgets, uint8_t value) {
    for (uint8_t i = 0; i < 8; i++) {
        gtk_image_set_from_icon_name(GTK_IMAGE(widgets[i]),
                                     (value & (1U << i)) ? "gtk-yes" : "gtk-no",
                                     GTK_ICON_SIZE_LARGE_TOOLBAR);
    }
}

//__attribute__((optimize("unroll-loops")))
void gwemu_setUint16_to_register_widgets(GtkWidget **widgets, uint16_t value) {
    for (uint8_t i = 0; i < 16; i++) {
        gtk_image_set_from_icon_name(GTK_IMAGE(widgets[i]),
                                     (value & ( 1U << i)) ? "gtk-yes" : "gtk-no",
                                     GTK_ICON_SIZE_LARGE_TOOLBAR);
    }
}
