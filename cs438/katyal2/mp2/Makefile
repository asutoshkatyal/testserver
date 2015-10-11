#
# Machine Problem #2
# CS 438
#

CC = g++
INC = -I.
FLAGS = -std=c++0x -g -Wall
LIBS = -lpthread

all: reliable_sender reliable_receiver

sender_main.o: sender_main.cpp sender_main.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)


receiver_main.o: receiver_main.cpp
reliable_receiver: receiver_main.o
	$(CC) $^ -o $@ $(LIBS)

reliable_sender: sender_main.o 
	$(CC) $^ -o $@ $(LIBS)

.PHONY : clean
clean:
	-rm -r -f *.o reliable*
