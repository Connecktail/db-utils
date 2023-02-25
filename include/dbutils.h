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
 * @brief Check if the query was successfully executed
 * @param conn A pointer to a PGconn object
 * @param result A pointer to a PGresult object
 * @return A pointer to NULL if the execution failed
 */
void *check_executed_query(PGconn *conn, PGresult *result);

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
int *check_url(url_t url);

/**
 * @brief Insert an object into the database, check the insertion and return id
 * @param conn A pointer to a PGconn object
 * @param query The query to execute
 * @return A pointer to NULL if the insertion failed otherwise a pointer to the id
 */
id_db_t _insert_data(PGconn *conn, char *query);

/**
 * @brief Delete an object from the database
 * @param conn A pointer to a PGconn object
 * @param query The query to execute
 * @return A pointer to NULL if the deletion failed
 */
void *_delete_data(PGconn *conn, char *query);

/**
 * @brief Update an object in the database
 * @param conn A pointer to a PGconn object
 * @param query The query to execute
 * @return A pointer to NULL if the update failed
 */
void *_update_data(PGconn *conn, char *query);

/**
 * @brief concatenate a formated string passed in param
 * @param dst The string that will be concatenated
 * @param src The string that will be appended to dst
 * @param dst_length length of the string that will be concatenated
 * @return A pointer to the concatenated string
 */
char *_concatenate_formated(char *dst, char *src, int *dst_length);

/**
 * @brief format a string to be used in a sql string query
 * @param target The string to format, the string will be modified
*/
void format_string(char *target);

#endif