
#include <stdio.h>
#include <stdlib.h>

#include <conio.h>
#include <windows.h>

#include "qstring.h"
#include "qwindow.h"

qedit_window* new_qedit_window() {
  qedit_window* window = malloc(sizeof(qedit_window));
  window->cx = 0;
  window->cy = 0;
  window->lines = new_dyn_list(1);

  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
  window->width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  window->height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

  return window;
}

void clear_window() {
  system("cls");
}

void rerender(qedit_window* window) {
  clear_window();
  for (int col = 0; col < window->height; col++) {
    if (col >= window->lines->size) {
      printf(":\n");
      continue;
    }
    qstring* line = dyn_list_get(window->lines, col);
    printf("%s", line->str);
  }
}

void destroy_qedit_window(qedit_window* window) {
  dyn_list_destroy(window->lines);
  free(window);
}
