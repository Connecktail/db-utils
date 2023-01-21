#ifndef DB_UTILS_H
#define DB_UTILS_H

#include <postgresql/libpq-fe.h>
#include "bottle.h"
#include "module.h"
#include "order.h"
#include "cocktail.h"

/**
 * @brief Connect to a database
 * @param host The host to connect to
 * @param dbname The database name
 * @param user username
 * @param password password of the user chosen
 * @return A pointer to a PGconn object
 */
PGconn *db_connect(const char *host, const char *dbname, const char *user, const char *password);

/**
 * @brief Disconnect from a database
 * @param conn A pointer to a PGconn object
 */
void db_disconnect(PGconn *conn);

/**
 * @brief Loop through the data returned by a query
 * @param result A pointer to a PGresult object
 * @param callback A function pointer to a function that takes a char * as argument
 */
void *_loop_through_data(PGresult *result, void *(*callback)(PGresult *, int, int));

/**
 * @brief Get all the bottles from the database
 * @param conn A pointer to a PGconn object
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
 * @brief Create a module base on the row of a PGresult object
 * @param result A pointer to a PGresult object
 * @param row The row to get the data from
 * @param nbFields The number of fields in the row
 * @return A pointer to a module_t object
 */
module_t *create_module(PGresult *result, int row, int nbFields);

#endif