
#include <stdlib.h>
#include <string.h>

#include "qstring.h"

qstring* qstring_new(const char* str) {
    qstring* qstr = malloc(sizeof(qstring));
    qstr->str = malloc(strlen(str));
    strcpy(qstr->str, str);
    qstr->length = strlen(str);
    return qstr;
}