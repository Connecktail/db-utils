#ifndef BOTTLE_H
#define BOTTLE_H

#include <postgresql/libpq-fe.h>
#include "module.h"

typedef struct
{
    id_db_t id;
    url_t url;
    float quantity;
    module_t *module;
} bottle_t;

/**
 * @brief Get all the bottles from the database
 * @param conn A pointer to a PGconn object
 * @param length A pointer to an int that will contain the length of the array
 * @return A pointer to a Bottle array
 */
bottle_t **get_bottles(PGconn *conn, int *length);

/**
 * @brief Create a bottle based on the row of a PGresult object
 * @param result A pointer to a PGresult object
 * @param row The row to get the data from
 * @param nbFields The number of fields in the row
 * @return A pointer to a bottle_t object
 */
bottle_t *create_bottle(PGresult *result, int row, int nbFields);

/**
 * @brief Print a bottle, used to debug
 * @param bottle A pointer to a bottle_t object
 */
void _print_bottle(bottle_t *bottle);

/**
 * @brief Insert a bottle in the database
 * @param conn A pointer to a PGconn object
 * @param module A pointer to a bottel_t object
 * @remark The bottle must have a module whereas insertion won't work
 */
void insert_bottle(PGconn *conn, bottle_t *bottle);

#endif