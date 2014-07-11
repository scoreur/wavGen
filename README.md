wavGen
======

score to wavfile

##Usage:
 Compile the progam in Terminal (first navigating to the directory):
  mac/linux: g++ main.cpp -std=c++0x -o score2wav
             ./score2wav
             input the scorefile name: eg. score.txt
             input the wavfile name: eg. wave.wav
  win: g++ main.cpp -std=c++0x -o score2wav.exe
       and similarly as above

 then the wave file will be generated.

##Score structure:
1. Three integers at the beginning, i)number of beats; ii)number of scores; iii)tempo in beat per minutes;
2. Two integers for each score, for pitch (440Hz as 0) and duration (in multiples of beats) respectively.

##Example:
```
16 14 108
1 1
1 1 
8 1
8 1
10 1
10 1 
8 2
6 1
6 1
5 1
5 1
3 1
3 1
1 2
```
see also "score.txt"
