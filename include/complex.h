//
//  complex.h
//  
//
//  Created by wyj on 7/23/14.
//
//

#ifndef _complex_h
#define _complex_h


const double ZERO = 1e-10;
const double INFITY = 1.79e+308;
const double PI = 3.14159265;

extern int sgn(double d);

class Complex{
private:
    
public:
    double x,y;//real and imaginary parts
    
    double norm() const;//复数模长
    double arg() const;//复数辐角，-PI～PI
    
    bool operator<(const Complex &);
    bool operator>(const Complex &);
    bool operator==(const Complex &);
    
    Complex():x(0.0),y(0.0){};
    Complex(double, double, short);
    ~Complex(){};
};


#endif
