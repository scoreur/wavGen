objects = main.o wav.o score.o complex.o spectrum.o
scoreur: $(objects)
	g++ $(objects)      -std=c++0x -o scoreur
main.o:        main.cpp types.h wav.h
	g++ -c main.cpp     -std=c++0x -o main.o
wav.o:         wav.cpp wav.h
	g++ -c wav.cpp      -std=c++0x -o wav.o
score.o:       score.cpp score.h spectrum.h
	g++ -c score.cpp    -std=c++0x -o score.o
spectrum.o:    spectrum.cpp spectrum.h
	g++ -c spectrum.cpp -std=c++0x -o spectrum.o
complex.o:     complex.cpp complex.h
	g++ -c complex.cpp  -std=c++0x -o complex.o

########## This is comment ##########
.PHONY: clean
clean:
	rm $(objects)