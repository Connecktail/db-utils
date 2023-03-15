#include <stdlib.h>
#include <string.h>
#include <time.h>

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
            order->status = convert_bool(PQgetvalue(result, row, i)[0]);
    }

    return order;
}

order_t *get(PGconn *conn, id_db_t id) {
    char query[QUERY_LENGTH];
    sprintf(query, "SELECT o.*, COUNT(c_o.id) as nb_cocktails FROM orders AS o, cocktails_orders AS c_o WHERE o.id = c_o.id_order AND o.id=%lld GROUP BY o.id", *id);
    PGresult *result = PQexec(conn, query);
    order_t **orders = (order_t **)_loop_through_data(result, &create_order);
    if (orders == NULL)
        return NULL;
    PQclear(result);
    order_t *order = orders[0];
    order->cocktails = get_order_cocktails(conn, &order->nb_cocktails, order->id);
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

void insert_order(PGconn *conn, order_t *order) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    
    // insert order
    int *check = check_positive((void *)&(order->price), FLOAT);
    if (check == NULL)
    {
        fprintf(stderr, "Invalid price\n");
        return;
    }
    order->status = 0;
    order->date.year = tm.tm_year + 1900;
    order->date.month = tm.tm_mon + 1;
    order->date.day = tm.tm_mday;
    order->date.hour = tm.tm_hour;
    order->date.minute = tm.tm_min;

    char query[QUERY_LENGTH];
    sprintf(query, "INSERT INTO orders (date, price, status) VALUES (NOW(), %f, FALSE) RETURNING id", order->price);
    id_db_t id = _insert_data(conn, query);
    if (id == NULL)
        return;
    order->id = id;

    // insert personnalized cocktails and link them to the order
    for (int i = 0; i < order->nb_cocktails; i++)
    {
        if(order->cocktails[i]->personalized == 1){
            insert_cocktail(conn, order->cocktails[i]);
        }
        sprintf(query, "INSERT INTO cocktails_orders (id_cocktail, id_order) VALUES (%lld, %lld) RETURNING id", *order->cocktails[i]->id, *order->id);
        _insert_data(conn, query);
    }
}


void *update_order(PGconn *conn, order_t *order, float *new_price, int *new_status)
{
    if (order == NULL)
    {
        fprintf(stderr, "Invalid bottle\n");
        return NULL;
    }

    if (new_price == NULL && new_status == NULL)
    {
        fprintf(stderr, "Nothing to update\n");
        return NULL;
    }

    char *query = (char *)calloc(20, sizeof(char));
    char query_add[255];
    sprintf(query_add, "UPDATE orders SET");
    int query_length = strlen(query_add) + 1;
    query = (char *)realloc(query, query_length * sizeof(char));
    strcat(query, query_add);

    if (new_price != NULL)
    {
        int *check = check_positive((void *)new_price, FLOAT);
        if (check == NULL)
        {
            fprintf(stderr, "Order price must be positive\n");
            return NULL;
        }
        strcpy(query_add, "");
        sprintf(query_add, " price = %f,", *new_price);
        query = _concatenate_formated(query, query_add, &query_length);
    }
    if (new_status != NULL)
    {   
        strcpy(query_add, "");
        sprintf(query_add, " status = %s,", (*new_status==1?"TRUE":((*new_status==2)?"NULL":"FALSE")));
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
    sprintf(query_add, " WHERE id = %lld", *(order->id));
    query = _concatenate_formated(query, query_add, &query_length);

    printf("query: %s\n", query);

    void *check = _update_data(conn, query);
    if (check != NULL)
    {
        order->price = new_price == NULL ? order->price : *new_price;
        order->status = new_status == NULL ? order->status : ((*new_status==2)?NULL:*new_status);
    }
    else
    {
        printf("error on update\n");
    }

    return order;
}