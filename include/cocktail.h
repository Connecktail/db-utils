#ifndef COCKTAIL_H
#define COCKTAIL_H
#include <postgresql/libpq-fe.h>
#include "types.h"

typedef struct
{
    id_db_t id;
    char name[255];
    char description[255];
    float price;
    url_t image_url;
    bool personalized;
} cocktail_t;

/**
 * @brief Create a cocktail base on the row of a PGresult object
 * @param result A pointer to a PGresult object
 * @param row The row to get the data from
 * @param nbFields The number of fields in the row
 * @return A pointer to a cocktail_t object
 */
cocktail_t *create_cocktail(PGresult *result, int row, int nbFields);

/**
 * @brief Get all the cocktails from the database
 * @param conn A pointer to a PGconn object
 * @param length A pointer to an int that will contain the length of the array 
 * @return A pointer to a cocktail array
 */
cocktail_t **get_cocktails(PGconn *conn, int *length);

/**
 * @brief Get a cocktail from the database
 * @param conn A pointer to a PGconn object
 * @param id The id of the cocktail to get
 * @return A pointer to a cocktail_t object
*/
cocktail_t *get_cocktail_by_id(PGconn *conn, id_db_t id);

/**
 * @brief Get all the cocktails of an order from the database
 * @param conn A pointer to a PGconn object
 * @param length A pointer to an int that will contain the length of the array 
 * @param id_order The id of the order
 * @return A pointer to a cocktail array
 */
cocktail_t **get_order_cocktails(PGconn *conn, int *length, id_db_t id_order);

/**
 * @brief print a cocktail, used for debugging
 * @param cocktail A pointer to a cocktail_t object
 */
void _print_cocktail(cocktail_t *cocktail);

/**
 * @brief Insert a cocktail in the database
 * @param conn A pointer to a PGconn object
 * @param cocktail A pointer to a cocktail_t object
 */
void insert_cocktail(PGconn *conn, cocktail_t *cocktail);

/**
 * @brief Delete a cocktail from the database
 * @param conn A pointer to a PGconn object
 * @param id The id of the cocktail to delete
 */
void delete_cocktail(PGconn *conn, id_db_t id);

/**
 * @brief Update a cocktail in the database
 * @param conn A pointer to a PGconn object
 * @param cocktail A pointer to a cocktail_t object
 * @param new_name The new name of the cocktail
 * @param new_description The new description of the cocktail
 * @param new_price The new price of the cocktail
 * @param new_image_url The new image of the cocktail
 * @return A pointer to NULL if the update failed
 */
void *update_cocktail(PGconn *conn, cocktail_t *cocktail, char *new_name[255], char *new_description[255], float *new_price, url_t *new_image_url);

#endif