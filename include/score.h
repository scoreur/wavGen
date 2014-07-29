//
//  score.h
//  
//
//  Created by wyj on 7/26/14.
//
//

#ifndef ____score__
#define ____score__


#include <vector>
#include "types.h"


namespace MID{
    enum EVENT{
        TONE_begin = 0,     //0音符开始
        TONE_end,           //1音符结束
        TONE_force,         //2音符力度设置（有些乐器可在音符持续过程中变化力度,有些不行）
        TONE_instrument,    //3乐器设置
        
        TEMPO,              //4速度，in beats-per-minute
        BEAT                //5节拍
        
    };
    enum INSTRUMENT{        //乐器类型待扩展
        Flute = 0,
        Piano
        
        
    };
    enum BEATS{
        b24 = 0,            //2/4拍，以此类推
        b34,
        b44,
        b38,
        b68,
        b12,
        b22,
        b32
    };
}
extern float amp_fade(int i, int fade_range);//fade-in and fade-out effect
extern float amp_fade2(int i, int fade_range = 3500);
extern float amp(u32 i, u32 range, MID::INSTRUMENT instru = MID::Flute);
//各种乐器的波形包络

struct midEvent{            //参考MID格式
    u16 time_diff;          //与上一音符的差，以毫秒为单位
    MID::EVENT flag;        //事件类型标记
    u16 argu;               //事件参数
    
    midEvent(short t, MID::EVENT f, short a): time_diff(t), flag(f), argu(a){}
    ~midEvent(){}
};


struct toneState{
    u16 pitch;                      //0音符表示关闭（或称为何止符）
    u16 force;                      //当前力度，范围待定
    MID::INSTRUMENT instrument;     //当前乐器
    u16 tempo;                      //当前速度，in beats-per-minute
    MID::BEATS beat;                //当前节拍
    //状态量

    void print_info() const;
    bool add(midEvent);             //返回当前音符状态是否为打开
    
};
const toneState df_tone = {0, 100, MID::Flute, 96, MID::b44};

class midSeq{
private:
    toneState tone;
    u32 tone_total;                 //音符总数,即midEvent::TONE_begin计数
    u32 duration;                   //总持续时间，毫秒
    //统计量
    std::vector<midEvent> events;   //事件序列
public:
    
    bool isOn(u32 num) const;      //从前往后第num个事件执行后是否有音符打开
    bool isOn() const;             //返回当前音符是否打开;
    u32 tone_num(u32 num) const;   //从前往后num毫秒后音符总数，即midEvent::TONE_begin个数
    u32 tone_num() const;          //返回当前音符总数
 
    void add(midEvent);
    
    u32 dura() const;                       //总持续时间
    u16 dura_f(u32 num = 0) const;          //从前往后第num个音符持续时间，默认为首音符num=0
    u16 dura_b(u32 num = 0) const;          //从后往前第num个音符持续时间，默认为末音符num=0
    
    void score2mid(char scoresrc[]);
    midSeq():tone(df_tone), tone_total(0), duration(0){}
    ~midSeq(){}
    
    friend u32 mid2data(short* &data, midSeq & mids, int sampleps);//由通道数据生成pcm数据,返回采样点数
    friend u32 mid2data(short* &data, std::vector<midEvent> &midv, int sampleps);
};
#endif /* defined(____score__) */
