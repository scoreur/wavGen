//
//  wav.h
//  waveur
//
//  Created by wyj on 7/23/14.
//  Copyright (c) 2014 scoreur. All rights reserved.
//

#ifndef wavReader_wav_h
#define wavReader_wav_h

#include "types.h"
#include "spectrum.h"
#include <cmath>
#include <fstream>
#include <iostream>

// ***********************************************************
//wav文件头常量
const char df_riffId[4]={'R','I','F','F'};
const char df_riffFmt[4]={'W','A','V','E'};
const char df_wavId[4]={'f','m','t',' '};
const char df_factId[4]={'f','a','c','t'};
const char df_dataId[4]={'d','a','t','a'};
//
struct riff_Header
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
// ***********************************************************


const int offs0=sizeof(riff_Header)+sizeof(wav_Block)-4+sizeof(data_Header);//44

extern double amp_fade(int i, int fade_range=4000);//fade-in and fade-out effect


//

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
    
    WavFile():st(false){}
    ~WavFile()
    {
        if(st)wavstream.close();
    }
};

class WavIn: public WavFile{
 public:
    void get_data(short *ldata,short *rdata,int bufsize,int offs);
    int wav2score(char scoresrc[]);
    
    WavIn(char src[]);//read
};

class WavOut: public WavFile{
 public:
    void put_data(short *ldata,short *rdata,int bufsize,int offs);//
    void put_data(int btt,int snum,double *freq,short tnum[],int offs);
    int score2wav(char scoresrc[]);
    void chgsrc(char src[]);

    WavOut(char src[], u32 sampleps=sampleps0,u16 bpsample=16,u16 channel=2);
};

#endif

