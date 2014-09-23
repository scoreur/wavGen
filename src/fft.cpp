
#include "../include/fft.h"
#ifndef USE_STD_COMPLEX
cmplx operator+(const cmplx &c1, const cmplx &c2){
    return cmplx(c1.Real+c2.Real, c1.Imag+c2.Imag);
}
cmplx operator-(const cmplx &c1, const cmplx &c2){
    return cmplx(c1.Real-c2.Real, c1.Imag-c2.Imag);
}
cmplx operator*(const cmplx &c1, const cmplx &c2){
    return cmplx(c1.Real*c2.Real-c1.Imag*c2.Imag,
                 c1.Real*c2.Imag+c1.Imag*c2.Real);
}
cmplx operator/(const cmplx &c1, const cmplx &c2){
    return cmplx((c1.Real*c2.Real+c1.Imag*c2.Imag)/c2.norm(),
                 (-c1.Real*c2.Imag+c1.Imag*c2.Real)/c2.norm());
}
std::istream &operator>>(std::istream & in, cmplx &c){
    in>>c.Real>>c.Imag;
    return in;
}
std::ostream &operator<<(std::ostream & out, cmplx &c){
    if(c.Real>ZERO || c.Real<-ZERO)
    out<<c.Real;
    else
    out<<0;
    if(c.Imag>ZERO || c.Imag<-ZERO)
    out<<"+I*"<<c.Imag;
    return out;
}
template<typename T>
cmplx operator*(T t1, const cmplx &c2){
    return (cmplx)t1 * c2;
}
template<typename T>
cmplx operator*(const cmplx &c1, T t2){
    return c1 * (cmplx)t2;
}
template<typename T>
cmplx operator/(T t1, const cmplx &c2){
    return (cmplx)t1/c2;
}
#endif //alternate version of std::complex

using std::cout;


unsigned int ord2(unsigned int y){//nearest log_2
    unsigned int l=0;
    unsigned int x=y;
    while(x & ~1){l+=2;x>>=2;}
    l = x+l-1; x=1;
    for(unsigned int i=0;i<l;++i){
        if(x & y)return ++l;
        x<<=1;}
    return l;
}

unsigned int bitrev(unsigned int num, unsigned int ord=0){
    if(ord == 0 ) return 0;
    unsigned k = 1&num;
    for(int j=1;j<ord;++j){
        k = (k<<1)|((num>>j)&1);
    }
    return k;
}

template<typename T>
T * fml(T *in, unsigned int num){//formalize to 2-power and bit-reverse
    unsigned int n = 1<<ord2(num);
    T *out = new T[n];
    if(n == 1){ out[0]=in[0];return out;}
    for(int i=0;i<num;++i)
        out[bitrev(i)] = in[i];
    return out;
}


cmplx e_i(int m){//e^(2PI*i/m)
    return cmplx(cos(2*M_PI/m),sin(2*M_PI/m));
}

cmplx *fft(cmplx *in, unsigned int num, bool mode){//iterative
    
    cmplx *out = fml(in, num);
    unsigned int ord = ord2(num);
    num = 1<<ord;//formalized length
    cmplx w(0),u(0),t(0);//for temporary storing
    unsigned int m = 1;
    for(unsigned s=1;s<=ord;++s){//depth
        m = 1<<s;
        cmplx wm = mode? e_i(m): e_i(-m);//rotating factor
        
        for(int k=0;k<num;k+=m)
        {
            w = cmplx(1.0,0.0);
            for(int j=0;j<(m>>1);++j){
                t = w * out[k+j+(m>>1)];
                u = out[k+j];
                out[k+j]=u+t;
                out[k+j+(m>>1)]=u-t;
                w = w * wm;
            }
        }
    }
    if(mode)
        for(int k=0;k<num;++k){
            out[k] *= 1.0/num;//normalize
        }
    
    return out;
}
cmplx *dft(cmplx *in, unsigned int num, bool mode){//accurate
    cmplx *out = new cmplx[num];
    cmplx w0 = mode? e_i(num): e_i(-num);
    cmplx w = 1.0;
    for(int j=0;j<num;++j){
        out[j] = 0.0;
        for(int k=num-1;k>=0;--k){//Horner
            out[j] = w*out[j]+in[k];
        }
        w *= w0;
        if(mode)
            out[j] *= (1.0 /num);
        
        //if(j) out[num-j] = cmplx(out[j].real(), -out[j].imag());
    }
    return out;
}

int peak(cmplx *data, int num){
    double max = 0;
    double k = -1;
    for(int i=0;i<num;++i){
        if(max<data[i].abs()){
            max = data[i].abs();
            k = i;
        }
    }
    cout<<"MAX: "<<max;
    return k;
}


 
int main(int argc, char *argv[]){

    
    int d = 11;
    unsigned int num = 64;
    cout<<"input the 2-power:\n";
    std::cin>>num;
    unsigned int ord = ord2(num);
    num = 1<<ord;
    cmplx *in = new cmplx[num];
   
    for(int k=0;k<num;++k){
        in[k] = k*2;//100*sin(2*M_PI/num * k * d);
                    //if(k%16==0)cout<<in[k];
    }
    cmplx *test = dft(in, num);
    cout<<'\n';
    cmplx *out = fft(in, num);//should be deleted
    
    cmplx *back = fft(out, num, false);
    for(int i=0;i<num;++i){
        cout<<in[i]<<"OUT:"<<out[i]<<"TEST:"<<test[i]<<'\n'<<back[i]<<'\n';
    }
    //cout<<"OUT: "<<out[num-d]<<'\t'<<out[d]<<'\t'<<out[d+1]<<'\n';
    
    //cout<<"TEST: "<<test[num-d]<<'\t'<<test[d]<<'\t'<<test[d+1]<<'\n';
    //g = peak(test, num);
    //cout<<"at "<<g <<'\n';
    delete []out; delete []back; delete []test; delete[]in;

    return 0;
}

