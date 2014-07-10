wavGen
======

wav file generating

##Usage:
1. Compile the progam (with make);
1. Save the wave score as `sine.txt`;
1. Run main.exe, the wave file  `sine.wav` will be generated.

##Score structure:
1. Two integers at the beginning, for wave duration and the number of scores;
1. Two integers for each score, for pitch and duration respectively.

##Example:
```
16 14
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