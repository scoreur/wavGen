//
//  complex.h
//  
//
//  Created by wyj on 7/9/14.
//
//

#ifndef _complex_h
#define _complex_h
#include <cmath>

const double ZERO=1e-10;
const double INFITY=1.79e+308;
const double PI=3.14159265;
const int sampleps0=44100;//默认采样数
const double fr0= 440*2*PI/sampleps0;//标准音（圆）频率除以采样数

int sgn(double d){
    if(d>ZERO)return 1;
    else if(d<-ZERO)return -1;
    else return 0;
}

double fr(int frnum)//音数与频率换算
{
    return fr0*pow(2,frnum/12.0);
}


class Complex{
public:
    double x,y;
    
    double norm(){//复数模长
        return sqrt(x*x+y*y);
    }
    double angle(){//复数辐角，-PI～PI
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
        else{//极坐标赋值
            x=c1*cos(c2);
            y=c1*sin(c2);
        }
    }
};


#endif
