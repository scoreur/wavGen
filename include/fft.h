//
//  fft.h
//  
//
//  Created by user on 8/18/14.
//
//
#include <iostream>
#ifndef _fft_h
#define _fft_h
#include <cmath>

const double ZERO = 1e-10;
//#define USE_STD_COMPLEX
//control: whether to use std lib
#ifndef USE_STD_COMPLEX
class cmplx{
public:
    cmplx():Real(0),Imag(0){}
    cmplx(const cmplx & c):
    Real(c.Real),Imag(c.Imag){}
    cmplx(double x):Real(x),Imag(0){}
    cmplx(double x, double y):
    Real(x),Imag(y){}
    double norm() const{return Real*Real+Imag*Imag;}
    double abs() const{return sqrt(norm());}
    double real(){return Real;}
    double imag(){return Imag;}
    //operators
    friend cmplx operator+(const cmplx &c1, const cmplx &c2);
    friend cmplx operator-(const cmplx &c1, const cmplx &c2);
    friend cmplx operator*(const cmplx &c1, const cmplx &c2);
    friend cmplx operator/(const cmplx &c1, const cmplx &c2);
    template<typename T>
    friend cmplx operator*(T t1, const cmplx &c2);
    template<typename T>
    friend cmplx operator*(const cmplx &c1, T t2);
    template<typename T>
    friend cmplx operator/(T t1, const cmplx &c2);
    friend std::istream &operator>>(std::istream & in, cmplx &c);
    friend std::ostream &operator<<(std::ostream & out, cmplx &c);
    
    cmplx operator*=(const cmplx &c){
        double _Real = Real*c.Real-Imag*c.Imag;
        Imag = Real*c.Imag+Imag*c.Real;
        Real = _Real;
        return *this;
    }
    cmplx operator=(const cmplx &c){
        Real = c.Real;
        Imag = c.Imag;
        return *this;
    }
    
    operator bool(){return (abs()>ZERO);}
private:
    double Real, Imag;
    
};
#else
#include <complex>
typedef std::complex<double> cmplx;
#endif

unsigned int ord2(unsigned int y);//nearest log_2
template<typename T>
T * fml(T *in, unsigned int num);//formalize to 2-power and bit-reverse
cmplx e_i(int m);//e^(2PI*i/m)
cmplx *fft(cmplx *in, unsigned int num, bool mode = true);//iterative
cmplx *dft(cmplx *in, unsigned int num, bool mode = true);
int peak(cmplx *data, int num);

#endif
