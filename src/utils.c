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

void *check_executed_query(PGconn *conn, PGresult *result)
{
    if (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "Query failed: %s", PQerrorMessage(conn));
        return NULL;
    }
    return result;
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

int *check_positive(void *arg, types type)
{
    if (type == INT)
    {
        int *value = (int *)arg;
        if (*value < 0)
            return NULL;
    }
    else if (type == FLOAT)
    {
        float *value = (float *)arg;
        if (*value < 0)
            return NULL;
    }
    else if (type == DOUBLE)
    {
        double *value = (double *)arg;
        if (*value < 0)
            return NULL;
    }
    return (int *)arg;
}

int *check_url(url_t url)
{
    if (strncmp(url, "http://", 7) != 0 && strncmp(url, "https://", 8) != 0)
        return NULL;
    if (strchr(url, ' ') != NULL)
        return NULL;
    return (int *)url;
}

id_db_t _insert_data(PGconn *conn, char *query)
{
    PGresult *result = PQexec(conn, query);
    void *check = check_executed_query(conn, result);
    if (check == NULL)
        return NULL;
    id_db_t id = malloc(sizeof(id_db_t));
    *id = atoi(PQgetvalue(result, 0, 0));
    PQclear(result);
    return id;
}

void *_delete_data(PGconn *conn, char *query)
{
    PGresult *result = PQexec(conn, query);
    void *check = check_executed_query(conn, result);
    if (check == NULL)
        return NULL;
    PQclear(result);
    return result;
}

void *_update_data(PGconn *conn, char *query)
{
    PGresult *result = PQexec(conn, query);
    void *check = check_executed_query(conn, result);
    if (check == NULL)
        return NULL;
    PQclear(result);
    return result;
}

char *_concatenate_formated(char *dst, char *src, int *dst_length)
{
    *dst_length += strlen(src);
    dst = (char *)realloc(dst, *dst_length * sizeof(char));
    strcat(dst, src);
    return dst;
}


void format_string(char *target)
{
    char buffer[1024] = { 0 };
    char *insert_point = &buffer[0];
    const char *tmp = target;
    size_t needle_len = 1;
    size_t repl_len = 2;

    while (1) {
        const char *p = strstr(tmp, "'");

        if (p == NULL) {
            strcpy(insert_point, tmp);
            break;
        }

        memcpy(insert_point, tmp, p - tmp);
        insert_point += p - tmp;

        memcpy(insert_point, "''", repl_len);
        insert_point += repl_len;

        tmp = p + needle_len;
    }

    strcpy(target, buffer);
}

date_t create_date(char *date)
{
    date_t date_t;
    sscanf(date, "%d-%d-%d %d:%d", &date_t.year, &date_t.month, &date_t.day, &date_t.hour, &date_t.minute);
    return date_t;
}

void print_date(date_t date)
{
    printf("%d-%d-%d %d:%d", date.year, date.month, date.day, date.hour, date.minute);
}

int convert_bool(char car) {
    switch(car) {
        case 't':
            return 1;
            break;
        case 'f':
            return 0;
            break;
        default:
            return 2;
            break;
    }
}