#include <stdlib.h>
#include <string.h>

#include "../include/dbutils.h"

module_t *create_module(PGresult *result, int row, int nbFields)
{
    module_t *module = (module_t *)malloc(sizeof(module_t));
    module->id = (id_db_t)malloc(sizeof(id_db_t));
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
    long long int id = module->id == NULL ? -1 : *(module->id);
    printf("Module[%lld] {", id);
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
    char query[QUERY_LENGTH];
    sprintf(query, "INSERT INTO modules (id, ip_address) VALUES ('%lld','%s') RETURNING id", *(module->id), module->ip_address);

    id_db_t id = _insert_data(conn, query);
    if (id == NULL)
        return;
    module->id = id;
}

void delete_module(PGconn *conn, id_db_t id)
{
    if (id == NULL)
    {
        fprintf(stderr, "Invalid id\n");
        return;
    }
    char query[QUERY_LENGTH];
    sprintf(query, "DELETE FROM modules WHERE id = %lld", *id);
    _delete_data(conn, query);
}

void *update_module(PGconn *conn, module_t *module, ip_address_t new_ip_address)
{
    if (module->id == NULL)
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
        sprintf(query, "UPDATE modules SET ip_address = '%s' WHERE id = %lld", new_ip_address, *(module->id));
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