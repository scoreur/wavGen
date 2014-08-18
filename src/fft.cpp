
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
template<typename T>
T * fml(T *in, unsigned int num){//formalize to 2-power and bit-reverse
    if(num==1) return in;
    unsigned int n = 1<<ord2(num);
    T *out = new T[n];
    
    for(int i=0;i<num;++i)
    out[i] = in[i];
    for(int i=num;i<n;++i)
    out[i] = (T)0.0;
    T temp = (T)0.0;
    for(unsigned int i=1;i<n>>1;++i){//bit reverse
        unsigned k = 1&i;
        for(int j=1;j<ord2(num);++j){
            k = (k<<1)|((i>>j)&1);
        }
        temp = out[i]; out[i]=out[k];out[k]=temp;
        temp = out[n-k]; out[n-k]=out[n-i];out[n-k]=temp;
        
    }
    return out;
}

cmplx e_i(int m){//e^(2PI*i/m)
    return cmplx(cos(2*M_PI/m),sin(2*M_PI/m));
}

cmplx *fft(cmplx *in, unsigned int num, int mode){//iterative
    //mode == -1 means inverse transform
    if(mode != 1) mode = -1;
    cmplx *out = fml(in, num);
    unsigned int ord = ord2(num);
    num = 1<<ord;//formalized length
    for(unsigned s=1;s<=ord;++s){//depth
        int m = 1<<s;
        cmplx wm = e_i(mode * m);//rotating factor
        cmplx w,u,t;//for temporary storing
        for(int k=0;k<num>>1;k+=m)
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
    if(mode == 1)
    for(int k=0;k<num;++k){
        out[k]=out[k]*2.0/(cmplx)num;//normalize
    }
   
    return out;
}
cmplx *dft(cmplx *in, unsigned int num, int mode){//accurate
    //mode == -1 means inverse transform
    if(mode != 1) mode = -1;
    cmplx *out = new cmplx[num];
    cmplx w0 = e_i( mode * num);
    cmplx w = 1.0;
    for(int j=0;j<(num>>1);++j){
        out[j] = 0.0;
        for(int k=num-1;k>=0;--k){//Horner
            out[j] = w*out[j]+in[k];
        }
        w *= w0;
        if(mode ==1)
            out[j] *= (2.0 /num);
        out[num-j] = cmplx(out[j].real(), -out[j].imag());
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

/* test
 
int main(int argc, char *argv[]){

    
    int d = 11;
    unsigned int num = 64;
    cout<<"input the 2-power:\n";
    std::cin>>num;
    num = 1<<ord2(num);
    cout<<"input the test pos<"<<num<<":\n";
    std::cin>>d;
    cmplx *in = new cmplx[num];
   
    for(int k=0;k<num;++k){
        in[k] = 100*sin(2*M_PI/num * k * d);
         if(k%16==0)cout<<in[k];
    }
    cmplx *test = dft(in, num);
    cout<<std::endl;
    cmplx *out = fft(in, num);//should be deleted
    unsigned int ord = ord2(num);
    num = 1<<ord;
    cmplx *back = fft(out, num, -1);
    
    cout<<"OUT: "<<out[num-d]<<'\t'<<out[d]<<'\t'<<out[d+1]<<'\n';
    int g = peak(out, num);
    cout<< "at "<<g <<'\n';
    
    cout<<"TEST: "<<test[num-d]<<'\t'<<test[d]<<'\t'<<test[d+1]<<'\n';
    g = peak(test, num);
    cout<<"at "<<g <<'\n';
    delete []out; delete []back; delete []test; delete[]in;

    return 0;
}
 */
