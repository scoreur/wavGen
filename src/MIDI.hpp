/// @file MIDI.h
///
/// @author WYJ
///
///////////////////

#ifndef __MIDI_
#define __MIDI_
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#define debug(XXX) std::cout<<XXX<<std::endl;
using std::vector;
using std::string;


typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

u32 B2bigEndian(const u8 *length, u16 size = 4){
    u32 tmp = (u32)length[0];
    for(int i=1;i<size;++i){
        tmp = (tmp<<8)+(u32)length[i];
    }
    return tmp;
}

u32 B2varyNum(char* &length){
    u32 tmp = 0x7F & (u32)*(length);
    while(0x80 & (u32)*length)
        tmp = (tmp << 7) + (0x7F & (u32)*(++length));
    ++length;
    return tmp;
}

struct Chunk{
    char type[4];
    u8 length[4];
    char *content;
    
    Chunk():type{0,0,0,0},length{0,0,0,0},
    content(0){}                //for writing
    Chunk(char *in){            //for reading
        memcpy(type, in, 4);
        memcpy(length, (in+4), 4);
        content = in+8;         //shallow copy
    }
    u32 len() const{            //big endian
        return B2bigEndian(length);
    }
    
};

struct ControlChunk{
    u32 deltaTime;
    char type;// <=0x7F, or 0xF0/0xF7
    u32 length;
    char *content;
    ControlChunk(u32 delta, char* &in):
    deltaTime(delta), type(*(in++)),
    content(0){
        
        length = B2varyNum(in);
        if(length>0){
            content = new char[length];
            memcpy(content, in, length);
            in += length;
        }
        //deep copy
        display();
    }
    ~ControlChunk(){
        if(length > 0)
            delete[]content;
    }
    void display(){//debug
        std::cout<<std::dec<<deltaTime<<':'
        <<"type: "<<std::hex<<(u16)(u8)type<<';';
        for(u32 i=0;i<length;++i)
            std::cout<<content[i];
        std::cout<<std::endl;
    }
    
};

struct MidiEvent{
    u32 deltaTime;
    char type;
    u8 param1, param2;
    MidiEvent(u32 delta, char *&in):
    deltaTime(delta){
        if(0x80 & *(u8*)in)
            type = *(in++);
        else
            type = 0;
        param1 = *(u8*)(in++);
        param2 = *(u8*)(in++);
        display();
    }
    ~MidiEvent(){}
    void display(){//debug
        std::cout<<std::dec<<deltaTime<<':'
        <<"type: "<<std::hex<<(u16)(u8)type<<';'
        <<"param1: "<<std::hex<<(u16)param1<<';'
        <<"param2: "<<std::hex<<(u16)param2<<'\n';
        std::cout<<std::endl;
    }
};
/// @brief Midi Header chunck
class MidiHeader:public Chunk{
public:
    MidiHeader(){
        strcpy(type, "MThd");
        
    }
    MidiHeader(char *in):Chunk(in){
        debug("Header created!")
        fmt = (Format)B2bigEndian((u8*)content,2);
        nTracks = B2bigEndian((u8*)content+2,2);
        division = B2bigEndian((u8*)content+4,2);
    }
    ~MidiHeader(){}
    
    enum Format{
        single = 0,
        multi_sync,
        multi_independent
    } fmt;
    friend class MidiFileIn;
private:
    u32 nTracks;
    u32 division;
    
    
};

/// @brief Midi Track Chunk
class MidiTrack:public Chunk{
public:
    MidiTrack(){
        strcpy(type, "MTrk");

    }
    MidiTrack(char *in):Chunk(in){
        debug("Track created!")
        in += 8;
        char *beg = in;
        u32 l = len();
        while(in - beg < l){
            u32 delta = B2varyNum(in);
            switch(*(u8*)in){
                case 0xFF:
                    ++in;
                    cEvts.push_back(new ControlChunk(delta, in));
                    break;
                case 0xF0: case 0xF7:
                    cEvts.push_back(new ControlChunk(delta, in));
                    break;
                default:
                    mEvts.push_back(new MidiEvent(delta, in));
                    break;
            }
        }
        debug("Track: "<< mEvts.size())
    }
    ~MidiTrack(){
        for(vector<MidiEvent*>::iterator iter=mEvts.begin();
            iter != mEvts.end(); ++iter)
            delete *iter;
        for(vector<ControlChunk*>::iterator iter=cEvts.begin();
             iter != cEvts.end(); ++iter)
            delete *iter;
        debug("delete Tracks!")
    }
    
private:
    vector<MidiEvent*> mEvts;//midi events
    vector<ControlChunk*> cEvts;//control events
    
};

/// @brief Raw Midi data
class RawFile{
public:
    RawFile(char *fileName):buffer(0),length(0){
        std::ifstream in(fileName);
        if(!in){
            std::cout<<"file not valid";
            return;
        }
        in.seekg(0, std::ios::beg);
        u32 beg = in.tellg();
        in.seekg(0, std::ios::end);
        length = in.tellg();
        length -= beg;
        buffer = new char[length];
        in.seekg(0, std::ios::beg);
        in.read(buffer, length);
    }
    ~RawFile(){
        if(length>0)
            delete[]buffer;
        debug("delete raw file!")
        
    }
    u32 len() const{
        return length;
    }
    char *operator()(u32 i=0)const{
        return buffer+i;
    }
    
private:
    char *buffer;
    u32 length;
    
};

/// @brief Input Midi File
class MidiFileIn{
public:
    MidiFileIn(char *fileName):
    raw(fileName){
        construct();
    }
    ~MidiFileIn(){
        delete Header;
        for(vector<MidiTrack*>::iterator iter = tracks.begin();
            iter != tracks.end(); ++iter)
            delete *iter;
        debug("delete chunks!")
    }
    
    MidiHeader & header() const{
        return *Header;
    }
    RawFile raw;
private:
    void construct(){
        u32 ptr = 0;
        Header = new MidiHeader(raw(ptr));
        ptr += (Header->len()+8);
        
        for(u32 i=0;i<Header->nTracks;++i){
            tracks.push_back(new MidiTrack(raw(ptr)));
            ptr += (tracks[i]->len()+8);
        }
        
        
    }
    
    MidiHeader *Header;
    vector<MidiTrack*>tracks;

    
};

//test
/*
int main(int argc, char *argv[]){
    
    if(argc==1)
        return 1;
    MidiFileIn midiIn(argv[1]);
    debug("raw file length: "<<midiIn.raw.len())
    debug("header length: "<<(midiIn.header().len()))
    return 0;
}
*/

#endif /* __MIDI_ */