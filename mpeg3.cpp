//
//  mpeg3.cpp
//  
//
//  Created by wyj on 7/24/14.
//
//

#include "mpeg3.h"


u32 ID3_Header::realsize(){
    u32 size_temp = (u32)(framesize[0] & 0x7F);
    for(int i=1;i<4;++i){
        size_temp <<= 7;
        size_temp += (framesize[i] & 0x7F);
    }
    return size_temp;
    
}
void ID3_Header::setsize(u32 inputsize){
    for(int i=3;i>=0;--i){
        framesize[i] = (u8)(inputsize & 0x7F);
        inputsize >>= 7;
    }
}

bool mp3In::valid(){//to be continued
    if(*(u32*)id3h.id3Id == *(u32*)df_id3h.id3Id)
       st = true;
    else
       st = false;
    return st;
}

u32 Tag_Header(){
    u32 size_temp = (u32)(framesize[0]);
    for(int i=1;i<4;++i){
        size_temp <<= 8;
        size_temp += framesize[i];
    }
    return size_temp;
}

void Tag_Header::setsize(u32 inputsize){
    for(int i=3;i>=0;--i){
        framesize[i] = (u8)(inputsize & 0xFF);
        inputsize >>= 8;
    }
}

// ****** constructor ******************
mp3In::mp3In(char mp3src[]){
    open(mp3src, std::ios::binary|std::ios::in);
    if(!(*this)){
        if(valid())
            ;
        else{
            close();
            std::cout<<std::endl<<"not a mp3 file!";
        }
    }
    else std::cout<<"the file does not exist!";
    
}

mp3Out::mp3Out(char mp3src[]){
    open(mp3src, std::ios::binary|std::ios::out);
    st = !!(*this);//是否打开正确(fstream已重载!运算符)
    
}
// ************************************

/* /test

int main(int argc, char* argv[]){
    
    mp3File a;
    a.id3h.setsize(255);
    std::cout<< a.id3h.realsize();
    return 0;
}
 
 *//





