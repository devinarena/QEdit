
#include <stdio.h>
#include <stdlib.h>

#include <conio.h>
#include <windows.h>

#include "qstring.h"
#include "qwindow.h"

#define INFO_LINES 1
#define EXTRA_LINE_CHARS 2

qedit_window* new_qedit_window(const char* filename) {
  qedit_window* window = malloc(sizeof(qedit_window));
  window->cx = 0;
  window->cy = 0;
  window->scroll_y = 0;
  window->lines = new_dyn_list(1, (void*)qstring_destroy);
  window->line = 0;
  window->col = 0;
  window->filename = filename;

  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
  window->width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  window->height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
  window->edit_window_height = window->height - INFO_LINES;

  return window;
}

void set_cursor_pos(qedit_window* window) {
  COORD pos = {window->cx, window->cy};
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void clear_window() {
  system("cls");
}

void rerender(qedit_window* window) {
  clear_window();
  for (int col = window->scroll_y; col < window->height - 1; col++) {
    if (col >= window->lines->size) {
      printf("\n.");
      continue;
    }
    qstring* line = dyn_list_get(window->lines, col);
    printf("%s", line->str);
  }
  render_info(window);
}

void render_info(qedit_window* window) {
  uint32_t oldX = window->cx;
  uint32_t oldY = window->cy;

  window->cx = 0;
  window->cy = window->height - 1;
  set_cursor_pos(window);

  char info[200];
  snprintf(info, sizeof(info), "FILE: %s --- POS: (%d, %d)", window->filename,
           window->line, window->col);

  printf("%-*s", window->width, info);

  window->cx = oldX;
  window->cy = oldY;
  set_cursor_pos(window);
}

static void move_cursor_up(qedit_window* window, qstring** line) {
  if (window->col >= window->width) {
    window->col -= window->width;
    window->cy--;
  } else {
    if (window->line > 0) {
      window->line--;
      *line = dyn_list_get(window->lines, window->line);
      window->cy -= (int)((*line)->length / window->width) + 1;
      window->col = 0;
      window->cx = 0;
    }
  }
  set_cursor_pos(window);
  render_info(window);
}

static void move_cursor_down(qedit_window* window, qstring** line) {
  if (window->col + window->width < (*line)->length - 1) {
    window->cy++;
    window->col += window->width;
  } else {
    if (window->line < window->lines->size - 1) {
      window->cy += (int)(((*line)->length - window->col) / window->width) + 1;
      window->line++;
      *line = dyn_list_get(window->lines, window->line);
      window->col = 0;
      window->cx = 0;
    }
  }
  set_cursor_pos(window);
  render_info(window);
}

static void move_cursor_left(qedit_window* window, qstring** line) {
  short padding = window->cy < window->lines->size ? EXTRA_LINE_CHARS : 0;
  if (window->col > 0) {
    window->col--;
    if (window->cx > 0)
      window->cx--;
    else {
      window->cy--;
      window->cx = window->width - 1;
    }
  } else {
    if (window->line > 0) {
      window->line--;
      *line = dyn_list_get(window->lines, window->line);
      window->col = (*line)->length - padding;
      window->cx = window->col % window->width;
      window->cy--;
    }
  }
  set_cursor_pos(window);
  render_info(window);
}

static void move_cursor_right(qedit_window* window, qstring** line) {
  short padding = window->cy < window->lines->size ? EXTRA_LINE_CHARS : 0;
  if (window->col < (*line)->length - padding) {
    window->col++;
    window->cx++;
  } else {
    if (window->line < window->lines->size - 1) {
      window->line++;
      *line = dyn_list_get(window->lines, window->line);
      window->col = 0;
      window->cx = 0;
      window->cy++;
    }
  }
  set_cursor_pos(window);
  render_info(window);
}

void start_listener(qedit_window* window) {
  int running = 1;
  qstring* line = dyn_list_get(window->lines, window->line);
  while (running) {
    if (kbhit()) {
      char c = _getch();
      if (c == 27) {
        running = 0;
      }
      if (c == -32 || c == 224) {
        c = _getch();
        switch (c) {
          case 72: {  // UP
            move_cursor_up(window, &line);
            break;
          }
          case 80: {  // DOWN
            move_cursor_down(window, &line);
            break;
          }
          case 75: {  // LEFT
            move_cursor_left(window, &line);
            break;
          }
          case 77: {  // RIGHT
            move_cursor_right(window, &line);
            break;
          }
          case 71: {
            rerender(window);
            break;
          }
        }
      } else {
        if (isalnum(c)) {
          putch(c);
          window->cx++;
          set_cursor_pos(window);
          render_info(window);
        }
      }
    }
  }
}

void destroy_qedit_window(qedit_window* window) {
  dyn_list_destroy(window->lines);
  free(window);
}
