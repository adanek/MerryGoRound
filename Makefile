CC = gcc
OBJ = MerryGoRound.o LoadShader.o Matrix.o
CFLAGS = -g -Wall -Wextra

LDLIBS=-lm -lglut -lGLEW -lGL

MerryGoRound: $(OBJ)
	 $(CC) -o $@ $^ $(CFLAGS) $(LDLIBS)

noobj:
	rm -f *.o

clean:
	rm -f *.o MerryGoRound 
.PHONY: all clean
