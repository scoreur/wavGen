//
//  spectrum.h
//  
//
//  Created by wyj on 7/23/14.
//
//

#ifndef _spectrum_h
#define _spectrum_h

#include "complex.h"
#include <vector>

const int NOTENUM = 88; //Piano音符数目
const int LOWEST_NOTE = -48;
const int HIGHEST_NOTE = 39;
const int NOISE = 100.0;
const int sampleps0=44100;//默认每秒采样数
const double fr0= 440*2*PI/sampleps0;//标准音（圆）频率除以采样数
const float freq_piano[NOTENUM] = {27.5,29.1352,30.8677,32.7032,34.6478,36.7081,38.8909,41.2034,43.6535,46.2493,48.9994,51.9131,55,58.2705,61.7354,65.4064,69.2957,73.4162,77.7817,82.4069,87.3071,92.4986,97.9989,103.826,110,116.541,123.471,130.813,138.591,146.832,155.563,164.814,174.614,184.997,195.998,207.652,220,233.082,246.942,261.626,277.183,293.665,311.127,329.628,349.228,369.994,391.995,415.305,440,466.164,493.883,523.251,554.365,587.33,622.254,659.255,698.456,739.989,783.991,830.609,880,932.328,987.767,1046.5,1108.73,1174.66,1244.51,1318.51,1396.91,1479.98,1567.98,1661.22,1760,1864.66,1975.53,2093,2217.46,2349.32,2489.02,2637.02,2793.83,2959.96,3135.96,3322.44,3520,3729.31,3951.07,4186.01};//钢琴88音符频率

extern float fr(int frnum, float sampleps = float(sampleps0));//音数与频率换算

//频域分析

extern Complex* fourier0(short* data, int samplesize, double freq1,  short frsize = 1, short *freq=NULL);//4倍周期取样



class freqSpectrum{//piano频谱分布类型
private:
    
public:
    double spectrum[ NOTENUM ]={0};//amplitude for each note
    std::vector<short> peaks;//ranging from LOWEST_NOTE=-48 to HIGHEST_NOTE=39, 440Hz=0
    
    double noise;
    double avg;//辐度均值
    double norm;//方均根
    
    short maxnote;//主频率对应音数
    double maxpeak;//主峰值
    
    
    void update(short*, short);
    void peaking(double threshold=0.5);//finding sub peaks
    
    freqSpectrum(): noise(NOISE), avg(0.0), norm(0.0),
        maxnote(LOWEST_NOTE-1),maxpeak(0.0){//initializer
        //for(int i=0; i<NOTENUM; spectrum[i++]=0.0);
        };
    
};

extern double similar(freqSpectrum, freqSpectrum);//频谱相似程度

//时域分析
    
extern int find_beat(short* data, int datasize, int accuracy=500);//求每拍对应采样数
    
#endif
