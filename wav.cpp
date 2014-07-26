//
//  wav.cpp
//  
//
//  Created by wyj on 7/23/14.
//
//  7/25/2014升级曲谱结构V2.0，类似MID
//

#include "wav.h"
#include "score.h"
#include <cmath>

// ****** constructor ******************************
WavIn::WavIn(char src[])//read
{
    open(src,std::ios::binary|std::ios::in);
    
    read((char*)&riff,sizeof(riff_Header));
    if(*(u32*)riff.riffId!=*(u32*)df_riffId)
    {
        std::cout<<"not riff file!"<<std::endl;
        close();
        st=false;
    }
    else
    {
        read((char*)&wav,sizeof(wav_Block));
        if(*(u32*)wav.wavId!=*(u32*)df_wavId)
        {
            std::cout<<"not wav file!"<<std::endl;
            close();
            st=false;
        }
        else
        {
            if(wav.wavSize==16)seekg(-2,std::ios::cur);
            st=true;
        }
    }
}

WavOut::WavOut(char src[], u32 sampleps, u16 bpsample, u16 channel)
{
    open(src,std::ios::binary|std::ios::out);
    st=true;
    
    wav.Fmt.bpsample = bpsample;
    wav.Fmt.sampleps = sampleps;
    wav.Fmt.channel = channel;
    wav.Fmt.blockAlign = bpsample/8*channel;//4
    wav.Fmt.avgBps = sampleps*wav.Fmt.blockAlign;
    wav.wavSize = 16;//or 18
    datah.dataSize = 0;//change when appending
    riff.riffSize = datah.dataSize+wav.wavSize+20;//change when appending
    
    seekp(0,std::ios::beg);
    write((char*)&riff,sizeof(riff_Header));
    write((char*)&wav,sizeof(wav_Block)-4);
    write((char*)&datah,sizeof(data_Header));
}
// ************************************************


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
void WavIn::get_data(short *ldata, short *rdata, int bufsize, int offs)//copy directly
{
    if(st)
    {
        
        seekg(offs,std::ios::beg);
        int i = 0;
        while(i<bufsize)
        {
            read((char*)(ldata+i),2);//left channel
            read((char*)(rdata+i),2);//right channel
            ++i;
        }
    }
}

void WavOut::put_data(short *ldata, short *rdata, int bufsize, int offs = offs0)
{
    seekp(offs,std::ios::beg);
    for(int i=0;i<bufsize;++i)
    {
        write((char*)(ldata+i),2);
        write((char*)(rdata+i),2);
    }
    
}

void WavOut::put_data(int btt, int snum, double *freq, short tnum[], int offs = offs0)
{
    int * bufsize = new int[snum];

    for(int i=0;i<snum;++i)
    {
        double freq0 = *(freq+i);
        double freq1 = 2*freq0;//泛音
        double freq2 = 3*freq0;
        double freq3 = 4*freq0;
        
        
        bufsize[i] = btt*tnum[i];
        short *ldata = new short[bufsize[i]];
        short *rdata = new short[bufsize[i]];
        for(int j=0;j<bufsize[i];++j)
        {
            short data_temp = amp(j,bufsize[i])*8000;
            ldata[j] = data_temp * (0.8*sin(freq0*j)+0.4*cos(freq1*j)+0.3*sin(freq2*j)+0.1*cos(freq3*j));//left channel
            rdata[j] = data_temp * (0.8*cos(freq0*j)+0.4*sin(freq1*j)+0.3*cos(freq2*j)+0.1*sin(freq3*j));//right channel
        }
        
        put_data(ldata, rdata, bufsize[i], offs);
        delete []ldata; delete []rdata;
        offs += (bufsize[i] * 4);
    }
    delete []bufsize;
}
int WavOut::score2wav(char scoresrc[]){//scoreV1.1
    short snum;
    short tlen;
    short beatspm;
    std::fstream scorefile;
    scorefile.open(scoresrc,std::ios::in);
    char sv[10];
    scorefile>>sv;
    scorefile>>tlen>>snum>>beatspm;
    int samplepb=sampleps0*60.0/beatspm;//每拍采样数
    short *frnum = new short[snum];
    short *tnum = new short[snum];
    double *freq=new double[snum];
    
    for(int i=0;i<snum;++i)
    {
        scorefile>>frnum[i]>>tnum[i];
        freq[i]=fr(frnum[i]-LOWEST_NOTE);
    }
    scorefile.close();
    
    datah.dataSize=tlen*samplepb*wav.Fmt.blockAlign;//change when appending
    riff.riffSize=datah.dataSize+wav.wavSize+20;//change when appending
    seekp(0,std::ios::beg);
    write((char*)&riff,sizeof(riff_Header));
    write((char*)&wav,sizeof(wav_Block)-4);
    write((char*)&datah,sizeof(data_Header));
    put_data(samplepb,snum,freq,tnum);
    
    delete []frnum;delete []tnum; delete []freq;
    
    std::cout<<std::endl<<sv<<" Duration:"<<tlen*60.0/beatspm<<"s; Beats: "<<snum;
    print_info();
    return 1;
    
}

int WavOut::score2wav(midSeq & mids){//scoreV2.0
    

    short * lrdata;
    datah.dataSize = mid2data(lrdata, mids, sampleps0) * wav.Fmt.blockAlign;//change when appending
    riff.riffSize = datah.dataSize+wav.wavSize+20;//change when appending
    seekp(0,std::ios::beg);
    write((char*)&riff,sizeof(riff_Header));
    write((char*)&wav,sizeof(wav_Block)-4);
    write((char*)&datah,sizeof(data_Header));
    
    put_data(lrdata, lrdata, datah.dataSize/wav.Fmt.blockAlign);
    delete []lrdata;
    
    std::cout<<std::endl<<"duration:"<<mids.dura()/1000.0<<"s; tone number:"<<mids.tone_num();
    return 1;
}


int WavIn::wav2score(char scoresrc[]){//to be continued
    
    
    short ldata[160000];
    short rdata[160000];
    get_data(ldata,rdata, 160000,44);
    std::cout<<"beats per minute: "<<60.0/(find_beat(ldata, 160000,200)/(float)sampleps0);
    
    /*
     std::fstream scorefile;
     scorefile.open(scoresrc,std::ios::out);
     
     scorefile.close();
     */
    
    
    /*
     freqSpectrum sp;
     for(int k=0;k<20;++k){
     wavin.get_data(ldata,rdata, 16000,16000*k+6044);
     sp.update(rdata+4000,64);
     std::cout<<sp.peaks.size()<<","<<sp.maxpeak<<","<<","<<sp.avg<<","<<sp.maxnote<<","<<sp.norm<<std::endl;
     }*/
    
    return 1;
}
void WavOut::chgsrc(char src[]){//change to a new file
    if(st)
    {
        close();
        open(src,std::ios::binary|std::ios::out);
        
        datah.dataSize=0;//change when appending
        riff.riffSize = datah.dataSize+wav.wavSize+20;//change when appending
        seekp(0,std::ios::beg);
        write((char*)&riff,sizeof(riff_Header));
        write((char*)&wav,sizeof(wav_Block)-4);
        write((char*)&datah,sizeof(data_Header));
        
    }
}


