//
//  main.cpp
//  wavReader
//
//  Created by user on 5/2/14.
//  Copyright (c) 2014 user. All rights reserved.
//


#include <math.h>
#include <fstream>
#include <vector>
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
      
    fstream wavin2;
    wavin2.open(in2addr,ios::in);
	vector<short> v_tnum;
	vector<double> v_freq;
	
	short f_i,t_i;
	while(!wavin2.eof())
	{
		wavin2>>f_i>>t_i;
		v_freq.push_back(fr(f_i));
		v_tnum.push_back(t_i);
	}
    wavin2.close();
	
short snum=v_tnum.size();// number of scores
short tlen=0;// total duration of scores	
for(vector<short>::iterator j=v_tnum.begin();j!=v_tnum.end();++j)
    tlen += *j;
	
    cout<<"time:"<<tlen*bttfrac<<"s; "<<snum;
    
    wavfile wavin1(in1addr,tlen*btt);
    wavin1.print_info();
    wavin1.put_data(btt,snum,v_freq,v_tnum);
    
    return 0;
}

