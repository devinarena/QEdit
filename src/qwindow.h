
#ifndef QEDIT_WINDOW_H
#define QEDIT_WINDOW_H

#include <stdint.h>

#include "dyn_list.h"

typedef struct {
    dyn_list* lines;
    uint32_t cx;
    uint32_t cy;
    uint32_t width;
    uint32_t height;
} qedit_window;

qedit_window* new_qedit_window();
void clear_window();
void rerender(qedit_window* window);
void destroy_qedit_window(qedit_window* window);

#endif