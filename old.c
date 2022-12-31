
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <conio.h>
#include <windows.h>

/**
 * @brief Edit text files very quickly in the terminal. Incredibly basic and
 * easy to learn editor for making fast edits.
 * @author Devin Arena
 */

#define MAX_LINE_LENGTH 30

typedef struct {
  char* line;
  int length;  // does not include null terminator
  int start;   // start of line in file
} Line;

typedef struct Editor {
  int line;
  int col;
  int width;
  int height;
  int cursorX;
  int cursorY;
  Line* lines;
  int lineCount;
  const char* fileName;
} Editor;

Editor editor;

static void initEditor(const char* fileName) {
  // Get windows window size
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  int columns, rows;

  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
  columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

  editor.line = 0;
  editor.col = 0;
  editor.lineCount = 8;
  editor.lines = malloc(sizeof(Line) * editor.lineCount);
  editor.fileName = fileName;
  editor.width = columns;
  editor.height = rows;
  editor.cursorX = 0;
  editor.cursorY = 0;
}

/**
 * @brief Gets the current line in the editor.
 *
 * @return Line the current line
 */
static Line currentLine() {
  return editor.lines[editor.line];
}

/**
 * @brief Gets the previous line in the editor.
 */
static Line previousLine() {
  return editor.lines[editor.line - 1];
}

/**
 * @brief Gets the next line in the editor.
 */
static Line nextLine() {
  return editor.lines[editor.line + 1];
}

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

/**
 * @brief Clears the screen.
 */
static void clearScreen() {
  system("cls");
}

/**
 * @brief Helper for moving the cursor to a position.
 *
 * @param x the console x position
 * @param y the console y position
 */
static void gotoXY(int x, int y) {
  COORD coord;
  coord.X = x;
  coord.Y = y;
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

  editor.cursorX = x;
  editor.cursorY = y;
}

/**
 * @brief Helper for moving to a row and column (with proper bounds checking).
 *
 * @param row the row to move to
 * @param col the column to move to
 */
static void gotoRC(int row, int col) {
  gotoXY(col, row);
}

/**
 * @brief Loads the lines of the file into Line structs, stored in the editor.
 *
 * @param source the source string (split by '\n')
 */
static void loadLines(const char* source) {
  // split by newlines
  char* dupe = strdup(source);
  char* line = strtok(dupe, "\n");
  int lineCount = 0;
  int totalLen = 0;

  while (line != NULL) {
    if (lineCount >= editor.lineCount) {
      editor.lineCount *= 2;
      editor.lines = realloc(editor.lines, editor.lineCount * sizeof(Line));
    }
    int len = max(strlen(line) - 1, 0);
    // copy the line into the lines array
    Line qLine = {line, len, totalLen};
    editor.lines[lineCount++] = qLine;
    line = strtok(NULL, "\n");
    totalLen += (1 + floor((double)len / editor.width)) * editor.width;
  }

  // last line needs one space of padding
  editor.lines[lineCount - 1].length++;

  // initialize the rest of the lines to empty
  for (size_t i = lineCount; i < editor.lineCount; i++) {
    Line qLine = {"", -1};
    editor.lines[i] = qLine;
  }
}

void insertChar(char c) {
  
}

/**
 * @brief Starts the text editors main loop.
 */
static void open() {
  clearScreen();
  for (int i = 0; i < editor.lineCount; i++) {
    if (editor.lines[i].length > -1)
      printf("%s\n", editor.lines[i].line, editor.lines[i].length);
  }

  gotoXY(editor.cursorX, editor.cursorY);

  // Note: only works for windows, when I boot into arch I should fix this.
  while (1) {
    int c = getch();
    switch (c) {  // A-D for arrow keys
      case 72:
        if (editor.line > 0) {
          // if the current line wraps, move to the end of the previous line
          Line prev = previousLine();
          Line current = currentLine();
          editor.line--;
          editor.col = min(editor.col, prev.length);
          gotoRC(prev.start / editor.width, editor.col);
        }
        break;
      case 80:
        if (editor.line < editor.lineCount - 1) {
          Line next = nextLine();
          if (next.length > -1) {
            Line current = currentLine();
            editor.line++;
            editor.col = min(editor.col, next.length);
            gotoRC(next.start / editor.width, editor.col);
          }
        }
        break;
      case 75: {
        Line current = currentLine();
        if (editor.col > 0) {
          editor.col--;
          gotoRC(current.start / editor.width +
                     (editor.col / (current.length + 1)),
                 editor.col % editor.width);
        }
        break;
      }
      case 77:
        Line current = currentLine();
        if (editor.col < current.length) {
          editor.col++;
          if (editor.col == current.length) {
            gotoRC((current.start + current.length) / editor.width,
                   editor.col % editor.width);
          } else
            gotoRC((current.start + editor.col) / editor.width,
                   editor.col % editor.width);
        }
        break;
      case 27:
        clearScreen();
        exit(1);
        return;
      default:
        insertChar((char)c);
        break;
    }
  }
}

int main(int argc, const char* argv[]) {
  // Check if the user provided a file to edit
  if (argc < 2) {
    printf("Usage: %s <file>", argv[0]);
    return 1;
  }

  const char* source = readFile(argv[1]);

  if (!source)
    return 1;

  initEditor(argv[1]);

  loadLines(source);

  free((void*)source);

  open();

  return 0;
}