.PHONY: all clean
vers = c++98

VPATH = ../
vpath %.h ../include:
objects = main.o wav.o score.o complex.o spectrum.o
scoreur: $(objects)
	g++ $(objects)      -std=$(vers) -o ../scoreur
main.o:        main.cpp types.h wav.h
	@g++ -c main.cpp     -std=$(vers) -o main.o
wav.o:         wav.cpp wav.h
	@g++ -c wav.cpp      -std=$(vers) -o wav.o
score.o:       score.cpp score.h spectrum.h
	@g++ -c score.cpp    -std=$(vers) -o score.o
spectrum.o:    spectrum.cpp spectrum.h
	@g++ -c spectrum.cpp -std=$(vers) -o spectrum.o
complex.o:     complex.cpp complex.h
	@g++ -c complex.cpp  -std=$(vers) -o complex.o

########## This is comment ##########
all: clean scoreur
	@echo "clean and make all!"
clean:
	@-rm $(objects)