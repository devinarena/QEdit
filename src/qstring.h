
#ifndef QEDIT_QSTRING_H
#define QEDIT_QSTRING_H

typedef struct {
    char* str;
    int length;
} qstring;

qstring* qstring_new(const char* str);

#endif