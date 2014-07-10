//
//  wav.h
//  wavReader
//
//  Created by wyj on 5/2/14.
//  Copyright (c) 2014 user. All rights reserved.
//

#ifndef wavReader_wav_h
#define wavReader_wav_h

#include "types.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <math.h>
using namespace std;
const char df_riffId[4]={'R','I','F','F'};
const char df_riffFmt[4]={'W','A','V','E'};
const char df_wavId[4]={'f','m','t',' '};
const char df_factId[4]={'f','a','c','t'};
const char df_dataId[4]={'d','a','t','a'};
const double PI=3.1415926;
//////////////////////

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

double am(int i)//fade in and fade out
{
    if(i<5000)
    {double k=1-cos(PI*i/5000);
    return k/2;
    }
    else return 1.0;
}
//////////////////////

class wavfile
{
    
    fstream wavin;
    
    riff_Header riff;
    wav_Block wav;
    fact_Header facth;
    short st;//file state:1 read;2 write; 0 null;
    data_Header datah;
    
public:
    void print_info();
    void chgaddr(char addr[]);
    
    void put_data(short *ldata,short *rdata,int bufsize,int);//
    void put_data(int btt,int snum,vector<double> &v_freq,vector<short> &v_tnum,int);
    void get_data(short *ldata,short *rdata,int bufsize,int offs)
    {
        if(st==1)
        {
        int i=0;
        wavin.seekg(offs,ios::beg);
        while(i<bufsize)
        {
            wavin.read((char*)(ldata+i),2);
            wavin.read((char*)(rdata+i),2);
            ++i;
        }
        }
    }
        
    wavfile(char addr[])//read。。。。以后改用继承吧
    {
        wavin.open(addr,ios::binary|ios::in);
        
        
        {
        wavin.read((char*)&riff,sizeof(riff_Header));
        if(*(u32*)riff.riffId!=*(u32*)df_riffId)
        {
            cout<<"not riff file!"<<endl;
            wavin.close();
            st=0;
        }
        else
        {
            wavin.read((char*)&wav,sizeof(wav_Block));
            if(*(u32*)wav.wavId!=*(u32*)df_wavId)
            {
                cout<<"not wav file!"<<endl;
                wavin.close();
                st=0;
            }
            else
            {
                if(wav.wavSize==16)wavin.seekg(-2,ios::cur);
                st=1;
            }
        }
        }
        
    }
    wavfile(char addr[],u32 samplenum, u32 sampleps=44100,u16 bpsample=16,u16 channel=2)//write
    {
        wavin.open(addr,ios::binary|ios::out);
        wav.Fmt.bpsample=bpsample;
        wav.Fmt.sampleps=sampleps;
        wav.Fmt.channel=channel;
        wav.Fmt.blockAlign=bpsample/8*channel;//4
        wav.Fmt.avgBps=sampleps*wav.Fmt.blockAlign;
        wav.wavSize=16;//or 18
        datah.dataSize=samplenum*wav.Fmt.blockAlign;
        riff.riffSize=datah.dataSize+wav.wavSize+20;
        st=2;
        wavin.seekp(0,ios::beg);
        wavin.write((char*)&riff,sizeof(riff_Header));
        wavin.write((char*)&wav,sizeof(wav_Block)-4);
        wavin.write((char*)&datah,sizeof(data_Header));
    }
    
    ~wavfile()
    {
        wavin.close();
    }
};
void wavfile::chgaddr(char addr[])
{
    if(st==1)
    {wavin.close();
        wavin.open(addr,ios::binary);
    }
}
void wavfile::print_info()
{
    if(st)
    {
        cout<<endl<<"head:";
        for(int i=0;i<4;++i)cout<<riff.riffId[i];
        cout<<endl<<"wave:";
        for(int i=0;i<4;++i)cout<<wav.wavId[i];
        cout<<endl<<"size:"<<wav.wavSize<<endl;
        cout<<wav.Fmt.channel<<"channel(s) "<<wav.Fmt.sampleps<<"Hz "<<wav.Fmt.bpsample<<"bits"<<endl;
    }
    else cout<<endl<<"the file is not valid!"<<endl;
}
void wavfile::put_data(short *ldata,short *rdata,int bufsize,int offs=44)
{
    if(st==2)
    {
        wavin.seekp(offs,ios::beg);
        for(int i=0;i<bufsize;++i)
        {
            wavin.write((char*)(ldata+i),2);
            wavin.write((char*)(rdata+i),2);
        }
    }
}
void wavfile::put_data(int btt,int snum,vector<double> &v_freq,vector<short> &v_tnum,int offs=44)
{	
    wavin.seekp(offs,ios::beg);
    for(int i=0;i<snum;++i)
    {
        double freq0=v_freq[i];
        double freq1=2*freq0;
        double freq2=3*freq0;
        double freq3=4*freq0;
        double freq4=5*freq0;
        for(int j=0;j<btt*v_tnum[i];++j)
        {
            short lrdata=am(j)*am(btt*v_tnum[i]-j)*8000*(0.8*sin(freq0*j)+0.5*sin(freq1*j)+0.4*sin(freq2*j)+0.1*sin(freq3*j+0.1*sin(freq4*j)));
            wavin.write((char*)&lrdata,2);
            wavin.write((char*)&lrdata,2);
        }
    }
}

#endif
