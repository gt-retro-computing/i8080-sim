//
// Created by codetector on 8/26/19.
//
#include <gtk/gtk.h>
#include "stdlib.h"

#ifndef I8080SIM_UI_UTILS_H
#define I8080SIM_UI_UTILS_H

void gwemu_setUint8_to_register_widgets(GtkWidget **widgets, uint8_t value);
void gwemu_setUint16_to_register_widgets(GtkWidget **widgets, uint8_t value);

#endif //I8080SIM_UI_UTILS_H
