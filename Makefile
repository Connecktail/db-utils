FLAGS= -Wall
CC= gcc
LIBS= -lpq
OBJECTS=objects
BUILD=build
SRC=src
INCLUDE=include

STATIC=$(BUILD)/libdb-utils.a
SHARED=$(BUILD)/libdb-utils.so

OBJECTS_FILES=$(OBJECTS)/utils.o $(OBJECTS)/bottle.o $(OBJECTS)/cocktail.o $(OBJECTS)/module.o

all: $(STATIC) $(SHARED)

$(STATIC): $(OBJECTS_FILES)
	ar rcs $@ $(OBJECTS_FILES)

$(SHARED): $(OBJECTS_FILES)
	$(CC) -shared $(FLAGS) $(OBJECTS_FILES) -lpq -o $@

$(OBJECTS)/%.o: $(SRC)/%.c $(INCLUDE)/*.h
	$(CC) $(FLAGS) -c -fPIC -o $@ $<

install: $(STATIC) $(SHARED)
	sudo cp $(SHARED) /usr/lib
	sudo mkdir -p /usr/include/db-utils
	sudo cp $(INCLUDE)/*.h /usr/include/db-utils

uninstall:
	sudo rm -rf /usr/include/db-utils
	sudo rm -f /usr/lib/libdb-utils.so

clean: uninstall
	rm -f $(OBJECTS)/*
	rm -f $(BUILD)/*

test: test.c $(SHARED)
	$(CC) $(FLAGS) -o test test.c -L$(BUILD) -ldb-utils -lpq -o test