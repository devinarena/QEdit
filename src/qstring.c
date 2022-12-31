
#include <stdlib.h>
#include <string.h>

#include "qstring.h"

qstring* qstring_new(const char* str) {
  qstring* qstr = malloc(sizeof(qstring));
  qstr->length = strlen(str);
  qstr->str = malloc(qstr->length);
  strcpy(qstr->str, str);
  return qstr;
}

void qstring_destroy(qstring* str) {
  free(str->str);
  free(str);
}

void insert_char(qstring* str, char c, int index) {
  if (index < 0 || index > str->length) {
    return;
  }
  str->str = realloc(str->str, str->length + 1);
  for (int i = str->length; i > index; i--) {
    str->str[i] = str->str[i - 1];
  }
  str->str[index] = c;
  str->length++;
}