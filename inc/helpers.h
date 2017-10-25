#ifndef HELPERS_H
#define HELPERS_H

#include <stddef.h>

#define cnt_of_array(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

#endif