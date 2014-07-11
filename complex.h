//
//  complex.h
//  
//
//  Created by wyj on 7/9/14.
//
//

#ifndef _complex_h
#define _complex_h
#include <iostream>
#include <math.h>

const double ZERO=1e-10;
const double INFITY=1.79e+308;
const double PI=3.14159265;
int sgn(double d){
    if(d>ZERO)return 1;
    else if(d<-ZERO)return -1;
    else return 0;
}

class Complex{
public:
    double x,y;
    
    double norm(){
        return sqrt(x*x+y*y);
    }
    double angle(){
        double temp=0.0;
        switch(sgn(x)){
            case -1:
                if(y>ZERO)temp= PI+atan(y/x);
                else if(y<-ZERO)temp= atan(y/x)-PI;
                else temp= PI;
                break;
            case 0:
                if(y>ZERO)temp= PI/2;
                else if(y<-ZERO)temp=3*PI/2;
                else temp= 0.0;
                break;
                
            case 1:
                if(y>ZERO||y<-ZERO)return atan(y/x);
                else temp= 0.0;
                break;
        }
        return temp;

    }
    
    Complex(double c1=0.0,double c2=0.0,short mode=0){
        if(mode==0){
            x=c1; y=c2;
        }
        else{
            x=c1*cos(c2);
            y=c1*sin(c2);
        }
    }
};

Complex *fourier0(short *data, int bufsize, double freq1,  short frsize=1, short *freq=NULL){
    Complex *amp_cp=new Complex[frsize];
    double *sin_temp=new double(bufsize);
    double *cos_temp=new double(bufsize);
    double amp_cos=0.0,amp_sin=0.0;
    
    for(int j=0;j<bufsize;++j){
        *(sin_temp+j)=sin(freq1*j);
        *(cos_temp+j)=cos(freq1*j);
        amp_cos+=(double)(data[j]*cos_temp[j]);
        amp_sin+=(double)(data[j]*sin_temp[j]);
    }
    amp_cp[0].x=amp_cos/bufsize*2;
    amp_cp[0].y=amp_sin/bufsize*2;
    
    for(int i=1;i<frsize;++i){
        for(int j=0;j<bufsize;++j){
            amp_cos+=(double)(data[j]*cos_temp[(j*freq[i])%bufsize]);
            amp_sin+=(double)(data[j]*sin_temp[(j*freq[i])%bufsize]);
        }
        amp_cp[i].x=amp_cos/bufsize*2;
        amp_cp[i].y=amp_sin/bufsize*2;
    }
    delete []sin_temp; delete []cos_temp;
    return amp_cp;
    
}

#endif
