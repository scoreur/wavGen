//
//  main.cpp
//  wavReader
//
//  Created by user on 5/2/14.
//  Copyright (c) 2014 user. All rights reserved.
//


#include <math.h>
#include <fstream>
#include "wav.h"

using namespace std;

const int sampleps0=44100;//采样数
const double fr0= 440*2*PI/sampleps0;//标准音（圆）频率
const double bttfrac=0.3;
const int btt=bttfrac*sampleps0;//每拍采样数

//global var

double fr(int freq)//音数与频率换算
{
    return fr0*pow(2,freq/12.0);
}


int main(int argc, char * argv[])
{
    
    char in1addr[]="sine.wav";//输出
    char in2addr[]="sine.txt";//输入文件
    
    short snum;// number of scores
    short tlen;// total duration of scores
    
    fstream wavin2;
    wavin2.open(in2addr,ios::in);
    wavin2>>tlen>>snum;
	
    cout<<"time:"<<tlen*bttfrac<<"s; "<<snum;
    short *frnum = new short[snum]; //score frequency number
    short *tnum = new short[snum]; 	//score duration
    double *freq=new double[snum]; 	//score frequency value
    for(int i=0;i<snum;++i)
    {
        wavin2>>frnum[i]>>tnum[i];
        freq[i]=fr(frnum[i]);
    }
    wavin2.close();
    
    wavfile wavin1(in1addr,tlen*btt);
    wavin1.print_info();
    wavin1.put_data(btt,snum,freq,tnum);
    
    return 0;
}

