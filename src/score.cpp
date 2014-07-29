//
//  score.cpp
//  
//
//  Created by wyj on 7/26/14.
//
//

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "../include/spectrum.h"
#include "../include/score.h"

float amp_fade(int i, int fade_range)//fade-in and fade-out effect
{
    if(i<fade_range){
        float k=1-cos(i*PI/fade_range);
        return k/2;
    }
    else return 1.0 - log(i-fade_range+1.0) * 0.04 ;
}

float amp_fade2(int i, int fade_range)
{
    if(i<fade_range){
        float k=1-cos(i*PI/fade_range);
        return k/2*(0.5+0.5*i/fade_range);
    }
    else return 1.0;
}

float amp(u32 i, u32 range, MID::INSTRUMENT instru){
    switch(instru){
        case MID::Flute:
            return amp_fade(i, 3500)*amp_fade2(range - i); break;
        case MID::Piano:
            return amp_fade2(i)*amp_fade2(range - i); break;
            
    }
    
}

bool midSeq::isOn() const{
    return (bool)tone.pitch;
}
bool midSeq::isOn(u32 num) const{
    int end_temp = events.size();
    for(int i = num;i < end_temp; ++i){
        if(events[i].flag == MID::TONE_begin)//检测到有音符按下
            return true;
        else if(events[i].flag == MID::TONE_end){//检测到有音符抬起，还需确认
            while(++i < end_temp && events[i].time_diff == 0)//同时发生的事件
                if(events[i].flag == MID::TONE_begin) //同时有音符按下
                    return true;
            return false;//同时无音符按下
        }
    }
    return false;//未检测到音符事件；
}


u32 midSeq::tone_num() const{
    return tone_total;
}
u32 midSeq::tone_num(u32 num) const{
    if(num >= duration) return tone_total;//达到或超出总时长
    
    int i = 0;
    u32 tone_temp = events[i].time_diff;
    u32 num_temp = 0;
    //初始化
    
    while(tone_temp < num){//已保证num < duration，故i < events.size()-1
        if(events[i].flag == MID::TONE_begin) ++num_temp;
        tone_temp += events[++i].time_diff;
    }
    if(tone_temp == num) while(events[i].time_diff == 0)//判断同时事件
        if(events[i++].flag == MID::TONE_begin) ++num_temp;
    
    return num_temp;
}

bool toneState::add(midEvent midev){
    switch(midev.flag){
        case MID::TONE_begin:
            pitch = midev.argu; break;
        case MID::TONE_end:
            pitch = 0; break;
        case MID::TONE_force:
            force = midev.argu; break;
        case MID::TONE_instrument:
            instrument = (MID::INSTRUMENT)midev.argu; break;
        case MID::TEMPO:
            tempo = midev.argu; break;
        case MID::BEAT:
            beat = (MID::BEATS)midev.argu; break;
    
            
    }
    return (bool)pitch;
}
void midSeq::add(midEvent midev){
    events.push_back(midev);
    duration += midev.time_diff;
    switch(midev.flag){
        case MID::TONE_begin:
            tone.pitch = midev.argu; ++tone_total; break;
        case MID::TONE_end:
            tone.pitch = 0; break;
        case MID::TONE_force:
            tone.force = midev.argu; break;
        case MID::TONE_instrument:
            tone.instrument = (MID::INSTRUMENT)midev.argu; break;
        case MID::TEMPO:
            tone.tempo = midev.argu; break;
        case MID::BEAT:
            tone.beat = (MID::BEATS)midev.argu; break;
            
            
    }
    
}

u32 midSeq::dura() const{
    return duration;
}
u16 midSeq::dura_f(u32 num) const{
    if(num >= tone_total) return 0;
    
    int i=0;
    while(num>0) if(events[i++].flag == MID::TONE_end) --num;
    
    while(events[i++].flag != MID::TONE_begin);
    u16 temp = 0;
    while(events[i++].flag != MID::TONE_end) temp += events[i].time_diff;
    return temp + events[i].time_diff;
}
u16 midSeq::dura_b(u32 num) const{
    if(num >= tone_total) return 0;
    
    int i= events.size()-1;
    while(num>0) if(events[i--].flag == MID::TONE_begin) --num;
    
    while(events[i--].flag != MID::TONE_end);
    u16 temp = events[i].time_diff;
    while(events[i--].flag != MID::TONE_begin) temp += events[i].time_diff;
    return temp;

}

void toneState::print_info() const{
    std::cout<<std::endl<<"pitch/force:"<<pitch<<","<<force;
}

u32 mid2data(short* &data, midSeq & mids, int sampleps){
    double samplept = sampleps/1000.0;//每毫秒采样数
    u32 samplesize = samplept * mids.duration;
    
    data = new short[samplesize];
    
    for(u32 i=0;i<samplesize;data[i++]=0);//初始化
    
    u16 *t_freq = new u16[mids.tone_total]; //各音符频率
    u16 *t_force = new u16[mids.tone_total];    //各音符力度
    u16 *t_dura = new u16[mids.tone_total];     //各音符持续时间
    u32 *t_start = new u32[mids.tone_total];    //各音符开始时刻
    int j=0;
    t_start[j] = 0;
    toneState tonest(df_tone);
    
    for(int i=0;i<mids.events.size(); ++i){
        if(tonest.add(mids.events[i])){//非休止
            t_start[j] += mids.events[i].time_diff;
            t_dura[j] = 0;
            t_freq[j] = mids.events[i].argu;//440Hz as 50;
            t_force[j] = 50 * tonest.force;
            while(tonest.add(mids.events[++i])){//假定同时事件中begin在end后
                t_dura[j] += mids.events[i].time_diff;
            }
            t_dura[j++] += mids.events[i].time_diff;//休止
            t_start[j] = t_start[j-1] + t_dura[j-1];
        }
        else
            t_start[j] += mids.events[i].time_diff;
        
    }
    for(j=0; j<mids.tone_total;++j){
        u32 i1 =  (u32)(t_start[j] * samplept);
        u32 i2 = u32(t_dura[j] * samplept);
        float freq0 = fr(t_freq[j]-2);
        float freq1 = 2*freq0;//泛音
        float freq2 = 3*freq0;
        float freq3 = 4*freq0;
        for(int i=0; i<i2; ++i){
            data[i1+i] += t_force[j] * amp(i, i2) * (0.8*sin(freq0*i)+0.4*cos(freq1*i)+0.3*sin(freq2*i)+0.1*cos(freq3*i));
        }
        
    }
    
    
    delete []t_dura; delete []t_start; delete []t_freq; delete []t_force;
    return samplesize;
}

u32 mid2data(short* data, std::vector<midEvent> &midv, int sampleps){//to be continued
    double samplept = sampleps/1000.0;//每毫秒采样数
    
    return midv.size();
}

void midSeq::score2mid(char scoresrc[]){
    std::fstream midin;
    midin.open(scoresrc, std::ios::in);
    char sv[10];
    midin>>sv;
    std::cout<<sv<<std::endl;
    
    u16 t, a; int f;
    while(!midin.eof()){
        midin>>t>>f>>a;
        add(midEvent(t, (MID::EVENT)f,a));
    }
    
    midin.close();
    
}


