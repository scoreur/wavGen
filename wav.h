//
//  wav.h
//  waveur
//
//  Created by wyj on 7/9/14.
//  Copyright (c) 2014 scoreur. All rights reserved.
//

#ifndef wavReader_wav_h
#define wavReader_wav_h

#include "types.h"
#include "complex.h"
#include <fstream>
#include <iostream>

//default
const char df_riffId[4]={'R','I','F','F'};
const char df_riffFmt[4]={'W','A','V','E'};
const char df_wavId[4]={'f','m','t',' '};
const char df_factId[4]={'f','a','c','t'};
const char df_dataId[4]={'d','a','t','a'};
const int sampleps0=44100;//默认采样数
const double fr0= 440*2*PI/sampleps0;//标准音（圆）频率除以采样数
//

struct riff_Header  //
{
    char riffId[4]={'R','I','F','F'};
    u32 riffSize;//16 or 18
    char riffFmt[4]={'W','A','V','E'};
};

struct wav_Fmt
{
    u16 wavTag=1;//0x0001 PCM16b;0x0003 32b
    u16 channel;//0100 single,0200 stero
    u32 sampleps;//44AC ＝44100Hz
    u32 avgBps;
    u16 blockAlign=4;
    u16 bpsample;//8bits or 16bits
    u16 addpack=0;
};

struct wav_Block
{
    char wavId[4]={'f','m','t',' '};
    u32 wavSize;
    struct wav_Fmt Fmt;
};

struct fact_Header
{
    char factId[4]={'f','a','c','t'};
    u32 factSize;
};

struct data_Header
{
    char dataId[4]={'d','a','t','a'};
    u32 dataSize;
};

const int offs0=sizeof(riff_Header)+sizeof(wav_Block)-4+sizeof(data_Header);//44

double am(int i)//fade in and fade out
{
    if(i<5000)
    {double k=1-cos(PI*i/5000);
    return k/2;
    }
    else return 1.0;
}

double fr(int frnum)//音数与频率换算
{
    return fr0*pow(2,frnum/12.0);
}


//////////////////////

class WavFile
{
    
 public:
    
    std::fstream wavstream;
    
    riff_Header riff;
    wav_Block wav;
    fact_Header facth;
    data_Header datah;
    
    bool st;//true : success; false: failed
    
    void print_info();
    
    WavFile(){
        st=false;
    }
    
    ~WavFile()
    {
        if(st)wavstream.close();
    }
};

class WavIn: public WavFile{
 public:
    void get_data(short *ldata,short *rdata,int bufsize,int offs);
    WavIn(char src[])//read
    {
        wavstream.open(src,std::ios::binary|std::ios::in);
        
        wavstream.read((char*)&riff,sizeof(riff_Header));
        if(*(u32*)riff.riffId!=*(u32*)df_riffId)
        {
            std::cout<<"not riff file!"<<std::endl;
            wavstream.close();
            st=false;
        }
        else
        {
            wavstream.read((char*)&wav,sizeof(wav_Block));
            if(*(u32*)wav.wavId!=*(u32*)df_wavId)
            {
                std::cout<<"not wav file!"<<std::endl;
                wavstream.close();
                st=false;
            }
            else
            {
                if(wav.wavSize==16)wavstream.seekg(-2,std::ios::cur);
                st=true;
            }
        }
        
    }

};
class WavOut: public WavFile{
 public:
    void put_data(short *ldata,short *rdata,int bufsize,int offs);//
    void put_data(int btt,int snum,double *freq,short tnum[],int offs);
    void score2wav(char scoresrc[]);
    void wav2score(char scoresrc[]);
    void chgsrc(char src[]);

    WavOut(char src[], u32 sampleps=sampleps0,u16 bpsample=16,u16 channel=2)
    {
        wavstream.open(src,std::ios::binary|std::ios::out);
        st=true;
    
        wav.Fmt.bpsample=bpsample;
        wav.Fmt.sampleps=sampleps;
        wav.Fmt.channel=channel;
        wav.Fmt.blockAlign=bpsample/8*channel;//4
        wav.Fmt.avgBps=sampleps*wav.Fmt.blockAlign;
        wav.wavSize=16;//or 18
        datah.dataSize=0;//change when appending
        riff.riffSize=datah.dataSize+wav.wavSize+20;//change when appending
        
        wavstream.seekp(0,std::ios::beg);
        wavstream.write((char*)&riff,sizeof(riff_Header));
        wavstream.write((char*)&wav,sizeof(wav_Block)-4);
        wavstream.write((char*)&datah,sizeof(data_Header));
    }

};

void WavFile::print_info()
{
    if(st)
    {
        std::cout<<std::endl<<"head:";
        for(int i=0;i<4;++i)std::cout<<riff.riffId[i];
        std::cout<<std::endl<<"wave:";
        for(int i=0;i<4;++i)std::cout<<wav.wavId[i];
        std::cout<<std::endl<<"size:"<<wav.wavSize<<std::endl;
        std::cout<<wav.Fmt.channel<<"channel(s) "<<wav.Fmt.sampleps<<"Hz "<<wav.Fmt.bpsample<<"bits"<<std::endl;
    }
    else std::cout<<std::endl<<"the file is not valid!"<<std::endl;
}
void WavIn::get_data(short *ldata,short *rdata,int bufsize,int offs=offs0)//copy directly
{
    if(st)
    {
        int i=0;
        wavstream.seekg(offs,std::ios::beg);
        while(i<bufsize)
        {
            wavstream.read((char*)(ldata+i),2);//left channel
            wavstream.read((char*)(rdata+i),2);//right channel
            ++i;
        }
    }
}

void WavOut::put_data(short *ldata,short *rdata,int bufsize,int offs=offs0)
{
    wavstream.seekp(offs,std::ios::beg);
    for(int i=0;i<bufsize;++i)
    {
        wavstream.write((char*)(ldata+i),2);
        wavstream.write((char*)(rdata+i),2);
    }

}
void WavOut::put_data(int btt,int snum,double *freq,short tnum[],int offs=offs0)
{
    wavstream.seekp(offs,std::ios::beg);
    for(int i=0;i<snum;++i)
    {
        double freq0=*(freq+i);
        double freq1=2*freq0;
        double freq2=3*freq0;
        double freq3=4*freq0;
        double freq4=5*freq0;
        for(int j=0;j<btt*tnum[i];++j)
        {
            short lrdata=am(j)*am(btt*tnum[i]-j)*8000*(0.8*sin(freq0*j)+0.5*sin(freq1*j)+0.4*sin(freq2*j)+0.1*sin(freq3*j+0.1*sin(freq4*j)));
            wavstream.write((char*)&lrdata,2);
            wavstream.write((char*)&lrdata,2);
        }
    }
}
void WavOut::score2wav(char scoresrc[]){
    short snum;
    short tlen;
    short beatspm;
    std::fstream scorefile;
    scorefile.open(scoresrc,std::ios::in);
    scorefile>>tlen>>snum>>beatspm;
    int samplepb=sampleps0*60.0/beatspm;//每拍采样数
    short *frnum = new short[snum];
    short *tnum = new short[snum];
    double *freq=new double[snum];
    for(int i=0;i<snum;++i)
    {
        scorefile>>frnum[i]>>tnum[i];
        freq[i]=fr(frnum[i]);
    }
    scorefile.close();
    
    datah.dataSize=tlen*samplepb*wav.Fmt.blockAlign;//change when appending
    riff.riffSize=datah.dataSize+wav.wavSize+20;//change when appending
    wavstream.seekp(0,std::ios::beg);
    wavstream.write((char*)&riff,sizeof(riff_Header));
    wavstream.write((char*)&wav,sizeof(wav_Block)-4);
    wavstream.write((char*)&datah,sizeof(data_Header));
    put_data(samplepb,snum,freq,tnum);
    
    delete []frnum;delete []tnum; delete []freq;

    std::cout<<"score to wav successful! Duration:"<<tlen*60.0/beatspm<<"s; Beats: "<<snum;
    print_info();
    
}
void wav2score(char scoresrc[]){
    
}
void WavOut::chgsrc(char src[])
{
    if(st)
    {wavstream.close();
        wavstream.open(src,std::ios::binary);
        
        datah.dataSize=0;//change when appending
        riff.riffSize=datah.dataSize+wav.wavSize+20;//change when appending
        wavstream.seekp(0,std::ios::beg);
        wavstream.write((char*)&riff,sizeof(riff_Header));
        wavstream.write((char*)&wav,sizeof(wav_Block)-4);
        wavstream.write((char*)&datah,sizeof(data_Header));

    }
}
#endif
