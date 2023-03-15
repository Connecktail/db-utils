#ifndef ORDER_H
#define ORDER_H

#include <postgresql/libpq-fe.h>

typedef struct
{
    id_db_t id;
    date_t date;
    float price;
    int status; // 0 = false in db, 1 = true in db, 2 = NULL in db
    int nb_cocktails;
    cocktail_t **cocktails;
} order_t;

/**
 * @brief Get an order from the database
 * @param conn A pointer to a PGconn object
 * @param id The id of the order to get
 * @return A pointer to an order_t object
*/
order_t *get(PGconn *conn, id_db_t id);

/**
 * @brief Get all the orders from the database without the cocktails dependencies
 * @param conn A pointer to a PGconn object
 * @param length A pointer to an int that will contain the length of the array
 * @return A pointer to an order array
*/
order_t **get_orders(PGconn *conn, int *length);

/**
 * @brief Create an order based on the row of a PGresult object
 * @param result A pointer to a PGresult object
 * @param row The row to get the data from
 * @param nbFields The number of fields in the row
 * @return A pointer to a order_t object
 */
order_t *create_order(PGresult *result, int row, int nbFields);

/**
 * @brief Print an order, used to debug
 * @param order A pointer to a order_t object
 */
void _print_order(order_t *order);

/**
 * @brief Insert an order in the database and the cocktails dependencies in the cocktails_orders table, and insert the cocktails in the cocktails table if they are personalized
 * @param conn A pointer to a PGconn object
 * @param order A pointer to a order_t object
 * @note The nb_cocktails field of the order must be set and of the right value so the cocktails array can be iterated
 */
void insert_order(PGconn *conn, order_t *order);

/**
 * @brief Delete an order from the database
 * @param conn A pointer to a PGconn object
 * @param id The id of the order to delete
 */
void delete_order(PGconn *conn, id_db_t id);

/**
 * @brief Update an order in the database
 * @param conn A pointer to a PGconn object
 * @param order A pointer to a order_t object
 * @param new_price The new price of the order
 * @param new_status The new status of the order -> 0 for false, 1 for true, 2 for NULL
 * @return A pointer to NULL if the update failed
 */
void *update_order(PGconn *conn, order_t *order, float *new_price, int *new_status);

#endif