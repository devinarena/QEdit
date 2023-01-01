
#include <stdlib.h>
#include <string.h>

#include "qstring.h"

qstring* qstring_new(const char* str) {
  qstring* qstr = malloc(sizeof(qstring));
  qstr->length = strlen(str);
  qstr->capacity = qstr->length * QSTRING_GROWTH_FACTOR;
  qstr->str = malloc(qstr->capacity + 1);
  strcpy(qstr->str, str);
  qstr->str[qstr->length] = '\0';
  return qstr;
}

void qstring_destroy(qstring* str) {
  free(str->str);
  free(str);
}

void qstring_insert(qstring* str, char c, int index) {
  if (index < 0 || index > str->length) {
    return;
  }
  if (str->length == str->capacity) {
    str->capacity *= QSTRING_GROWTH_FACTOR;
    str->str = realloc(str->str, str->capacity + 1);
  }
  for (int i = str->length; i > index; i--) {
    str->str[i] = str->str[i - 1];
  }
  str->str[index] = c;
  str->length++;
  str->str[str->length] = '\0';
}

void qstring_append(qstring* str, char c) {
  qstring_insert(str, c, str->length);
}

void qstring_concat(qstring* str, qstring* other, int free_other) {
  if (str->length - 2 + other->length >= str->capacity) {
    str->capacity = (str->length - 2 + other->length) * QSTRING_GROWTH_FACTOR;
    str->str = realloc(str->str, str->capacity + 1);
  }

  for (int i = 0; i < other->length; i++) {
    str->str[str->length - 2 + i] = other->str[i];
  }

  str->length += other->length - 2;
  str->str[str->length] = '\0';
  if (free_other) {
    qstring_destroy(other);
  }
}

void qstring_delete(qstring* str, int index) {
  if (index < 0 || index >= str->length) {
    return;
  }
  for (int i = index; i < str->length - 1; i++) {
    str->str[i] = str->str[i + 1];
  }
  str->length--;
  str->str[str->length] = '\0';
}