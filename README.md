# db-utils

##Goal :

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
    int id;
    char ip_address[255];
} module_t;

```

**Bottle**
The struct is defined as below :

```c
typedef struct
{
    int id;
    char url[255];
    float quantity;
    module_t *module;
} bottle_t;
```

##Functions :
Connect to the database :

```c
PGconn *db_connect(const char *host, const char *dbname, const char *user, const char *password)
```

Disconnect from the database :

```c
void db_disconnect(PGconn *conn)
```

Get the lists of bottles :

```c
bottle_t **get_bottles(PGconn *conn, int *length)
```

Create a bottle :

```c
bottle_t *create_bottle(PGresult *result, int row, int nbFields)
```

Create a module :

```c
module_t *create_module(PGresult *result, int row, int nbFields)
```

Loop though a query result :

```c
void *_loop_through_data(PGresult *result, void *(*callback)(PGresult *, int, int))
```
