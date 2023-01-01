
#include <stdio.h>
#include <stdlib.h>

#include <conio.h>
#include <windows.h>

#include "qstring.h"
#include "qwindow.h"

#define INFO_LINES 1
#define EXTRA_LINE_CHARS 2

// track last key
int last_key = 0;

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
  snprintf(info, sizeof(info), "FILE: %s --- POS: (%d, %d) --- PREV KS: %d",
           window->filename, window->line, window->col, last_key);

  printf("%-*s", window->width, info);

  window->cx = oldX;
  window->cy = oldY;
  set_cursor_pos(window);
}

static qstring* get_current_line(qedit_window* window) {
  return dyn_list_get(window->lines, window->line);
}

static void cursor_right(qedit_window* window) {
  window->cx++;
  if (window->cx >= window->width) {
    window->cx = 0;
    window->cy++;
  }
}

static void cursor_left(qedit_window* window) {
  if (window->cx > 0) {
    window->cx--;
  } else {
    window->cx = window->width - 1;
    window->cy--;
  }
}

static void move_cursor_up(qedit_window* window) {
  if (window->col >= window->width) {
    window->col -= window->width;
    window->cy--;
  } else {
    if (window->line > 0) {
      window->line--;
      int length = get_current_line(window)->length;
      window->cy -= (int)(length / window->width) + 1;
      window->col = 0;
      window->cx = 0;
    }
  }
  set_cursor_pos(window);
  render_info(window);
}

static void move_cursor_down(qedit_window* window) {
  qstring* line = get_current_line(window);

  if (window->col + window->width < line->length - 1) {
    window->cy++;
    window->col += window->width;
  } else {
    if (window->line < window->lines->size - 1) {
      window->cy +=
          (int)((line->length - window->col + 1) / window->width) + 1;
      window->line++;
      window->col = 0;
      window->cx = 0;
    }
  }
  set_cursor_pos(window);
  render_info(window);
}

static void move_cursor_left(qedit_window* window) {
  qstring* line = get_current_line(window);

  short padding = window->cy < window->lines->size ? EXTRA_LINE_CHARS : 0;
  if (window->col > 0) {
    window->col--;
    cursor_left(window);
  } else {
    if (window->line > 0) {
      window->line--;
      window->col = get_current_line(window)->length - padding;
      window->cx = window->col % window->width;
      window->cy--;
    }
  }
  set_cursor_pos(window);
  render_info(window);
}

static void move_cursor_right(qedit_window* window) {
  qstring* line = get_current_line(window);

  short padding = window->cy < window->lines->size ? EXTRA_LINE_CHARS : 0;
  if (window->col < line->length - padding) {
    window->col++;
    cursor_right(window);
  } else {
    if (window->line < window->lines->size - 1) {
      window->line++;
      window->col = 0;
      window->cx = 0;
      window->cy++;
    }
  }
  set_cursor_pos(window);
  render_info(window);
}

static void put_character(qedit_window* window, char c) {
  qstring* line = get_current_line(window);
  qstring_insert(line, c, window->col);
  window->col++;
  cursor_right(window);
  putch(c);
  printf("%s", line->str + window->col);
  set_cursor_pos(window);
  render_info(window);
}

static void backspace(qedit_window* window) {
  qstring* line = get_current_line(window);

  if (window->col > 0) {
    qstring_delete(line, window->col - 1);
    window->col--;
    cursor_left(window);
    set_cursor_pos(window);
    printf("%s ", line->str + window->col - 1);
    set_cursor_pos(window);
    render_info(window);
  } else {
    if (window->line > 0) {
      window->line--;
      qstring* prev_line = get_current_line(window);
      window->col = prev_line->length - EXTRA_LINE_CHARS;
      window->cx = window->col % window->width;
      window->cy--;
      if (line->length != 0 && strcmp(line->str, "\r") != 0 &&
          strcmp(line->str, "\r\n") != 0) {
        qstring_concat(prev_line, line, 0);
      }
      dyn_list_remove(window->lines, window->line + 1);
      rerender(window);
    }
  }
}

static void special_key(qedit_window* window, char c) {
  switch (c) {
    case 72: {  // UP
      move_cursor_up(window);
      break;
    }
    case 80: {  // DOWN
      move_cursor_down(window);
      break;
    }
    case 75: {  // LEFT
      move_cursor_left(window);
      break;
    }
    case 77: {  // RIGHT
      move_cursor_right(window);
      break;
    }
    case 71: {
      rerender(window);
      break;
    }
  }
}

void start_listener(qedit_window* window) {
  int running = 1;
  while (running) {
    if (kbhit()) {
      char c = _getch();
      if (c == 27) {
        running = 0;
      }
      if (c == '\b') {
        backspace(window);
        continue;
      }
      if (c == -32 || c == 224) {
        c = _getch();
        last_key = c;
        special_key(window, c);
      } else {
        if (isalnum(c)) {
          last_key = c;
          put_character(window, c);
        }
      }
    }
  }
}

void destroy_qedit_window(qedit_window* window) {
  dyn_list_destroy(window->lines);
  free(window);
}
