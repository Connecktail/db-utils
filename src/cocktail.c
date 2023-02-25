#include <stdlib.h>
#include <string.h>

#include "../include/dbutils.h"

cocktail_t *create_cocktail(PGresult *result, int row, int nbFields)
{
    cocktail_t *cocktail = (cocktail_t *)malloc(sizeof(cocktail_t));
    cocktail->id = (id_db_t)malloc(sizeof(id_db_t));
    for (int i = 0; i < nbFields; i++)
    {
        if (strcmp(PQfname(result, i), "id") == 0)
            *(cocktail->id) = atoi(PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "name") == 0)
            strcpy(cocktail->name, PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "description") == 0)
            strcpy(cocktail->description, PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "price") == 0)
            cocktail->price = atof(PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "image_url") == 0)
            strcpy(cocktail->image_url, PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "personalized") == 0)
            cocktail->personalized = atoi(PQgetvalue(result, row, i));
    }
    return cocktail;
}

cocktail_t **get_cocktails(PGconn *conn, int *length)
{
    PGresult *result = PQexec(conn, "SELECT * FROM cocktails");

    cocktail_t **cocktails = (cocktail_t **)_loop_through_data(result, &create_cocktail);
    *length = PQntuples(result);
    if (cocktails == NULL)
        return NULL;
    PQclear(result);
    return cocktails;
}

void _print_cocktail(cocktail_t *cocktail)
{
    long long int id = cocktail->id == NULL ? -1 : *(cocktail->id);
    printf("Cocktail[%lld] {", id);
    printf("name: %s, ", cocktail->name);
    printf("description: %s, ", cocktail->description);
    printf("price: %f, ", cocktail->price);
    printf("image_url: %s", cocktail->image_url);
    printf("personalized: %s", cocktail->personalized ? "true" : "false");
    printf("}\n");
}

void insert_cocktail(PGconn *conn, cocktail_t *cocktail)
{
    int *check = check_positive((void *)&(cocktail->price), FLOAT);
    if (check == NULL)
    {
        fprintf(stderr, "Invalid price\n");
        return;
    }
    check = check_url(cocktail->image_url);
    if (check == NULL)
    {
        fprintf(stderr, "Invalid URL\n");
        return;
    }

    char query[QUERY_LENGTH];
    sprintf(query, "INSERT INTO cocktails (name, description, price, image_url, personalized) VALUES ('%s', '%s', %f, '%s', %s) RETURNING id", cocktail->name, cocktail->description, cocktail->price, cocktail->image_url, cocktail->personalized ? "TRUE" : "FALSE");
    id_db_t id = _insert_data(conn, query);
    if (id == NULL)
        return;
    cocktail->id = id;
}

void delete_cocktail(PGconn *conn, id_db_t id)
{
    if (id == NULL)
    {
        fprintf(stderr, "Invalid id\n");
        return;
    }
    char query[QUERY_LENGTH];
    sprintf(query, "DELETE FROM cocktails WHERE id = %lld", *id);
    _delete_data(conn, query);
}

void *update_cocktail(PGconn *conn, cocktail_t *cocktail, char *new_name[255], char *new_description[255], float *new_price, url_t *new_image_url)
{
    if (cocktail == NULL)
    {
        fprintf(stderr, "Invalid cocktail\n");
        return NULL;
    }

    if (new_name == NULL && new_description == NULL && new_price == NULL && new_image_url == NULL)
    {
        fprintf(stderr, "Nothing ot update\n");
        return NULL;
    }

    char *query = (char *)calloc(20, sizeof(char));
    char query_add[255];
    sprintf(query_add, "UPDATE cocktails SET");
    int query_length = strlen(query_add) + 1;
    query = (char *)realloc(query, query_length * sizeof(char));
    strcat(query, query_add);

    if (new_name != NULL)
    {
        strcpy(query_add, "");
        sprintf(&query_add, " name = '%s',", new_name);
        query = _concatenate_formated(query, query_add, &query_length);
    }

    if (new_description != NULL)
    {
        strcpy(query_add, "");
        sprintf(query_add, " description = '%s',", new_description);
        query = _concatenate_formated(query, query_add, &query_length);
    }

    if (new_price != NULL)
    {
        int *check = check_positive((void *)new_price, FLOAT);
        if (check == NULL)
        {
            fprintf(stderr, "Invalid price\n");
            return NULL;
        }
        strcpy(query_add, "");
        sprintf(query_add, " price = %f,", *new_price);
        query = _concatenate_formated(query, query_add, &query_length);
    }

    if (new_image_url != NULL)
    {
        int *check = check_url(*new_image_url);
        if (check == NULL)
        {
            fprintf(stderr, "Invalid URL\n");
            return NULL;
        }
        strcpy(query_add, "");
        sprintf(query_add, " image_url = '%s',", *new_image_url);
        query = _concatenate_formated(query, query_add, &query_length);
    }

    for (int i = query_length - 1; i >= 0; i--)
    {
        if (query[i] == ',')
        {
            query[i] = '\0';
            break;
        }
    }

    strcpy(query_add, "");
    sprintf(query_add, " WHERE id = %lld", *(cocktail->id));
    query = _concatenate_formated(query, query_add, &query_length);

    void *check = _update_data(conn, query);
    if (check != NULL)
    {
        if (new_name != NULL)
            strcpy(cocktail->name, new_name);
        if (new_description != NULL)
            strcpy(cocktail->description, new_description);
        if (new_price != NULL)
            cocktail->price = *new_price;
        if (new_image_url != NULL)
            strcpy(cocktail->image_url, *new_image_url);
    }
    else
    {
        printf("error on update\n");
    }

    return cocktail;
}