
#include <stdint.h>

#ifndef QEDIT_QSTRING_H
#define QEDIT_QSTRING_H

#define QSTRING_GROWTH_FACTOR 2

/**
 * @brief Special types of strings used in qedit for data storage.
 */

typedef struct {
    char* str;
    uint32_t length;
    uint32_t capacity;
} qstring;

qstring* qstring_new(const char* str);
void qstring_insert(qstring* str, char c, int index);
void qstring_destroy(qstring* str);

#endif