# TODO : change makefile

EXE=executable.exe
CC=g++ -std=c++11
FLAGS=-Wall -pthread -lm -g -lSDL2 #-DGUI
DSRC=src/
DOBJ=obj/
DINC=inc/

OBJ=$(DOBJ)field.o $(DOBJ)main.o $(DOBJ)simulation.o $(DOBJ)gui.o $(DOBJ)four-threads-simulation-no-synchro.o $(DOBJ)n-threads-simulation-no-synchro.o $(DOBJ)four-threads-simulation-synchro-sem.o $(DOBJ)n-threads-simulation-synchro-sem.o $(DOBJ)four-threads-simulation-synchro-monitor.o $(DOBJ)n-threads-simulation-synchro-monitor.o $(DOBJ)monitor.o

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

$(DOBJ)four-threads-simulation-synchro-sem.o:$(DSRC)four-threads-simulation-synchro-sem.c $(DINC)four-threads-simulation-synchro-sem.h $(DINC)field.h
	$(CC) $(FLAGS) -c $< -o $@ 

$(DOBJ)n-threads-simulation-synchro-sem.o:$(DSRC)n-threads-simulation-synchro-sem.c $(DINC)n-threads-simulation-synchro-sem.h $(DINC)field.h
	$(CC) $(FLAGS) -c $< -o $@ 

$(DOBJ)four-threads-simulation-synchro-monitor.o:$(DSRC)four-threads-simulation-synchro-monitor.c $(DINC)four-threads-simulation-synchro-monitor.h $(DINC)field.h $(DSRC)monitor.c $(DINC)monitor.h
	$(CC) $(FLAGS) -c $< -o $@ 

$(DOBJ)n-threads-simulation-synchro-monitor.o:$(DSRC)n-threads-simulation-synchro-monitor.c $(DINC)n-threads-simulation-synchro-monitor.h $(DINC)field.h $(DSRC)monitor.c $(DINC)monitor.h
	$(CC) $(FLAGS) -c $< -o $@ 

$(DOBJ)monitor.o:$(DSRC)monitor.c $(DINC)monitor.h
	$(CC) $(FLAGS) -c $< -o $@ 

clean:
	rm -f ./src/*~ ./obj/*.o ./*~ ./inc/*~ ./*.exe
