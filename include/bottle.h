#ifndef BOTTLE_H
#define BOTTLE_H

#include "module.h"

typedef struct
{
    int id;
    char url[255];
    float quantity;
    module_t *module;
} bottle_t;

#endif