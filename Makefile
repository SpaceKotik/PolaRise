PROG = PolaRise
CC = g++
CFLAGS = -c #-Wno-pragma-once-outside-header

all: $(PROG)
PolaRise: main.cpp game.cpp tile.cpp consts.cpp rayTracing.cpp game.hpp tile.hpp rayTracing.hpp level.cpp level.hpp hero.hpp hero.cpp 
	$(CC) $(CFLAGS) *.cpp 
	$(CC) *.o -o PolaRise -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
clean:
	rm -rf *o *gch file
