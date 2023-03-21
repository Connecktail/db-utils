# db-utils

## Purpose :

That library was written in order to make the communication with the postgre database easier. You have at your disposal several functions to for example : connect to the db, get the list of bottles...

## Build the library :

- clone the repository
- execute the following commands :

```bash
make
make install
```

The headers have been added to the /usr/include/db-utils repository

In your code, you have just to include the dbutils.h header file :

```c
#include <db-utils/dbutils.h>
```

## Compilation :

If you want to use the static library, you have just to specify the path of the lib and its name :

```bash
gcc file.c -L path/to/libdb-utils.a -ldb-utils -o file
```

On the other hand, if you want to use the dynamic library, specify the name of the lib during the links edition :

```bash
gcc file.c -ldb-utils -o file
```

## Data structures :

Some structs have been defined in order to corresponding to a row in each table in the database. It means, it provides a bottle_t type, module_t, ...
**Module :**

```c
typedef struct
{
    mac_address_t mac_address;
    ip_address_t ip_address;
} module_t;

```

**Bottle**

```c
typedef struct
{
    id_db_t id;
    char name[255];
    url_t url;
    float quantity;
    module_t *module;
    float price;
} bottle_t;
```

**Cocktail**

```c
typedef struct
{
    id_db_t id;
    char name[255];
    char description[255];
    float price;
    url_t image_url;
    bool personalized;
} cocktail_t;
```

**Step**

```c
typedef struct
{
    id_db_t id;
    float quantity;
    char description[255];
    id_db_t id_cocktail;
    bottle_t *bottle;
    char message[255];
} step_t;
```

**Order**

```c
typedef struct
{
    id_db_t id;
    date_t date;
    float price;
    int status; // 0 = false in db, 1 = true in db, 2 = NULL in db
    int nb_cocktails;
    cocktail_t **cocktails;
} order_t;
```

## Functions :

### Utils

- Connect to the database :

```c
PGconn *db_connect(const char *host, const char *dbname, const char *user, const char *password)
```

- Disconnect from the database :

```c
void db_disconnect(PGconn *conn)
```

- Know if a provided ipv4 address is valid :

```c
int *check_ip_address(char *ip_address)
```

- Know if a provided url is valid :

```c
int *check_url(char *url)
```

- Know if a provided value is positive :

```c
int *check_positive(void *arg, types type)
```

for the parameter please refer to the `types` enum.

```c
typedef enum
{
    INT,
    FLOAT,
    DOUBLE
} types;
```

### Bottles

- Get the list of bottles :

```c
bottle_t **get_bottles(PGconn *conn, int *length)
```

- Create a bottle :

```c
bottle_t *create_bottle(PGresult *result, int row, int nbFields)
```

- Insert bottle in the database :

```c
void insert_bottle(PGconn *conn, bottle_t *bottle)
```

- Delete a bottle from the database :

```c
void *update_module(PGconn *conn, module_t *module, ip_address_t new_ip_address)
```

- Update a bottle in the database :

```c
void *update_bottle(PGconn *conn, bottle_t *bottle, char *new_name[255], url_t *new_url, float *new_quantity, module_t *new_module, float *new_price)
```

### Modules

- Get the list of modules :

```c
module_t **get_modules(PGconn *conn, int *length)
```

- Create a module :

```c
module_t *create_module(PGresult *result, int row, int nbFields)
```

- Insert a module in the database :

```c
void insert_module(PGconn *conn, module_t *module)
```

- Delete a module from the database :

```c
void delete_module(PGconn *conn, mac_address_t mac_address)
```

- Update a module in the database :

```c
void *update_module(PGconn *conn, module_t *module, ip_address_t new_ip_address)
```

### Cocktails

- Get the list of cocktails :

```c
cocktail_t **get_cocktails(PGconn *conn, int *length)
```

- Get a cocktail by id :

```c
cocktail_t *get_cocktail_by_id(PGconn *conn, id_db_t id)
```

- Get the list of cocktails of a specifig order:

```c
cocktail_t **get_order_cocktails(PGconn *conn, int *length, id_db_t id_order)
```

- Create a cocktail :

```c
cocktail_t *create_cocktail(PGresult *result, int row, int nbFields)
```

- Insert a cocktail in the database :

```c
void insert_cocktail(PGconn *conn, cocktail_t *cocktail)
```

- Delete a cocktail from the database :

```c
void delete_cocktail(PGconn *conn, id_db_t id)
```

- Update a cocktail in the database :

```c
void *update_cocktail(PGconn *conn, cocktail_t *cocktail, float *new_price, url_t *new_image)
```

### Steps

- Get the list of steps of a cocktail :

```c
step_t **get_cocktail_steps(PGconn *conn, int *length, id_db_t id_cocktail)
```

- Create a step :

```c
step_t *create_step(PGresult *result, int row, int nbFields)
```

- Insert a cocktail step in the database :

```c
void insert_step(PGconn *conn, step_t *step)
```

- Delete a step from the database :

```c
void delete_step(PGconn *conn, id_db_t id)
```

- Update a step in the database :

```c
void *update_step(PGconn *conn, step_t *step, float *new_quantity, char *new_desc[255], bottle_t *new_bottle, char *new_message[255])
```

### Orders

- Get an order by id :

```c
order_t *get(PGconn *conn, id_db_t id)
```

- Get the list of orders :

```c
order_t **get_orders(PGconn *conn, int *length)
```

- Create an order :

```c
order_t *create_order(PGresult *result, int row, int nbFields)
```

- Insert an order (also insert the personnalized cocktails in the cocktails table, and the links in the cocktails_orders table):

```c
void insert_order(PGconn *conn, order_t *order)
```

- Update an order (can only update the price and the status):

```c
void *update_order(PGconn *conn, order_t *order, float *new_price, int *new_status)
```

### Functions not to use (used to develop)

- Loop though a query result :

```c
void *_loop_through_data(PGresult *result, void *(*callback)(PGresult *, int, int))
```

- Print an object :

```c
void _print_{object_name}(object_type *object)
```

example : `void _print_cocktail(cocktail_t *cocktail)`

- Insert an object in the database :

```c
id_db_t _insert_data(PGconn *conn, char *query)
```

- Check if an insertion was successful :

```c
void *check_insertion(PGconn *conn, PGresult *result)
```

- Delete an object from the database :

```c
void *_delete_data(PGconn *conn, char *query)
```

- Update an object in the database :

```c
void *_update_data(PGconn *conn, char *query)
```

- Concatenate two strings and realloc the first one :

```c
char *_concatenate_formated(char *dst, char *src, int *dst_length)
```

- Format string to be used in a sql request (e.g. add quotes) :

```c
void format_string(char *target)
```

- Create a date_t object from a string

```c
date_t create_date(char *date)
```

- Convert character ('f', 't' or '') into a int (0, 1 or 2)

```c
int convert_bool(char car)
```

### Notes :

- When you want to create a new object and insert it into the database please set the id field of the struct to NULL. The id will be automatically generated by the database. And the struct will be modified by the function.
