//
//  mpeg3.h
//  
//
//  Created by user on 7/24/14.
//
//

#ifndef ____mpeg3__
#define ____mpeg3__

#include "types.h"
#include <iostream>
#include <fstream>

const int samplepf = 1152;//每frame采样数


struct ID3_Header{
    u8 id3Id[3];//={'I', 'D', '3'};
    u8 version;//= 0x3;
    u8 revision;//= 0x0;
    u8 flag;//= 0x0;
    u8 framesize[4];//highest bits in each byte abandoned
    
    u32 realsize();//calculated from framesize[4]
    void setsize(u32);
    
};//sizefo(ID3_Header) == 10
const ID3_Header df_id3h = {{'I', 'D', '3'}, 0x3, 0x0, 0x0, {0x0,0x0,0x0,0xA}};

struct Tag_Header{
    u8 tagId[4];//将用常量数组存储各种帧标志
    u8 framesize[4];//Big endian, header not included
    u8 flag[2];//
    
    u32 realsize();
    void setsize(u32);
};

struct Frame_Header{//4 bytes,按位域存储
    u32 sync1:8;//0xFF
    //1st byte
    
    u32 sync2:3;//0x7
    u32 version:2;//0x3(binary 11),MPEG 1
    u32 layer:2;//0x1(binary 01),Layer 3
    u32 crc:1;//决定帧头后是否有2bytes校验: 0x1不校验
    //2nd byte
    
    u32 bps_index:4;//位率索引,0x9为128kpbs
    u32 sampleps_index:2;//采样率索引,00为44.1kHz
    u32 padding:1;//0无需调节帧长
    u32 extension:1;//未使用
    
    //3rd byte
    
    u32 channel:2;//00stereo
    u32 mode_extension:2;//for channel 01
    u32 copyright:1;//0无版权
    u32 original:1;//1原版
    u32 emphasis:2;//00未定义
    
    //4th byte

    Frame_Header():sync1(0xFF), sync2(7), version(3), layer(1){}
};

struct ID3_Tail{//128Bytes
    u8 tag[3];//="TAG"
    u8 title[30];
    u8 author[30];
    u8 album[30];
    u8 year[4];
    u8 comment[28];//附注
    u8 reserve;//保留位
    u8 tracknum;//音轨号
    u8 genrenum;//147种流派，用常量数组存储，其余为unknown
};

class mp3File: public std::fstream{
public:
    ID3_Header id3h;
    
    bool st;
    
    mp3File(): st(false), id3h(df_id3h){
        
    }
    ~mp3File(){
        if(st)
            close();
    }
};

class mp3In: public mp3File{
public:
    
    bool valid();
    inline mp3In(char mp3src[]);
};

class mp3Out: public mp3File{
public:
    
    inline mp3Out(char mp3src[]);
};
#endif /* defined(____mpeg3__) */
