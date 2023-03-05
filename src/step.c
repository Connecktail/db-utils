#include <stdlib.h>
#include <string.h>

#include "../include/dbutils.h"

step_t *create_step(PGresult *result, int row, int nbFields)
{
    step_t *step = (step_t *)malloc(sizeof(step_t));
    step->id = (id_db_t)malloc(sizeof(id_db_t));
    step->id_cocktail = (id_db_t)malloc(sizeof(id_db_t));
    for (int i = 0; i < nbFields; i++)
    {
        if (strcmp(PQfname(result, i), "id_s") == 0)
            *(step->id) = atoi(PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "quantity_s") == 0)
            step->quantity = atof(PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "description_s") == 0)
            strcpy(step->description, PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "id_cocktail_s") == 0)
            *(step->id_cocktail) = (long long int)atoi(PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "message") == 0)
            strcpy(step->message, PQgetvalue(result, row, i));
    }
    step->bottle = create_bottle(result, row, nbFields);
    return step;
}

step_t **get_cocktail_steps(PGconn *conn, int *length, id_db_t id_cocktail) {
    char query[QUERY_LENGTH];
    sprintf(query, "SELECT s.id AS id_s, s.quantity AS quantity_s, s.description AS description_s, s.id_cocktail AS id_cocktail_s, s.message AS message_s, b.id AS id_b, b.name AS name_b, b.quantity AS quantity_b, b.url AS url_b, b.price AS price_b FROM steps AS s, bottles AS b WHERE s.id_bottle = b.id AND s.id_cocktail = %lld ORDER BY s.id", *id_cocktail);
    PGresult *result = PQexec(conn, query);
    step_t **steps = (step_t **)_loop_through_data(result, &create_step);
    *length = PQntuples(result);
    if (steps == NULL)
        return NULL;
    PQclear(result);
    return steps;
}

void _print_step(step_t *step)
{
    long long int id = step->id == NULL ? -1 : *(step->id);
    printf("Step[%lld] {", id);
    printf("quantity: %f, ", step->quantity);
    printf("description: %s, ", step->description);
    printf("message: %s, ", step->message);
    printf("id_cocktail: %lld, ", *(step->id_cocktail));
    printf("id_bottle: %lld", *(step->bottle->id));
    printf("}\n");
}

void insert_step(PGconn *conn, step_t *step)
{
    int *check = check_positive((void *)&(step->quantity), FLOAT);
    if (check == NULL)
    {
        fprintf(stderr, "Bottle quantity must be positive\n");
        return;
    }

    char description[255];
    strcpy(description, step->description);
    format_string(description);
    char message[255] = "";
    strcpy(message, step->message);
    format_string(message);
    char query[QUERY_LENGTH];
    sprintf(query, "INSERT INTO steps (quantity, description, id_cocktail, id_bottle, message) VALUES (%f, '%s', %lld, %lld, '%s') RETURNING id", step->quantity, description, *step->id_cocktail, *step->bottle->id, (strlen(message)!=0?message:""));
   
    id_db_t id = _insert_data(conn, query);
    if (id == NULL)
    {
        fprintf(stderr, "Error while inserting step\n");
        return;
    }
    step->id = id;
}

void delete_step(PGconn *conn, id_db_t id)
{
    if (id == NULL)
    {
        fprintf(stderr, "Invalid id\n");
        return;
    }
    char query[QUERY_LENGTH];
    sprintf(query, "DELETE FROM steps WHERE id = %lld", *id);
    _delete_data(conn, query);
}

void *update_step(PGconn *conn, step_t *step, float *new_quantity, char *new_desc[255], bottle_t *new_bottle, char *new_message[255])
{
    if (step == NULL)
    {
        fprintf(stderr, "Invalid step\n");
        return NULL;
    }

    if (new_quantity == NULL && new_desc == NULL && new_bottle == NULL && new_message == NULL)
    {
        fprintf(stderr, "Nothing to update\n");
        return NULL;
    }

    char *query = (char *)calloc(20, sizeof(char));
    char query_add[255];
    sprintf(query_add, "UPDATE steps SET");
    int query_length = strlen(query_add) + 1;
    query = (char *)realloc(query, query_length * sizeof(char));
    strcat(query, query_add);

    if (new_quantity != NULL)
    {
        int *check = check_positive((void *)new_quantity, FLOAT);
        if (check == NULL)
        {
            fprintf(stderr, "Step quantity must be positive\n");
            return NULL;
        }
        strcpy(query_add, "");
        sprintf(query_add, " quantity = %f,", *new_quantity);
        query = _concatenate_formated(query, query_add, &query_length);
    }
    if (new_desc != NULL)
    {
        strcpy(query_add, "");
        sprintf(query_add, " description = '%s',", new_desc);
        query = _concatenate_formated(query, query_add, &query_length);
    }
    if (new_bottle != NULL)
    {   
        strcpy(query_add, "");
        sprintf(query_add, " id_bottle = %lld,", *(new_bottle)->id);
        query = _concatenate_formated(query, query_add, &query_length);
    }
    if (new_message != NULL)
    {
        strcpy(query_add, "");
        sprintf(query_add, " message = '%s',", new_message);
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
    sprintf(query_add, " WHERE id = %lld", *(step->id));
    query = _concatenate_formated(query, query_add, &query_length);

    void *check = _update_data(conn, query);
    if (check != NULL)
    {
        step->quantity = new_quantity == NULL ? step->quantity : *new_quantity;
        strcpy(step->description, new_desc == NULL ? step->description : new_desc);
        step->bottle = new_bottle == NULL ? step->bottle : new_bottle;
        strcpy(step->message, new_message == NULL ? step->message : new_message);
    }
    else
    {
        printf("error on update\n");
    }

    return step;
}