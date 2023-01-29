#ifndef TYPES_H
#define TYPES_H

typedef char url_t[255];
typedef char ip_address_t[255];
typedef int *id_db_t;

typedef enum
{
    INT,
    FLOAT,
    DOUBLE
} types;

#define QUERY_LENGTH 1024

#endif