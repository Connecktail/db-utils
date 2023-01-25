#include <stdlib.h>
#include <string.h>

#include "../include/dbutils.h"

module_t *create_module(PGresult *result, int row, int nbFields)
{
    module_t *module = (module_t *)malloc(sizeof(module_t));
    module->id = (int *)malloc(sizeof(int));
    for (int i = 0; i < nbFields; i++)
    {
        if (strcmp(PQfname(result, i), "id") == 0)
            *(module->id) = atoi(PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "ip_address") == 0)
            strcpy(module->ip_address, PQgetvalue(result, row, i));
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
    int id = module->id == NULL ? -1 : *(module->id);
    printf("Module[%d] {", id);
    printf("ip_address: %s", module->ip_address);
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

    char query[1024];
    sprintf(query, "INSERT INTO modules (ip_address) VALUES ('%s') RETURNING id", module->ip_address);
    PGresult *result = PQexec(conn, query);
    check_insertion(conn, result);
    module->id = (int *)malloc(sizeof(int));
    *(module->id) = atoi(PQgetvalue(result, 0, 0));
    PQclear(result);
}