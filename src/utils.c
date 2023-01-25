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

void check_insertion(PGconn *conn, PGresult *result)
{
    if (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "Insertion failed: %s", PQerrorMessage(conn));
        return;
    }
}

int *check_ip_address(char *ip_address)
{
    int *ip = malloc(4 * sizeof(int));
    char *ip_address_copy = malloc(strlen(ip_address) + 1);
    strcpy(ip_address_copy, ip_address);
    char *token = strtok(ip_address_copy, ".");
    int i = 0;
    while (token != NULL)
    {
        ip[i] = atoi(token);
        if (ip[i] < 0 || ip[i] > 255)
            return NULL;
        token = strtok(NULL, ".");
        i++;
    }
    if (i != 4)
        return NULL;
    return ip;
}
