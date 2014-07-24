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
    u8 tagId[4];//
    u8 framesize[4];//Big endian, header not included
    u8 flag[2];//
    
    u32 realsize();
    void setsize(u32);
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
