//
// Created by codetector on 8/26/19.
//

#include "ui_utils.h"

void gwemu_setUint8(GtkWidget *widgets[], uint8_t value) {
    for (uint8_t i = 0; i < 8; i++) {
        gtk_image_set_from_icon_name(GTK_IMAGE(widgets[i]),
                                     (value & (1U << i)) ? "gtk-yes" : "gtk-no",
                                     GTK_ICON_SIZE_LARGE_TOOLBAR);
    }
}
