
#ifndef QEDIT_WINDOW_H
#define QEDIT_WINDOW_H

#include <stdint.h>

#include "dyn_list.h"

typedef struct {
    dyn_list* lines;
    const char* filename;
    
    uint32_t line;
    uint32_t col;
    uint32_t scroll_y;
    uint32_t cx;
    uint32_t cy;
    uint32_t width;
    uint32_t height;
    uint32_t edit_window_height;
} qedit_window;

qedit_window* new_qedit_window(const char* filename);
void set_cursor_pos(qedit_window* window);
void clear_window();
void render_info(qedit_window* window);
void rerender(qedit_window* window);
void start_listener(qedit_window* window);
void destroy_qedit_window(qedit_window* window);

#endif