//
//  main.cpp
//  waveur
//
//  Created by user on 7/9/14.
//  Copyright (c) 2014 scoreur. All rights reserved.
//


#include <string>
#include "wav.h"

void test_score2wav(){
    char wavsrc[21]="1.wav";//output,default 1.wav
    char scoresrc[21]="1.txt";//input data,default 1.txt
    
    std::cout<<"please input the data file name(<20 letters):"<<std::endl;
    std::cin>>scoresrc;
    std::cout<<"please input the wave file name (<20 letters):"<<std::endl;
    std::cin>>wavsrc;
    
    WavOut wavout(wavsrc);
    wavout.score2wav(scoresrc);
    std::cout<<"score to wav success!";
}
void test_wav2score(){
    char wavsrc[21]="1.wav";//output,default 1.wav
    std::cout<<"please input the wave file name (<20 letters):"<<std::endl;
    std::cin>>wavsrc;
    
    WavIn wavin(wavsrc);
    freqSpectrum sp;
    short ldata[160000];
    short rdata[160000];
    wavin.get_data(ldata,rdata, 160000,44);
    std::cout<<"beats per minute: "<<60.0/(find_beat(ldata, 160000,200)/(float)sampleps0);
    /*
    for(int k=0;k<20;++k){
        wavin.get_data(ldata,rdata, 16000,16000*k+6044);
        sp.update(rdata+4000,64);
        std::cout<<sp.peaks.size()<<","<<sp.maxpeak<<","<<","<<sp.avg<<","<<sp.maxnote<<","<<sp.norm<<std::endl;
    }*/
}

int main(int argc, char * argv[])
{
    test_score2wav();
    test_wav2score();
        
    
    return 0;
}

