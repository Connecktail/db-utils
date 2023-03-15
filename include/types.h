#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h> 

typedef char url_t[255];
typedef char mac_address_t[255];
typedef char ip_address_t[255];
typedef long long int *id_db_t;

typedef enum
{
    INT,
    FLOAT,
    DOUBLE
} types;

typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int minute;
} date_t;

#define QUERY_LENGTH 1024

#endif