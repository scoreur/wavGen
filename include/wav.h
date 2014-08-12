///////////////////////////////////////////////////////////////////////////
/// @file       wav.h
/// @brief      Wave File Processing.
///
/// read and write wave file, provide score-to-wavefile
/// and wavefile-to-score function.\n
/// -update history:
///   -# July 23:\n
///      overload WavOut::score2wav() to generate wave file
///      from mid-like sequence.\n
///   -# Aug 5:\n
///      reformat notes in Doxygen style.\n
///
/// @author     wyj, [other authors], https://github.com/scoreur/wavGen
/// @version    2.0
/// @date       7/23/2014.
/// @copyright(c) 2014 Scoreur. All rights reserved.
///////////////////////////////////////////////////////////////////////////

#ifndef wavReader_wav_h
#define wavReader_wav_h

#include "types.h"
#include "spectrum.h"
#include "score.h"
#include <fstream>
#include <iostream>

/// @namespace  WAV
/// @brief      define default parameters in wave file
namespace WAV {
    const char riffId[4]={'R','I','F','F'}; ///< Resource Interchanging File Format
    const char riffFmt[4]={'W','A','V','E'};///< wav Format
    const char wavId[4]={'f','m','t',' '};  ///< Format flag
    const char factId[4]={'f','a','c','t'}; ///< fact block header flag
    const char dataId[4]={'d','a','t','a'}; ///< data block header flag
}

/// @brief      Mark file type and length
struct riff_Header
{
    char riffId[4]; ///< RIFF flag
    u32 riffSize;   ///< number of bytes from next byte to the EOF, >=36
    char riffFmt[4];///< WAVE flag
    
    riff_Header():riffSize(36){
        *(u32*)riffId = *(u32*)WAV::riffId;
        *(u32*)riffFmt = *(u32*)WAV::riffFmt;
    }
};

/// @brief      Mark wave format parameters
struct wav_Fmt
{
    u16 wavTag;     ///< 0x0001 for PCM(16bits), 0x0003 for PCM(32bits)
    u16 channel;    ///< 0x0001 for single, 0x0002 for stero
    u32 sampleps;   ///< sample per second: 0xAC44=44100Hz(little endian)
    u32 avgBps;     ///< average bytes per second
    u16 blockAlign; ///< alignment for data block, default 4 = 2bytes * 2;
    u16 bpsample;   ///< bits per sample, 8 or 16
    u16 addpack;    ///< adding pack, exists when wavSize = 18
    
    wav_Fmt():wavTag(0x1), channel(0x2),
        sampleps(0xac44), avgBps(0x2b110),
        blockAlign(0x4), bpsample(0x10),
        addpack(0x0){;}
};

/// @brief      Wave format block
struct wav_Block
{
    char wavId[4];      ///< the start of format parameters description
    u32 wavSize;        ///< the length of format block form next byte, 16 or 18
    struct wav_Fmt Fmt;
    
    wav_Block(): wavSize(16){
        *(u32*)wavId = *(u32*)WAV::wavId;
    }
};

/// @brief      Fact block header [optional], unused in non-compressed PCM
struct fact_Header
{
    char factId[4];     ///< the start of fact block
    u32 factSize;       ///< length of fact block
    
    fact_Header(): factSize(0){
        *(u32*)factId = *(u32*)WAV::factId;
    }
};

/// @brief      Data block header, mark the length of sound data
struct data_Header
{
    char dataId[4];     ///< start of data block
    u32 dataSize;       ///< number of bytes from next byte to the EOF
    
    data_Header():dataSize(0){
        *(u32*)dataId = *(u32*)WAV::dataId;
    }
};

/// @brief      The default position where sound data start
const int offs0=sizeof(riff_Header)+sizeof(wav_Block)-4+sizeof(data_Header);//44


///////////////////////////////////////////////////////////////////////////
/// @class      WavFile
/// @brief      base class for reading/writing wave file, inherit from fstream
/// @author     wyj
/// @note
///////////////////////////////////////////////////////////////////////////
class WavFile: public std::fstream{
 protected:
    
    riff_Header riff;
    wav_Block wav;
    fact_Header facth;
    data_Header datah;
    
    bool st;    ///< state of file: true -> successful; false -> failed
 
public:
    
/// @brief      Output description of file format parameters
    void print_info();
    
    WavFile(): st(false){;}
    ~WavFile(){
        if(st) close();
    }
};


///////////////////////////////////////////////////////////////////////////
/// @class      WavIn
/// @brief      derived class of WavFile for reading wave file
/// @author     wyj
/// @note
///////////////////////////////////////////////////////////////////////////
class WavIn: public WavFile{
 public:
    
/// @brief      get 16bits sound data from wave file
///     @param [out] ldata:  pointer to left channel sound volume sequence
///     @param [out] rdata:  pointer to right channel sound volume sequence
///     @param [in] bufsize:  number of samples to get
///     @param [in] offs:  offset position to start retrieving data
    void get_data(short *ldata, short *rdata, int bufsize, int offs = offs0);
    
    
    int wav2score(char scoresrc[]);
    
    WavIn(char src[]);//read
};


///////////////////////////////////////////////////////////////////////////
/// @class      WavOut
/// @brief      derived class of WavFile for writing wave file
/// @author     wyj
/// @note
///////////////////////////////////////////////////////////////////////////
class WavOut: public WavFile{
 public:
    
/// @brief      put 16bits sound data into wave file at certain position
///     @param [in] ldata:  pointer to left channel sound volume sequence
///     @param [in] rdata:  pointer to right channel sound volume sequence
///     @param [in] bufsize:  number of samples to put
///     @param [in] offs:  offset position to start putting data
    void put_data(short *ldata, short *rdata, int bufsize, int offs = offs0);
    
/// @brief      put
///     @param [in]
///     @param [in]
///     @param [in]
    void put_data(int btt, int snum, double *freq, short *tnum, int offs = offs0);
    
/// @brief      convert score data to wave data
///     @param [in] scoresrc: the path of score file
/// @return int whether the converting is successful, set 1 if yes
    int score2wav(char scoresrc[]);//scoreV1.1;
    
    
    int score2wav(midSeq & mids);//用类似MID格式的输入, scoreV2.0;
    
    
    void chgsrc(char src[]);

    WavOut(char src[], u32 sampleps=sampleps0, u16 bpsample=16, u16 channel=2);
};

#endif

