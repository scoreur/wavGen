wavGen
======

score-to-wavefile and wavefile-to-score.

##HeaderFile:
1. types.h: define unsigned type of different lengths;
2. complex.h: define complex number class and some other mathemathical functions;
3. spectrum.h: for frequency/time-domain analysis of wave data, finding key notes, beats & tempo;
4. score.h: define score structure based on events, in light of MID format;
5. wav.h: read/write wave file, provide score-to-wavefile and wavefile-to-score functions;
6. mpeg3.h: read mp3 file, currently unused (to be continued).

##Usage:
1. Launch Terminal, navigate to the directory holding main.cpp and all the header files;
2. Compile the program using "make" under mac/linux (for windows you can make small change to the  Makefile);
3. Run the program using "./scoreur", imput the required filenames.

##Score structure version 1.1 (see "scoreV1.txt"):
1. First line: "scoreV1.1";
2. Three integers in the second line, i)number of beats; ii)number of scores; iii)tempo in beat-per-minute;
3. Two integers for each score, for pitch (440Hz as 0) and duration (in multiples of beats) respectively. 

##Score structure version 2.0 (see "scoreV2.txt"):
1. First line: "scoreV2.0";
2. Three integers for each line: i) time difference in miliseconds; ii) type of events; iii) arguments;
3. see "score.h" for more infomation for MID::EVENT.
