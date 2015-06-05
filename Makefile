CC = gcc
OBJ = vector.o MerryGoRound.o LoadShader.o Matrix.o OBJParser.o List.o StringExtra.o
CFLAGS = -g -Wall -Wextra -std=c99

LDLIBS=-lm -lglut -lGLEW -lGL

MerryGoRound: $(OBJ)
	 $(CC) -o $@ $^ $(CFLAGS) $(LDLIBS)

noobj:
	rm -f *.o

clean:
	rm -f *.o MerryGoRound 
.PHONY: all clean
