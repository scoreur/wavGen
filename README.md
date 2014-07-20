wavGen
======

score-to-wavefile and wavefile-to-score.

##HeaderFile:
1. types.h: define unsigned type of different lengths;
2. complex.h: define complex number class and some other mathemathical functions;
3. spectrum.h: for frequency/time-domain analysis of wave data, finding key notes, beats & tempo;
4. wav.h: read/write wave file, provide score-to-wavefile and wavefile-to-score functions.

##Usage:
1. Launch Terminal, navigate to the directory holding main.cpp and all the header files;
2. Compile and run the program:
```
for mac/linux: g++ main.cpp -std=c++0x -o wavGen
./wavGen
for win: g++ main.cpp -std=c++0x -o wavGen.exe
./wavGen.exe
```           

3. Score-to-wavefile: input the scorefile name: eg. score.txt; input the wavfile name: eg. wave.wav,
then the wave file will be generated.
4. Wavefile-to-score: input the wavefile name.

##Score structure (see "score.txt"):
1. Three integers at the beginning, i)number of beats; ii)number of scores; iii)tempo in beat-per-minute;
2. Two integers for each score, for pitch (440Hz as 0) and duration (in multiples of beats) respectively. 
