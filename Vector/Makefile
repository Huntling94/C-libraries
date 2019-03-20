# VARIABLES - change the values here to match your project setup

# specifying the C Compiler and Compiler Flags for make to use
CC     = gcc
CFLAGS = -Wall

# exe name and a list of object files that make up the program
EXE    = test
OBJ    = vector_test.o vector.o ../Utilities/utils.o ../Math_Extended/math_extended.o


# RULES - these tell make when and how to recompile parts of the project

# the first rule runs by default when you run 'make' ('make rule' for others)
# in our case, we probably want to build the whole project by default, so we
# make our first rule have the executable as its target
#  |
#  v
$(EXE): $(OBJ) # <-- the target is followed by a list of prerequisites
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ)
# ^
# and a TAB character, then a shell command (or possibly multiple, 1 line each)
# (it's very important to use a TAB here because that's what make is expecting)

# the way it works is: if any of the prerequisites are missing or need to be
# recompiled, make will sort that out and then run the shell command to refresh
# this target too

# so our first rule says that the executable depends on all of the object files,
# and if any of the object files need to be updated (or created), we should do
# that and then link the executable using the command given


# okay here's another rule, this time to help make create object files
vector_test.o: vector.c vector.h
	$(CC) $(CFLAGS) -c vector_test.c

../Utilities/utils.o: ../Utilities/utils.c ../Utilities/utils.h

../Math_Extended/math_extended.o: ../Math_Extended/math_extended.c ../Math_Extended/math_extended.h


# so in the future we could save a lot of space and just write these rules:
# $(EXE): $(OBJ)
# 	$(CC) $(CFLAGS) -o $(EXE) $(OBJ)


# it can be accessed by specifying this target directly: 'make clean'
clean:
	rm -f $(OBJ) $(EXE)
