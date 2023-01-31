#include <stdlib.h>
#include <string.h>

#include "../include/dbutils.h"

bottle_t *create_bottle(PGresult *result, int row, int nbFields)
{
    bottle_t *bottle = (bottle_t *)malloc(sizeof(bottle_t));
    bottle->id = (id_db_t)malloc(sizeof(id_db_t));
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
    long long int id = bottle->id == NULL ? -1 : *(bottle->id);
    long long int id_module = bottle->module == NULL ? -1 : *(bottle->module->id);
    printf("Bottle[%lld] {", id);
    printf("url: %s, ", bottle->url);
    printf("quantity: %f, ", bottle->quantity);
    printf("module: %lld", id_module);
    printf("}\n");
}

void insert_bottle(PGconn *conn, bottle_t *bottle)
{
    int *check = check_positive((void *)&(bottle->quantity), FLOAT);
    if (check == NULL)
    {
        fprintf(stderr, "Bottle quantity must be positive\n");
        return;
    }
    check = check_url(bottle->url);
    if (check == NULL)
    {
        fprintf(stderr, "Invalid URL\n");
        return;
    }

    char query[QUERY_LENGTH];
    if (bottle->module == NULL)
        sprintf(query, "INSERT INTO bottles (quantity, url) VALUES (%f, '%s') RETURNING id", bottle->quantity, bottle->url);
    else
        sprintf(query, "INSERT INTO bottles (quantity, url, id_module) VALUES (%f, '%s', %lld) RETURNING id", bottle->quantity, bottle->url, *(bottle->module->id));

    id_db_t id = _insert_data(conn, query);
    if (id == NULL)
    {
        fprintf(stderr, "Error while inserting bottle\n");
        return;
    }
    bottle->id = id;
}

void delete_bottle(PGconn *conn, id_db_t id)
{
    if (id == NULL)
    {
        fprintf(stderr, "Invalid id\n");
        return;
    }
    char query[QUERY_LENGTH];
    sprintf(query, "DELETE FROM bottles WHERE id = %lld", *id);
    _delete_data(conn, query);
}

void *update_bottle(PGconn *conn, bottle_t *bottle, url_t *new_url, float *new_quantity, id_db_t new_module_id)
{
    if (bottle == NULL)
    {
        fprintf(stderr, "Invalid bottle\n");
        return NULL;
    }

    if (new_url == NULL && new_quantity == NULL && new_module_id == NULL)
    {
        fprintf(stderr, "Nothing to update\n");
        return NULL;
    }

    char *query = (char *)calloc(20, sizeof(char));
    char query_add[255];
    sprintf(query_add, "UPDATE bottles SET");
    int query_length = strlen(query_add) + 1;
    query = (char *)realloc(query, query_length * sizeof(char));
    strcat(query, query_add);

    if (new_url != NULL)
    {
        int *check = check_url(*new_url);
        if (check == NULL)
        {
            fprintf(stderr, "Invalid URL\n");
            return NULL;
        }
        strcpy(query_add, "");
        sprintf(query_add, " url = '%s',", *new_url);
        query = _concatenate_formated(query, query_add, &query_length);
    }
    if (new_quantity != NULL)
    {
        int *check = check_positive((void *)new_quantity, FLOAT);
        if (check == NULL)
        {
            fprintf(stderr, "Bottle quantity must be positive\n");
            return NULL;
        }
        strcpy(query_add, "");
        sprintf(query_add, " quantity = %f,", *new_quantity);
        query = _concatenate_formated(query, query_add, &query_length);
    }
    if (new_module_id != NULL)
    {
        strcpy(query_add, "");
        sprintf(query_add, " id_module = %lld,", *new_module_id);
        query = _concatenate_formated(query, query_add, &query_length);
    }

    // Remove the comma from last edit
    for (int i = query_length - 1; i >= 0; i--)
    {
        if (query[i] == ',')
        {
            query[i] = '\0';
            break;
        }
    }

    strcpy(query_add, "");
    sprintf(query_add, " WHERE id = %lld", *(bottle->id));
    query = _concatenate_formated(query, query_add, &query_length);

    void *check = _update_data(conn, query);
    if (check != NULL)
    {
        strcpy(bottle->url, new_url == NULL ? bottle->url : *new_url);
        bottle->quantity = new_quantity == NULL ? bottle->quantity : *new_quantity;
        bottle->module = new_module_id == NULL ? bottle->module : *new_module_id;
    }
    else
    {
        printf("error on update\n");
    }

    return bottle;
}