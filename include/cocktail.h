#ifndef COCKTAIL_H
#define COCKTAIL_H
#include <postgresql/libpq-fe.h>
#include "types.h"

typedef struct
{
    id_db_t id;
    float price;
    url_t image;
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

#endif