
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dyn_list.h"
#include "qstring.h"
#include "qwindow.h"

/**
 * @brief Edit text files very quickly in the terminal. Incredibly basic and
 * easy to learn editor for making fast edits.
 * @author Devin Arena
 */

/**
 * @brief Reads the contents of a file and returns it as a null terminated
 * C-string.
 *
 * @param path the path to the file to read
 * @return const char* the contents of the file
 */
const char* readFile(const char* path) {
  FILE* file = fopen(path, "rb");
  if (file == NULL) {
    printf("Error: Could not open file");
    return NULL;
  }

  // Get the size of the file
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);

  // Allocate memory for the file contents
  char* contents = malloc(size + 1);
  if (contents == NULL) {
    printf("Error: Could not allocate memory for file contents");
    return NULL;
  }

  // Read the file contents
  fread(contents, 1, size, file);
  fclose(file);
  contents[size] = '\0';

  return contents;
}

int main(int argc, const char* argv[]) {
  // Check if the user provided a file to edit
  if (argc < 2) {
    printf("Usage: %s <file>", argv[0]);
    return 1;
  }

  const char* source = readFile(argv[1]);
  if (source == NULL) {
    return 1;
  }

  qedit_window* window = new_qedit_window(argv[1], source);

  free((void*)source);

  rerender(window);
  set_cursor_pos(window);

  start_listener(window);

  destroy_qedit_window(window);

  return 0;
}