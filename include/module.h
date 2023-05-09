#ifndef MODULE_H
#define MODULE_H
#include <postgresql/libpq-fe.h>
#include "types.h"

typedef struct
{
    mac_address_t mac_address;
    ip_address_t ip_address;
} module_t;

/**
 * @brief Create a module base on the row of a PGresult object
 * @param result A pointer to a PGresult object
 * @param row The row to get the data from
 * @param nbFields The number of fields in the row
 * @return A pointer to a module_t object
 */
module_t *create_module(PGresult *result, int row, int nbFields);

/**
 * @brief Get all the modules from the database
 * @param conn A pointer to a PGconn object
 * @param length A pointer to an int that will contain the length of the array
 * @return A pointer to a Module array
 */
module_t **get_modules(PGconn *conn, int *length);

/**
 * @brief Print a module, used to debug
 * @param module A pointer to a module_t object
 */
void _print_module(module_t *module);

/**
 * @brief Insert a module in the database
 * @param conn A pointer to a PGconn object
 * @param module A pointer to a module_t object
 */
void insert_module(PGconn *conn, module_t *module);

/**
 * @brief Delete a module from the database
 * @param conn A pointer to a PGconn object
 * @param mac_adress The mac address of the module to delete
 */
void delete_module(PGconn *conn, mac_address_t mac_address);

/**
 * @brief Update a module in the database
 * @param conn A pointer to a PGconn object
 * @param module A pointer to a module_t object
 * @param new_ip_address The new ip address of the module
 * @return A pointer to NULL if the update failed
 */
void *update_module(PGconn *conn, module_t *module, ip_address_t new_ip_address);

/**
 * @brief Know if a module is associated to a bottle
 * @param conn A pointer to a PGconn object
 * @param module A pointer to a module_t object
 * @return 1 if the module is associated, 0 otherwise
 */
int is_associated(PGconn *conn, module_t *module);

#endif