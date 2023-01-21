#include <stdlib.h>
#include <string.h>

#include "../include/dbutils.h"

PGconn *db_connect(const char *host, const char *dbname, const char *user, const char *password)
{
    char *conninfo = (char *)malloc(100);
    sprintf(conninfo, "host=%s dbname=%s user=%s password=%s", host, dbname, user, password);
    PGconn *conn = PQconnectdb(conninfo);
    free(conninfo);
    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return NULL;
    }
    return conn;
}

void db_disconnect(PGconn *conn)
{
    PQfinish(conn);
}

void *_loop_through_data(PGresult *result, void *(*callback)(PGresult *, int, int))
{
    if (PQresultStatus(result) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "No data retrieved\n");
        PQclear(result);
        return NULL;
    }
    int nFields = PQnfields(result);
    int nTuples = PQntuples(result);
    void **table = malloc(nTuples * sizeof(void *));

    for (int i = 0; i < nTuples; i++)
    {
        void *data = callback(result, i, nFields);
        table[i] = data;
    }
    return table;
}

bottle_t *create_bottle(PGresult *result, int row, int nbFields)
{
    bottle_t *bottle = (bottle_t *)malloc(sizeof(bottle_t));
    for (int i = 0; i < nbFields; i++)
    {
        if (strcmp(PQfname(result, i), "id") == 0)
            bottle->id = atoi(PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "quantity") == 0)
            bottle->quantity = atoi(PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "url") == 0)
            strcpy(bottle->url, PQgetvalue(result, row, i));
    }
    bottle->module = create_module(result, row, nbFields);
    return bottle;
}

module_t *create_module(PGresult *result, int row, int nbFields)
{
    module_t *module = (module_t *)malloc(sizeof(module_t));
    for (int i = 0; i < nbFields; i++)
    {
        if (strcmp(PQfname(result, i), "id") == 0)
            module->id = atoi(PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "ip_address") == 0)
            strcpy(module->ip_address, PQgetvalue(result, row, i));
    }
    return module;
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