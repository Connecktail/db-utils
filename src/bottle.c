#include <stdlib.h>
#include <string.h>

#include "../include/dbutils.h"

bottle_t *create_bottle(PGresult *result, int row, int nbFields)
{
    bottle_t *bottle = (bottle_t *)malloc(sizeof(bottle_t));
    bottle->id = (id_db_t)malloc(sizeof(id_db_t));
    for (int i = 0; i < nbFields; i++)
    {
        if (strcmp(PQfname(result, i), "id_b") == 0)
            *(bottle->id) = atoi(PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "name_b") == 0)
            strcpy(bottle->name, PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "quantity_b") == 0)
            bottle->quantity = atof(PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "url_b") == 0)
            strcpy(bottle->url, PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "price_b") == 0)
            bottle->price = atof(PQgetvalue(result, row, i));
    }
    bottle->module = create_module(result, row, nbFields);
    return bottle;
}

bottle_t **get_bottles(PGconn *conn, int *length)
{
    PGresult *result = PQexec(
        conn, 
        "SELECT b.id AS id_b, b.name AS name_b, b.quantity AS quantity_b, b.url AS url_b, b.price AS price_b, m.mac_address, m.ip_address FROM bottles AS b LEFT OUTER JOIN modules AS m ON b.id_module = m.mac_address ORDER BY id");
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
    mac_address_t id_module;
    strcpy(id_module, (bottle->module == NULL)?" ":(bottle->module->mac_address));
    printf("Bottle[%lld] {", id);
    printf("name: %s, ", bottle->name);
    printf("url: %s, ", bottle->url);
    printf("quantity: %f, ", bottle->quantity);
    printf("module: %s, ", id_module);
    printf("price: %f", bottle->price);
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

    char name[255];
    strcpy(name, bottle->name);
    format_string(name);
    char query[QUERY_LENGTH];
    if (bottle->module == NULL)
        sprintf(query, "INSERT INTO bottles (name, quantity, url, price) VALUES ('%s', %f, '%s', %f) RETURNING id", name, bottle->quantity, bottle->url, bottle->price);
    else{
        sprintf(query, "INSERT INTO bottles (name, quantity, url, id_module, price) VALUES ('%s', %f, '%s', '%s', %f) RETURNING id", name, bottle->quantity, bottle->url, (bottle->module->mac_address), bottle->price);
    }
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

void *update_bottle(PGconn *conn, bottle_t *bottle, char *new_name[255], url_t *new_url, float *new_quantity, module_t *new_module, float *new_price)
{
    if (bottle == NULL)
    {
        fprintf(stderr, "Invalid bottle\n");
        return NULL;
    }

    if (new_name == NULL && new_url == NULL && new_quantity == NULL && new_module == NULL && new_price == NULL)
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

    if (new_name != NULL)
    {
        strcpy(query_add, "");
        sprintf(query_add, " name = '%s',", new_name);
        query = _concatenate_formated(query, query_add, &query_length);
    }
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
    if (new_module != NULL)
    {   
        strcpy(query_add, "");
        sprintf(query_add, " id_module = '%s',", (new_module)->mac_address);
        query = _concatenate_formated(query, query_add, &query_length);
    }
    if (new_price != NULL)
    {
        int *check = check_positive((void *)new_price, FLOAT);
        if (check == NULL)
        {
            fprintf(stderr, "Bottle price must be positive\n");
            return NULL;
        }
        strcpy(query_add, "");
        sprintf(query_add, " price = %f,", *new_price);
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
        strcpy(bottle->name, new_name == NULL ? bottle->name : new_name);
        strcpy(bottle->url, new_url == NULL ? bottle->url : *new_url);
        bottle->quantity = new_quantity == NULL ? bottle->quantity : *new_quantity;
        bottle->module = new_module == NULL ? bottle->module : new_module;
        bottle->price = new_price == NULL ? bottle->price : *new_price;
    }
    else
    {
        printf("error on update\n");
    }

    return bottle;
}

bottle_t** get_non_associated_bottles(PGconn *conn, int *length)
{
    PGresult *result = PQexec(conn, "SELECT id as id_b,name as name_b,quantity as quantity_b, url as url_b, price as price_b FROM bottles WHERE id_module IS NULL");
    *length = PQntuples(result);
    return (bottle_t **)_loop_through_data(result, &create_bottle);
}