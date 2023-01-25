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
    int id = bottle->id == NULL ? -1 : *(bottle->id);
    printf("Bottle[%d] {", id);
    printf("url: %s, ", bottle->url);
    printf("quantity: %f, ", bottle->quantity);
    printf("module: %d", *(bottle->module->id));
    printf("}\n");
}

void insert_bottle(PGconn *conn, bottle_t *bottle)
{
    char query[1024];
    sprintf(query, "INSERT INTO bottles (quantity, url, id_module) VALUES (%f, '%s', %d) RETURNING id", bottle->quantity, bottle->url, *(bottle->module->id));
    PGresult *result = PQexec(conn, query);
    check_insertion(conn, result);
    bottle->id = (int *)malloc(sizeof(int));
    *(bottle->id) = atoi(PQgetvalue(result, 0, 0));
    PQclear(result);
}