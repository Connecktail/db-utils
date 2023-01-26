#ifndef DB_UTILS_H
#define DB_UTILS_H

#include <postgresql/libpq-fe.h>
#include "types.h"
#include "bottle.h"
#include "module.h"
#include "cocktail.h"
// #include "order.h"

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
 * @brief Check if the insertion was successful
 * @param conn A pointer to a PGconn object
 * @param result A pointer to a PGresult object
 * @return A pointer to NULL if the insertion failed
 */
void *check_insertion(PGconn *conn, PGresult *result);

/**
 * @brief Check if the ip address is valid
 * @param ip_address The ip address to check
 * @return A pointer to NULL if the ip address isn't valid
 */
int *check_ip_address(char *ip_address);

/**
 * @brief Check if the arg is positive
 * @param arg The arg to check
 * @param type The type of the argument
 * @return A pointer to NULL if the arg isn't valid
 */
int *check_positive(void *arg, types type);

/**
 * @brief Check if the url is valid
 * @param url The url to check
 * @return A pointer to NULL if the url isn't valid
 */
int *check_url(char *url);

/**
 * @brief Insert an object into the database, check the insertion and return id
 * @param conn A pointer to a PGconn object
 * @param query The query to execute
 * @return A pointer to NULL if the insertion failed otherwise a pointer to the id
 */
int *_insert_data(PGconn *conn, char *query);

#endif