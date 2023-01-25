#include <stdlib.h>
#include <string.h>

#include "../include/dbutils.h"

bottle_t *create_bottle(PGresult *result, int row, int nbFields)
{
    bottle_t *bottle = (bottle_t *)malloc(sizeof(bottle_t));
    bottle->id = (int *)malloc(sizeof(int));
    for (int i = 0; i < nbFields; i++)
    {
        if (strcmp(PQfname(result, i), "id") == 0)
            *(bottle->id) = atoi(PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "quantity") == 0)
            bottle->quantity = atoi(PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "url") == 0)
            strcpy(bottle->url, PQgetvalue(result, row, i));
    }
    bottle->module = create_module(result, row, nbFields);
    return bottle;
}

bottle_t **get_bottles(PGconn *conn, int *length)
{
    PGresult *result = PQexec(conn, "SELECT * FROM bottles, modules WHERE bottles.id_module = modules.id");
    bottle_t **bottles = (bottle_t **)_loop_through_data(result, &create_bottle);
    *length = PQntuples(result);
    if (bottles == NULL)
        return NULL;
    PQclear(result);
    return bottles;
}

void _print_bottle(bottle_t *bottle)
{
    printf("Bottle[%d] {", *(bottle->id));
    printf("quantity: %f, ", bottle->quantity);
    printf("module: %d", *(bottle->module->id));
    printf("}\n");
}