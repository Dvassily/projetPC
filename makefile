# TODO : change makefile

EXE=executable.exe
CC=gcc -std=c99
FLAGS=-Wall -pthread -lm -g -lSDL2
DSRC=src/
DOBJ=obj/
DINC=inc/

OBJ=$(DOBJ)field.o $(DOBJ)main.o $(DOBJ)simulation.o

$(EXE):$(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o $(EXE)

$(DOBJ)main.o:$(DSRC)main.c
	$(CC) $(FLAGS) -c $< -o $@ 

$(DOBJ)field.o:$(DSRC)field.c $(DINC)field.h
	$(CC) $(FLAGS) -c $< -o $@

$(DOBJ)simulation.o:$(DSRC)simulation.c $(DINC)simulation.h $(DINC)field.h
	$(CC) $(FLAGS) -c $< -o $@ -pthread

clean:
	rm -f ./src/*~ ./obj/*.o ./*~ ./inc/*~ ./*.exe
