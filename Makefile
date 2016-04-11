# cs335 hw1
CFLAGS = -I ./include
##LIB = ./libggfonts.so
LFLAGS = -lrt -lX11 -lGLU -lGL -lm #-lXrandr

# to compile your project, type make and press enter

all: hw1

hw1: hw1.cpp  
	g++ $(CFLAGS) hw1.cpp libggfonts.a  $(LFLAGS) -ohw1

clean:
	rm -f hw1
	rm -f *.o

