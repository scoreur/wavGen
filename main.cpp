//
//  main.cpp
//  wavReader
//
//  Created by user on 5/2/14.
//  Copyright (c) 2014 user. All rights reserved.
//

#include <GLUT/glut.h>
#include "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk/System/Library/Frameworks/GLUT.framework/Headers/glut.h"

//above for openGL

#include <math.h>

#include "/Users/user/Documents/compile/wavReader/wav.h"

//using namespace std;

int bufsize=1000;
float acc=0.005;
int offs=46;
float wavleft=-0.99;
float wavtop=0.3;
float wavbot=-0.3;
int winheight=400, winwidth=400;
const int sampleps0=44100;
const double fr0= 440*2*PI/sampleps0;
const int btt=0.25*sampleps0;

//global var controling display

void display(void)//print waveform
{
    glClear(GL_COLOR_BUFFER_BIT);
   /* glBegin(GL_POLYGON);//circle
    for(int i=0;i<36000;i++)
    {
        glVertex2f(0.5*cos(i*PI*2/36000),0.5*sin(i*PI*2/36000));
    }
    glEnd();*/
    
    short *chh= new short[bufsize];
    short *chl= new short[bufsize];
    
    char addr[]="/Users/user/Documents/compile/wavReader/in.wav";
    wavfile in(addr);
    in.get_data(chh,chl,bufsize,offs);//get data from wave file
    int ptlen=bufsize;//print how many sample
        glLoadIdentity();
    glBegin(GL_LINE_STRIP);
        for(int j=0;j<ptlen;++j){
            
        glVertex2f(acc*j+wavleft,wavtop+((float)chh[j])/(float)(1<<17));
        }
    glEnd();
        glBegin(GL_LINE_STRIP);
        for(int j=0;j<ptlen;++j){
            //cout<<hex<<j+offs<<'y'<<hex<<(u16)chh[j]<<'y'<<hex<<(u16)chl[j]<<';';//for debug
            glVertex2f(acc*j+wavleft,wavbot+((float)chl[j])/(float)(1<<17));
        }
        glEnd();
    glFlush();
    delete []chh;
    delete []chl;
}
void myInt()
{
    glClearColor(1.0, 1.0, 1.0, 0.5);
    glColor3f(0.0f, 1.0f, 0.0f);
}
void click(int button,int state, int x, int y)
{
    if(state==GLUT_DOWN)
    {
        glPointSize(4.0f);
        if(button==GLUT_LEFT_BUTTON)
        {
            cout<<endl<<x<<';'<<y;
            glBegin(GL_POINTS);
            glVertex2f(x*2.0/winheight-1.0, -y*2.0/winwidth+1.0);
            glEnd();
            glFlush();
        }
        else if(button==GLUT_RIGHT_BUTTON)
        {
            glTranslatef(0.1f, 0.1f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glColor3f(1.0f,1.0f,0.0f);
            offs=offs+200;
            //glutDisplayFunc(display);
            glFlush();
                    }
    }
}
void changeSize(int h, int w)
{
    h=winheight+20;w=winwidth+20;
    glViewport(0, 0, w, h);

}

double fr(int freq)
{
    return fr0*pow(2,freq/12.0);
}


int main(int argc, char * argv[])
{
    
    // insert code here...
    
    cout << endl<<sizeof(wav_Block)<<sizeof(wav_Fmt)<<' '<<sizeof(char);
    char in2addr[]="/Users/user/Documents/compile/wavReader/sine.wav";

    int snum=56;
    double *freq=new double[snum];
    short frnum[56]={-9,-9,-2,-2,0,0,-2,-4,-4,-5,-5,-7,-7,-9,-2,-2,-4,-4,-5,-5,-7,-2,-2,-4,-4,-5,-5,-7,-9,-9,-2,-2,0,0,-2,-4,-4,-5,-5,-7,-7,-9};
    short tnum[56]={1,1,1,1,1,1,2,1,1,1,1,1,1,2,1,1,1,1,1,1,2,1,1,1,1,1,1,2,1,1,1,1,1,1,2,1,1,1,1,1,1,2};
    for(int i=0;i<snum;++i)freq[i]=fr(frnum[i]);
    
    wavfile in2(in2addr,64*btt);
    in2.print_info();
    in2.put_data(btt,snum,freq,tnum);
    
    cin>>offs;
    glutInit(&argc, argv);
    glutInitWindowPosition(0,0);
    glutInitWindowSize(400,400);
    int myWin=glutCreateWindow("wave form");
    glutDisplayFunc(display);
    myInt();
    glutReshapeFunc(changeSize);
    glutMouseFunc(click);
    glutMainLoop();
    cin>>bufsize;
    cout<<myWin;
    cin>>offs;
    return 0;
}

