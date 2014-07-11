//
//  main.cpp
//  waveur
//
//  Created by user on 7/9/14.
//  Copyright (c) 2014 scoreur. All rights reserved.
//


#include <string>
#include "wav.h"

int main(int argc, char * argv[])
{
    
    char wavsrc[21]="1.wav";//output,default 1.wav
    char scoresrc[21]="1.txt";//input data,default 1.txt
    
    std::cout<<"please input the data file name(<20 letters):"<<std::endl;
    std::cin>>scoresrc;
    std::cout<<"please input the wave file name (<20 letters):"<<std::endl;
    std::cin>>wavsrc;

    WavOut wavout(wavsrc);
    wavout.score2wav(scoresrc);
    
    //test wav to score
    WavIn wavin(wavsrc);
    fr_Spectrum sp;
    short ldata[8000];
    short rdata[8000];
    for(int k=0;k<40;++k){
        wavin.get_data(ldata,rdata, 8000,16000*k+8044);
        sp.update(rdata+4000,64);
        std::cout<<sp.maxnote<<":"<<sp.maxpeak<<";";
        
    }
    //test the main note

    return 0;
}

