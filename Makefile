FLAGS= -Wall
CC= gcc
LIBS= -lpq
OBJECTS=objects
BUILD=build
SRC=src
INCLUDE=include


all: static shared

static: objects_files
	ar rcs $(BUILD)/libdb-utils.a $(OBJECTS)/*.o

shared: objects_files
	$(CC) -shared -o $(BUILD)/libdb-utils.so $(OBJECTS)/*.o

objects_files: $(SRC)/utils.c $(INCLUDE)/*.h
	$(CC) $(FLAGS) -c -fPIC $(SRC)/utils.c -o $(OBJECTS)/db-utils.o

install: static shared
	sudo cp $(BUILD)/libdb-utils.so /usr/lib
	sudo mkdir -p /usr/include/db-utils
	sudo cp $(INCLUDE)/*.h /usr/include/db-utils

uninstall:
	sudo rm -rf /usr/include/db-utils
	sudo rm -f /usr/lib/libdb-utils.so

clean: uninstall
	rm -f $(OBJECTS)*.o
	rm -f $(BUILD)*.a
	rm -f $(BUILD)*.so