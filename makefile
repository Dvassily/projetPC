# TODO : change makefile

EXE=executable.exe
CC=g++ -std=c++11
FLAGS=-Wall -pthread -lm -g -lSDL2 #-DGUI
DSRC=src/
DOBJ=obj/
DINC=inc/

OBJ=$(DOBJ)field.o $(DOBJ)main.o $(DOBJ)simulation.o $(DOBJ)gui.o $(DOBJ)four-threads-simulation-no-synchro.o $(DOBJ)n-threads-simulation-no-synchro.o

$(EXE):$(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o $(EXE)

$(DOBJ)main.o:$(DSRC)main.c
	$(CC) $(FLAGS) -c $< -o $@ 

$(DOBJ)field.o:$(DSRC)field.c $(DINC)field.h
	$(CC) $(FLAGS) -c $< -o $@

$(DOBJ)simulation.o:$(DSRC)simulation.c $(DINC)simulation.h $(DINC)field.h
	$(CC) $(FLAGS) -c $< -o $@ 

$(DOBJ)gui.o:$(DSRC)gui.c $(DINC)gui.h $(DINC)field.h
	$(CC) $(FLAGS) -c $< -o $@ 

$(DOBJ)four-threads-simulation-no-synchro.o:$(DSRC)four-threads-simulation-no-synchro.c $(DINC)four-threads-simulation-no-synchro.h $(DINC)field.h
	$(CC) $(FLAGS) -c $< -o $@ 

$(DOBJ)n-threads-simulation-no-synchro.o:$(DSRC)n-threads-simulation-no-synchro.c $(DINC)n-threads-simulation-no-synchro.h $(DINC)field.h
	$(CC) $(FLAGS) -c $< -o $@ 


clean:
	rm -f ./src/*~ ./obj/*.o ./*~ ./inc/*~ ./*.exe
