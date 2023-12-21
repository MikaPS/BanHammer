# Name of the program this Makefile is going to build
EXECBIN  = banhammer

# All available .c files are included as SOURCES
SOURCES  = $(wildcard *.c)
# Each .c file has a corresponding .o file
OBJECTS  = $(SOURCES:%.c=%.o)

CC       = clang
CFLAGS   = -Wall -Wpedantic -Werror -Wextra -Ofast -gdwarf-4

.PHONY: all clean spotless format

# built when 'make' is run without arguments.
all: $(EXECBIN)

# The program depends on *all* of the OBJECTS.
# This means the .o files from *every* .c file in the directory,
# given how we defined $(OBJECTS)
$(EXECBIN): $(OBJECTS)
	$(CC) -o $@ $^

# This is a default rule for creating a .o file from the corresponding .c file.
%.o : %.c
	$(CC) $(CFLAGS) -c $<

# Removes all of the OBJECT files that it can build.
# They can be recreated by running 'make all'.
clean:
	rm -f $(OBJECTS)

# Removes the derived files: the executable itself and
# all of the OBJECT files that it can build.
# They can be recreated by running 'make all'.
spotless:
	rm -f $(EXECBIN) $(OBJECTS)

# formats all files based on the clang format. 
format:
	clang-format -i -style=file banhammer.c
	clang-format -i -style=file bf.c 
	clang-format -i -style=file bv.c 
	clang-format -i -style=file ht.c 
	clang-format -i -style=file ll.c 
	clang-format -i -style=file node.c 
	clang-format -i -style=file parser.c 
