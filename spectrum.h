//
//  spectrum.h
//  
//
//  Created by wyj on 7/15/14.
//
//

#ifndef _spectrum_h
#define _spectrum_h

#include "complex.h"
#include <vector>
//#include <iostream>//for test

#define NOTENUM (88) //Piano音符数目
#define LOWEST_NOTE (-48)
#define HIGHEST_NOTE (39)
#define NOISE (100.0)


//频域分析

Complex *fourier0(short *data, int samplesize, double freq1,  short frsize=1, short *freq=NULL){//4倍周期取样
    
    double delta_i=8*PI/freq1/samplesize;//采样间隔
    
    Complex *amp_cp=new Complex[frsize];
    
    double *sin_temp=new double[samplesize];
    double *cos_temp=new double[samplesize];
    short *sampledata=new short[samplesize];
    double amp_cos=0.0,amp_sin=0.0;
    double i0=(delta_i*samplesize/2.0);//from middle
    int i=-(int)i0;
    for(int j=0;j<samplesize;++j){//default, for single tone
        i=(int)(delta_i*j-i0);
        sampledata[j]=*(data+i);
        sin_temp[j]=sin(freq1*i);
        cos_temp[j]=cos(freq1*i);
        amp_cos+=(double)(sampledata[j]*cos_temp[j]);
        amp_sin+=(double)(sampledata[j]*sin_temp[j]);
    }
    amp_cp[0].x=amp_cos/samplesize*2;
    amp_cp[0].y=amp_sin/samplesize*2;
    
    amp_cos=0.0;amp_sin=0.0;
    
    for(i=1;i<frsize;++i){//extend, for overtone
        for(int j=0;j<samplesize;++j){
            amp_cos+=(double)(sampledata[j]*cos_temp[(j*freq[i])%samplesize]);
            amp_sin+=(double)(sampledata[j]*sin_temp[(j*freq[i])%samplesize]);
        }
        amp_cp[i].x=amp_cos/samplesize*2;
        amp_cp[i].y=amp_sin/samplesize*2;
    }
    delete []sin_temp; delete []cos_temp; delete []sampledata;
    return amp_cp;
    
}


class freqSpectrum{//piano频谱分布类型
public:
    double spectrum[ NOTENUM ]={0};//amplitude for each note
    double noise= NOISE;
    std::vector<short> peaks;//ranging from LOWEST_NOTE=-48 to HIGHEST_NOTE=39, 440Hz=0
    
    
    short maxnote= LOWEST_NOTE-1;//主频率对应音数
    double maxpeak= 0.0;//主峰值
    double avg= 0.0;//辐度均值
    double norm=0.0;//方均根
    
    void update(short*,short);
    void peaking(double threshold=0.5);//finding sub peaks
    
};

void freqSpectrum::update(short *data,short samplesize){
    avg= 0.0;
    noise= NOISE;
    maxnote= LOWEST_NOTE-1;
    maxpeak= 0.0;
    norm=0.0;
    
    for(int i=0;i<NOTENUM;++i){
        avg+=(spectrum[i]= fourier0(data, samplesize, fr(i-48))[0].norm());
        norm+=(spectrum[i]*spectrum[i]);
        if(maxpeak<spectrum[i]){maxpeak=spectrum[i];maxnote=i-48;}
        if(noise>spectrum[i]){noise=spectrum[i];}
    }
    avg/=NOTENUM;
    norm=sqrt(norm);
    if(noise<0.5*NOISE){noise=0.4*NOISE;}
    peaking();
}
void freqSpectrum::peaking(double threshold){
    peaks.clear();
    for(short i=2;i<NOTENUM-2;++i){
        if(spectrum[i]>spectrum[i-1]+3*noise && spectrum[i]>spectrum[i+1]+5*noise){
            peaks.push_back(i-48);
        }
        else if(spectrum[i]>(spectrum[i-2]+spectrum[i-1])/2.0+10*noise && spectrum[i]>(spectrum[i+2]+spectrum[i+1])/2.0+10*noise){
            peaks.push_back(i-48);
        }
        
    }
    
}
double similar(freqSpectrum fs1, freqSpectrum fs2){//频谱相似程度
    double s=0.0;
    for(int i=0;i<NOTENUM;++i){
        s+=(fs1.spectrum[i]*fs2.spectrum[i]);
    }
    return s/fs1.norm/fs2.norm;
    
}


//时域分析
int find_beat(short *data, int datasize, int accuracy=500){//求每拍对应采样数
    const int windowsize=1000;
    int samplesize=(datasize-windowsize)/accuracy;
    double *st_amp=new double[samplesize];
    double avg_amp=0.0, min_amp=INFITY, max_amp=0.0;
    
    int i=0;
    for(i=0; i<samplesize;++i){//求短时功率
        st_amp[i]=0.0;
        short st_temp=0;
        for(int j=0;j<windowsize;j=j+2){
            st_temp=*(data+j);
            st_amp[i]+=st_temp*st_temp;
        }
        avg_amp+=(st_amp[i]=sqrt(st_amp[i]/windowsize*2));
        if(st_amp[i]>max_amp)max_amp=st_amp[i];
        if(st_amp[i]<min_amp)min_amp=st_amp[i];
        data+=accuracy;
    }
    avg_amp/=samplesize;//平均功率
    
    double beat_thres=min_amp*2;
    std::vector<float> beatpoint;
    i=0;
    while(sgn(st_amp[i]-=beat_thres)<=0)++i;
    
    short leftpoint=i,rightpoint=i;
    for(++i;i<samplesize;++i){
        st_amp[i]-=beat_thres;
        if(sgn(st_amp[i-1])>0 && sgn(st_amp[i])<=0)
            rightpoint=leftpoint=i-1;
        if(sgn(st_amp[i-1])<=0 && sgn(st_amp[i])>0)
            rightpoint=i;
        if(rightpoint>leftpoint){
            beatpoint.push_back((leftpoint+rightpoint+windowsize/(float)accuracy)/2.0);
            leftpoint=rightpoint;
        }
    }
    std::vector<float>min_beat;
    min_beat.push_back(beatpoint[1]-beatpoint[0]);
    
    for(i= beatpoint.size()-1;i>0;--i){
        beatpoint[i]-=beatpoint[i-1];//add test std::cout<<
        if(min_beat[0]>beatpoint[i])min_beat[0]=beatpoint[i];
    }
    float beatpoint_temp;
    for(i=beatpoint.size()-1;i>0;--i){
        beatpoint_temp=(beatpoint[i]+beatpoint[0])/2.0;//提高精度
        if(beatpoint[i]<1.2*min_beat[0] && beatpoint[i]>min_beat[0]){
            bool f=true;
            int j=0;
            while(j<min_beat.size() && f){//avoid repetition
                if(sgn(min_beat[j++]-beatpoint[i])==0)f=false;
            }
            if(f)min_beat.push_back(beatpoint[i]);
            
            f=true;
            j=0;
            while(j<min_beat.size() && f){//avoid repetition
                if(min_beat[j++]==beatpoint_temp)f=false;
            }
            if(f)min_beat.push_back(beatpoint_temp);
        }
    }
    float *beat_error=new float[min_beat.size()];
    short error_min_i=0;
    for(i=0;i<min_beat.size();++i){
        beat_error[i]=0.0;
        for(int j=1;j<beatpoint.size();++j){
            float error_temp=beatpoint[j]-min_beat[i]*(int)(beatpoint[j]/min_beat[i]);
            beat_error[i]+=(error_temp*2<min_beat[i]?error_temp:min_beat[i]-error_temp);
        }
        if(sgn(beat_error[error_min_i]-beat_error[i])>0){
            error_min_i=i;
        }
    }
    //std::cout<<min_amp<<":"<<beat_error[3]<<":"<<beat_error[error_min_i]<<";";
    
    
    delete[]st_amp;delete[]beat_error;
    
    return (int)(min_beat[error_min_i]*accuracy);
}

#endif
