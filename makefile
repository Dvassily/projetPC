# TODO : change makefile

EXECUTABLE=executable.exe
COMP=gcc
FLAGS=-Wall -pthread -lm
DSRC=src/
DOBJ=obj/
DINC=inc/

OBJ=$(DOBJ)field.o $(DOBJ)main.o $(DOBJ)simulation.o

$(EXECUTABLE):$(OBJ)
	$(COMP) $(FLAGS) $(OBJ) -o $(EXECUTABLE)

$(DOBJ)main.o:$(DSRC)main.c
	$(COMP) $(FLAGS) -c $< -o $@ 

$(DOBJ)field.o:$(DSRC)field.c $(DINC)field.h
	$(COMP) $(FLAGS) -c $< -o $@

$(DOBJ)simulation.o:$(DSRC)simulation.c $(DINC)simulation.h
	$(COMP) $(FLAGS) -c $< -o $@ -pthread



clean:
	rm -f ./src/*~ ./obj/*.o ./*~ ./inc/*~ ./*.exe
