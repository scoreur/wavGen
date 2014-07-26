//
//  main.cpp
//  waveur
//
//  Created by user on 7/9/14.
//  Copyright (c) 2014 scoreur. All rights reserved.
//


//#include <string>
#include "wav.h"
using namespace std;

void test_score2wav(){
    char wavsrc[21]="1.wav";//output,default 1.wav
    char scoresrc[21]="1.txt";//input data,default 1.txt
    cout<<"please input the scoreV1 file name(<20 letters):"<<endl;
    cin>>scoresrc;
    cout<<"please input the wave file name (<20 letters):"<<endl;
    cin>>wavsrc;
    
    WavOut wavout(wavsrc);
    if(wavout.score2wav(scoresrc))
        cout<<"score to wav: successful!"<<endl;
}
void test_wav2score(){
    char wavsrc[21]="1.wav";//output,default 1.wav
    char scoresrc[21]="1.txt";//input data,default 1.txt
    cout<<"please input the wave file name(<20 letters):"<<endl;
    cin>>wavsrc;
    cout<<"please input the scoreV1 file name(<20 letters):"<<endl;
    cin>>scoresrc;
    
    WavIn wavin(wavsrc);
    if(wavin.wav2score(scoresrc))
        cout<<"wav to score: successful!"<<endl;
}
void test_score2mid(){
    char wavsrc[21]="1.wav";//output,default 1.wav
    char scoresrc[21]="1.txt";//input data,default 1.txt
    
    cout<<"please input scoreV2 file name(<20 letters):"<<endl;
    cin>>scoresrc;
    cout<<"please input the wave file name(<20 letters):"<<endl;
    cin>>wavsrc;
    
    midSeq mids;
    mids.score2mid(scoresrc);
    
    WavOut wavout(wavsrc);
    if(wavout.score2wav(mids))
        cout<<"score to wav: successful!"<<endl;
    
}
int main(int argc, char * argv[])
{
    test_score2wav();
    //test_wav2score();
    test_score2mid();
    return 0;
}

