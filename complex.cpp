//
//  complex.cpp
//  
//
//  Created by wyj on 7/23/14.
//
//

#include <cmath>
#include "complex.h"

int sgn(double d){
    if(d>ZERO)return 1;
    else if(d<-ZERO)return -1;
    else return 0;
}

Complex::Complex(double c1,double c2,short mode=0){
    if(mode==0){
        x=c1; y=c2;
    }
    else{//极坐标赋值
        x=c1*cos(c2);
        y=c1*sin(c2);
    }
}

double Complex::norm(){//复数模长
    return sqrt(x*x+y*y);
}

double Complex::arg(){
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