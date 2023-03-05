#include <stdlib.h>
#include <string.h>

#include "../include/dbutils.h"

order_t *create_order(PGresult *result, int row, int nbFields)
{
    order_t *order = (order_t *)malloc(sizeof(order_t));
    order->id = (id_db_t)malloc(sizeof(id_db_t));
    for (int i = 0; i < nbFields; i++)
    {
        if (strcmp(PQfname(result, i), "id") == 0)
            *(order->id) = atoi(PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "date") == 0)
            order->date = create_date(PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "price") == 0)
            order->price = atof(PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "nb_cocktails") == 0)
            order->nb_cocktails = atoi(PQgetvalue(result, row, i));
        if (strcmp(PQfname(result, i), "status") == 0)
            order->status = atoi(PQgetvalue(result, row, i));
    }

    return order;
}

order_t **get_orders(PGconn *conn, int *length)
{
    PGresult *result = PQexec(
        conn, 
        "SELECT o.*, COUNT(c_o.id) as nb_cocktails FROM orders AS o, cocktails_orders AS c_o WHERE o.id = c_o.id_order GROUP BY o.id");
    order_t **orders = (order_t **)_loop_through_data(result, &create_order);
    *length = PQntuples(result);
    if (orders == NULL)
        return NULL;
    PQclear(result);
    for(int i = 0; i < *length; i++) {
        orders[i]->cocktails = get_order_cocktails(conn, &orders[i]->nb_cocktails, orders[i]->id);
    }
    return orders;
}

void _print_order(order_t *order)
{
    long long int id = order->id == NULL ? -1 : *(order->id);
    printf("Order[%lld] {", id);
    printf("date: ");
    print_date(order->date);
    printf(", ");
    printf("price: %f, ", order->price);
    printf("status: %s, ", ((order->status==0) ? "waiting to be taken" : ((order->status==1) ? "active" : "finished")));
    printf("nb cocktails: %d, ", order->nb_cocktails);
    printf("cocktails: [\n");
    for (int i = 0; i < order->nb_cocktails; i++)
    {   
        printf("\t");
        _print_cocktail(order->cocktails[i]);
    }
    printf("}\n");
}
