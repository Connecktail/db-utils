#include <stdlib.h>
#include <string.h>

#include "../include/dbutils.h"

cocktail_t *create_cocktail(PGresult *result, int row, int nbFields)
{
    cocktail_t *cocktail = (cocktail_t *)malloc(sizeof(cocktail_t));
    cocktail->id = (int *)malloc(sizeof(int));
    for (int i = 0; i < nbFields; i++)
    {
        if (strcmp(PQfname(result, i), "id") == 0)
            *(cocktail->id) = atoi(PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "price") == 0)
            cocktail->price = atof(PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "image") == 0)
            strcpy(cocktail->image, PQgetvalue(result, row, i));
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
    printf("Cocktail[%d] {", *(cocktail->id));
    printf("price: %f, ", cocktail->price);
    printf("image: %s", cocktail->image);
    printf("}\n");
}