
#include <stdio.h>
#include <stdlib.h>

#include <conio.h>
#include <windows.h>

#include "qstring.h"
#include "qwindow.h"

#define INFO_LINES 1

// track last key
int last_key = 0;

static dyn_list* text_to_lines(const char* text) {
  dyn_list* lines = new_dyn_list(1, (void*)qstring_destroy);
  int i = 0;
  int start = 0;
  while (text[i] != '\0') {
    if (text[i] == '\n' || text[i + 1] == '\0') {
      uint32_t length = i - start - 1 > 0 ? i - start - 1 : 0;
      char* line = malloc(length + 1);
      memcpy(line, text + start, length);
      line[length] = '\0';
      qstring* qline = qstring_new(line);
      dyn_list_add(lines, qline);
      start = i + 1;
    }
    i++;
  }
  return lines;
}

qedit_window* new_qedit_window(const char* filename, const char* source) {
  qedit_window* window = malloc(sizeof(qedit_window));
  window->cx = 0;
  window->cy = 0;
  window->scroll_y = 0;
  window->lines = text_to_lines(source);
  window->line = 0;
  window->col = 0;
  window->filename = filename;
  window->original_source = source;

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
    if (col < window->lines->size - 1) {
      printf("\r\n");
    }
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

  if (window->col + window->width < line->length) {
    window->cy++;
    window->col += window->width;
  } else {
    if (window->line < window->lines->size - 1) {
      int rem = line->length - window->col;
      window->cy += rem / window->width + 1;
      window->line++;
      window->col = 0;
      window->cx = 0;
    }
  }
  set_cursor_pos(window);
  render_info(window);
}

static void move_cursor_left(qedit_window* window) {
  // qstring* line = get_current_line(window);

  if (window->col > 0) {
    window->col--;
    cursor_left(window);
  } else {
    if (window->line > 0) {
      window->line--;
      window->col = get_current_line(window)->length;
      window->cx = window->col % window->width;
      window->cy--;
    }
  }
  set_cursor_pos(window);
  render_info(window);
}

static void move_cursor_right(qedit_window* window) {
  qstring* line = get_current_line(window);

  if (window->col < line->length) {
    window->col++;
    cursor_right(window);
  } else {
    if (window->line + 1 < window->lines->size) {
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
    window->col--;
    qstring_delete(line, window->col);
    cursor_left(window);
    set_cursor_pos(window);
    printf("%s ", line->str + window->col);
    set_cursor_pos(window);
    render_info(window);
  } else {
    if (window->line > 0) {
      window->line--;
      qstring* prev_line = get_current_line(window);
      window->col = prev_line->length;
      window->cx = window->col % window->width;
      window->cy--;
      // only concat the next line if its not empty
      if (line->length != 0) {
        qstring_concat(prev_line, line, 0);
      }
      dyn_list_remove(window->lines, window->line + 1);
      rerender(window);
    }
  }
}

static void delete_key(qedit_window* window) {
  qstring* line = get_current_line(window);

  if (window->col < line->length) {
    qstring_delete(line, window->col);
    printf("%s ", line->str + window->col);
    set_cursor_pos(window);
    render_info(window);
  } else {
    if (window->line + 1 < window->lines->size) {
      qstring* next_line = dyn_list_get(window->lines, window->line + 1);
      // only concat the next line if its not empty
      if (next_line->length != 0) {
        qstring_concat(line, next_line, 0);
      }
      dyn_list_remove(window->lines, window->line + 1);
      rerender(window);
    }
  }
}

static void new_line(qedit_window* window) {
  qstring* line = get_current_line(window);
  qstring* new_line = qstring_new(line->str + window->col);
  line->length = window->col;
  line->str[line->length] = '\0';
  dyn_list_insert(window->lines, window->line + 1, new_line);
  window->line++;
  window->col = 0;
  window->cx = 0;
  window->cy++;
  rerender(window);
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
    case 71: {  // HOME
      rerender(window);
      break;
    }
    case 83: {  // DEL
      delete_key(window);
      break;
    }
  }
}

void start_listener(qedit_window* window) {
  int running = 1;
  while (running) {
    if (kbhit()) {
      char c = _getch();
      last_key = c;
      if (c == 'R' - 'R' + 1) {
        // ctrl + r - reloads the file

        continue;
      }
      if (c == 27) {
        running = 0;
      }
      if (c == '\b') {
        backspace(window);
        continue;
      }
      if (c == '\r') {
        new_line(window);
        continue;
      }
      if (c == -32 || c == 224) {
        c = _getch();
        last_key = c;
        special_key(window, c);
      } else {
        if (isalnum(c) || c == ' ') {
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
