CC = gcc -std=gnu99
GDB = gcc -std=gnu99 -ggdb
OBJECTS = product.o my_shell.o parse.o
mys_objs = my_shell.o parse.o shell_array.o
SHELL_O_DIR = shell_obj/
SHELL_OBJ=$(addprefix $(SHELL_O_DIR), $(mys_objs))

my_shell: $(SHELL_OBJ)
	$(CC) $(SHELL_OBJ) -o my_shell

my_shell_gdb: $(SHELL_OBJ)	
	$(CC) $(SHELL_OBJ) -ggdb -o my_shell

strip:
	strip my_shell

shell_obj/my_shell.o: shell_src/my_shell.c
	$(CC) -c shell_src/my_shell.c -o $(SHELL_O_DIR)my_shell.o

shell_obj/parse.o: shell_src/parse.c shell_src/parse.h
	$(CC) -c shell_src/parse.c -o $(SHELL_O_DIR)parse.o

shell_obj/shell_array.o: shell_src/shell_array.c shell_src/shell_array.h
	$(CC) -c shell_src/shell_array.c -o $(SHELL_O_DIR)shell_array.o

clean:
	rm SHELL_OBJ

cc:
	rm core.*
