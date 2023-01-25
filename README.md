# db-utils

##Purpose :

That library was written in order to make the communication with the postgre database easier. You have at your disposal several functions to for example : connect to the db, get the list of bottles...

##Build the library :

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

##Compilation :

If you want to use the static library, you have just to specify the path of the lib and its name :

```bash
gcc file.c -L path/to/libdb-utils.a -ldb-utils -o file
```

On the other hand, if you want to use the dynamic library, specify the name of the lib during the links edition :

```bash
gcc file.c -ldb-utils -o file
```

##Data structures :

Some structs have been defined in order to corresponding to a row in each table in the database. It means, it provides a bottle_t type, module_t, ...
**Module :**

```c
typedef struct
{
    id_db_t id;
    ip_address_t ip_address;
} module_t;

```

**Bottle**

```c
typedef struct
{
    id_db_t id;
    url_t url;
    float quantity;
    module_t *module;
} bottle_t;
```

**Cocktail**

```c
typedef struct
{
    id_db_t id;
    float price;
    url_t image;
} cocktail_t;
```

##Functions :

###Utils
Connect to the database :

```c
PGconn *db_connect(const char *host, const char *dbname, const char *user, const char *password)
```

Disconnect from the database :

```c
void db_disconnect(PGconn *conn)
```

Know if a provided ipv4 address is valid :

```c
int *check_ip_address(char *ip_address)
```

###Bottles
Get the list of bottles :

```c
bottle_t **get_bottles(PGconn *conn, int *length)
```

Create a bottle :

```c
bottle_t *create_bottle(PGresult *result, int row, int nbFields)
```

###Modules
Get the list of modules :

```c
module_t **get_modules(PGconn *conn, int *length)
```

Create a module :

```c
module_t *create_module(PGresult *result, int row, int nbFields)
```

Insert a module in the database :

```c
void insert_module(PGconn *conn, module_t *module)
```

###Cocktails
Get the list of cocktails :

```c
cocktail_t **get_cocktails(PGconn *conn, int *length)
```

Create a cocktail :

```c
cocktail_t *create_cocktail(PGresult *result, int row, int nbFields)
```

###Functions not to use (used to develop)

Loop though a query result :

```c
void *_loop_through_data(PGresult *result, void *(*callback)(PGresult *, int, int))
```

Print an object :

```c
void _print_{object_name}(object_type *object)
```

example : `void _print_cocktail(cocktail_t *cocktail)`
