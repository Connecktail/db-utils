#ifndef STEP_H
#define STEP_H

#include <postgresql/libpq-fe.h>
#include "module.h"

typedef struct
{
    id_db_t id;
    float quantity;
    char description[255];
    id_db_t id_cocktail;
    bottle_t *bottle;
    char message[255];
} step_t;

/**
 * @brief Get all the steps of a cocktail from the database
 * @param conn A pointer to a PGconn object
 * @param length A pointer to an int that will contain the length of the array
 * @param id_cocktail The id of the cocktail to get the steps from
 * @return A pointer to a Step array
 */
step_t **get_cocktail_steps(PGconn *conn, int *length, id_db_t id_cocktail);

/**
 * @brief Create a bottle based on the row of a PGresult object
 * @param result A pointer to a PGresult object
 * @param row The row to get the data from
 * @param nbFields The number of fields in the row
 * @return A pointer to a bottle_t object
 */
step_t *create_step(PGresult *result, int row, int nbFields);

/**
 * @brief Print a bottle, used to debug
 * @param bottle A pointer to a bottle_t object
 */
void _print_step(step_t *step);

/**
 * @brief Insert a step in the database
 * @param conn A pointer to a PGconn object
 * @param step A pointer to a step_t object
 * @remark The step must have a bottle and a cocktail which it referes to whereas insertion won't work
 */
void insert_step(PGconn *conn, step_t *step);

/**
 * @brief Delete a step from the database
 * @param conn A pointer to a PGconn object
 * @param id The id of the step to delete
 */
void delete_step(PGconn *conn, id_db_t id);

/**
 * @brief Update a bottle in the database
 * @param conn A pointer to a PGconn object
 * @param module A pointer to a module_t object
 * @param new_name The new name of the bottle
 * @param new_url The new url of the bottle
 * @param new_quantity The new quantity of the bottle
 * @param new_module The new module of the bottle
 * @param new_price The new price of the bottle
 * @return A pointer to NULL if the update failed
 */
void *update_step(PGconn *conn, step_t *step, float *new_quantity, char *new_desc[255], bottle_t *new_bottle, char *new_message[255]);

#endif