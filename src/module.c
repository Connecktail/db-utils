#include <stdlib.h>
#include <string.h>

#include "../include/dbutils.h"

module_t *create_module(PGresult *result, int row, int nbFields)
{
    module_t *module = (module_t *)malloc(sizeof(module_t));
    for (int i = 0; i < nbFields; i++)
    {
        if (strcmp(PQfname(result, i), "mac_address") == 0)
            strcpy(module->mac_address, PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "ip_address") == 0)
            strcpy(module->ip_address, PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "battery_level") == 0)
            module->battery_level = atoi(PQgetvalue(result, row, i));
    }
    return module;
}

module_t **get_modules(PGconn *conn, int *length)
{
    PGresult *result = PQexec(conn, "SELECT * FROM modules");

    module_t **modules = (module_t **)_loop_through_data(result, &create_module);
    *length = PQntuples(result);
    if (modules == NULL)
        return NULL;
    PQclear(result);
    return modules;
}

void _print_module(module_t *module)
{
    mac_address_t mac_address;
    strcpy(mac_address, module->mac_address == NULL ? "NULL" : module->mac_address);

    printf("Module[%s] {", mac_address);
    printf("ip_address: %s", module->ip_address);
    printf(", battery_level: %d", module->battery_level);
    printf("}\n");
}

void insert_module(PGconn *conn, module_t *module)
{
    int *check = check_ip_address(module->ip_address);
    if (check == NULL)
    {
        fprintf(stderr, "Invalid IP address\n");
        return;
    }
    char query[QUERY_LENGTH];
    // By default, the battery_level field is set to 100
    sprintf(query, "INSERT INTO modules (mac_address, ip_address, battery_level) VALUES ('%s','%s', 100) RETURNING mac_address", module->mac_address, module->ip_address);

    mac_address_t mac_address;
    strcpy(mac_address, _insert_data(conn, query));
    if (mac_address == NULL)
        return;
    strcpy(module->mac_address, mac_address);
}

void delete_module(PGconn *conn, mac_address_t mac_address)
{
    if (mac_address == NULL)
    {
        fprintf(stderr, "Invalid mac_address\n");
        return;
    }
    char query[QUERY_LENGTH];
    sprintf(query, "DELETE FROM modules WHERE id = '%s'", mac_address);
    _delete_data(conn, query);
}

void *update_module(PGconn *conn, module_t *module, ip_address_t new_ip_address, int *new_battery_level)
{
    if (module->mac_address == NULL)
    {
        fprintf(stderr, "Module is not created\n");
        return NULL;
    }
    if (new_ip_address != NULL)
    {
        void *check = check_ip_address(new_ip_address);
        if (check == NULL)
        {
            fprintf(stderr, "Invalid IP address\n");
            return NULL;
        }
        char query[QUERY_LENGTH];
        if (new_battery_level != NULL)
            sprintf(query, "UPDATE modules SET ip_address = '%s', battery_level = %d WHERE mac_address = '%s'", new_ip_address, *new_battery_level, module->mac_address);
        else
            sprintf(query, "UPDATE modules SET ip_address = '%s' WHERE mac_address = '%s'", new_ip_address, module->mac_address);
        check = _update_data(conn, query);
        if (check != NULL)
            strcpy(module->ip_address, new_ip_address);
    }
    else
    {
        fprintf(stderr, "IP address not provided\n");
        return NULL;
    }
    return module;
}

int is_associated(PGconn *conn, module_t *module)
{
    char query[QUERY_LENGTH];
    sprintf(query, "SELECT EXISTS(SELECT * FROM bottles WHERE id_module = '%s')", module->mac_address);
    PGresult *res = PQexec(conn, query);
    return convert_bool(PQgetvalue(res, 0, 0)[0]);
}

void dissociate_module(PGconn *conn, module_t *module)
{
    if (is_associated(conn, module) == 0)
    {
        fprintf(stderr, "Module is not associated\n");
        return;
    }
    char query[QUERY_LENGTH];
    sprintf(query, "UPDATE bottles SET id_module=Null WHERE id_module='%s'", module->mac_address);
    _update_data(conn, query);
}