main.exe: types.h wav.h
	g++ main.cpp -o main.exe
clean: 
	rm *.o
