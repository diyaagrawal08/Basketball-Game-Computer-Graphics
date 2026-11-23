
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <cstdio>
#include <cstring>

const float PI=3.14159f;
HDC dc;
HGLRC rc;
HWND hwnd;
bool running=true,shooting=false,scored=false,reflected=false;
float x=0,y=.43f,z=-2,vx=0,vy=6.3f,vz=-7.5f;
float angle=0,scale=1;
int score=0;

void box(float a,float b,float c,float w,float h,float d)
{
    glPushMatrix();
    glTranslatef(a,b,c);
    glScalef(w,h,d);
    glBegin(GL_QUADS);
    glNormal3f(0,1,0);
    glVertex3f(-.5,.5,-.5); glVertex3f(-.5,.5,.5);
    glVertex3f(.5,.5,.5); glVertex3f(.5,.5,-.5);
    glNormal3f(0,-1,0);
    glVertex3f(-.5,-.5,-.5); glVertex3f(.5,-.5,-.5);
    glVertex3f(.5,-.5,.5); glVertex3f(-.5,-.5,.5);
    glNormal3f(0,0,1);
    glVertex3f(-.5,-.5,.5); glVertex3f(.5,-.5,.5);
    glVertex3f(.5,.5,.5); glVertex3f(-.5,.5,.5);
    glNormal3f(0,0,-1);
    glVertex3f(-.5,-.5,-.5); glVertex3f(-.5,.5,-.5);
    glVertex3f(.5,.5,-.5); glVertex3f(.5,-.5,-.5);
    glEnd();
    glPopMatrix();
}

void court()
{
    glColor3f(.48f,.22f,.09f);
    box(0,-.12f,-1,15,.24f,24);
    glDisable(GL_LIGHTING);
    glColor3f(.65f,.36f,.16f);
    glLineWidth(1);
    glBegin(GL_LINES);
    for(float a=-12;a<=10;a+=1.2f)
    {
        glVertex3f(-7,.015f,a);
        glVertex3f(7,.015f,a);
    }
    glEnd();
    glColor3f(1,.9f,.75f);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex3f(-7,.02f,-13); glVertex3f(7,.02f,-13);
    glVertex3f(7,.02f,-13); glVertex3f(7,.02f,11);
    glVertex3f(7,.02f,11); glVertex3f(-7,.02f,11);
    glVertex3f(-7,.02f,11); glVertex3f(-7,.02f,-13);
    glVertex3f(-7,.02f,-1); glVertex3f(7,.02f,-1);
    glEnd();
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<80;i++)
    {
        float a=2*PI*i/80;
        glVertex3f(2*cosf(a),.02f,-1+2*sinf(a));
    }
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex3f(-2.5f,.02f,-13);
    glVertex3f(2.5f,.02f,-13);
    glVertex3f(2.5f,.02f,-7);
    glVertex3f(-2.5f,.02f,-7);
    glEnd();
    glEnable(GL_LIGHTING);
}

void hoop()
{
    glColor3f(.18f,.18f,.2f);
    box(0,1.4f,-11,.18f,2.8f,.18f);
    box(0,2.6f,-10,.18f,.18f,2);
    glColor3f(.9f,.94f,.97f);
    box(0,3.1f,-9.05f,3.6f,2.1f,.12f);
    glDisable(GL_LIGHTING);
    glColor3f(1,.22f,0);
    glLineWidth(5);
    glBegin(GL_LINE_LOOP);
    for(int i=0;i<60;i++)
    {
        float a=2*PI*i/60;
        glVertex3f(.62f*cosf(a),2.18f,-8.35f+.62f*sinf(a));
    }
    glEnd();
    glColor3f(1,1,1);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    for(int i=0;i<12;i++)
    {
        float a=2*PI*i/12;
        glVertex3f(.62f*cosf(a),2.15f,-8.35f+.62f*sinf(a));
        glVertex3f(.3f*cosf(a),1.25f,-8.35f+.3f*sinf(a));
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void ball()
{
    glPushMatrix();
    glTranslatef(x,y,z);
    glRotatef(angle,1,0,0);
    glRotatef(angle*.7f,0,0,1);
    glScalef(reflected?-scale:scale,scale,scale);
    GLUquadric* q=gluNewQuadric();
    glColor3f(1,.29f,.015f);
    gluSphere(q,.42,32,24);
    glDisable(GL_LIGHTING);
    glColor3f(.04f,.02f,.01f);
    glLineWidth(2);
    for(int k=0;k<3;k++)
    {
        glBegin(GL_LINE_STRIP);
        for(int i=0;i<=60;i++)
        {
            float a=2*PI*i/60;
            glVertex3f(.423f*cosf(a),.423f*sinf(a),
                       .02f*sinf(2*a+k*PI/3));
        }
        glEnd();
    }
    glColor3f(1,1,0);
    glBegin(GL_LINES);
    glVertex3f(.05f,.05f,.42f);
    glVertex3f(.3f,.05f,.32f);
    glEnd();
    glEnable(GL_LIGHTING);
    gluDeleteQuadric(q);
    glPopMatrix();
}

void text(float a,float b,const char* s)
{
    glRasterPos2f(a,b);
    glListBase(968);
    glCallLists((GLsizei)strlen(s),GL_UNSIGNED_BYTE, s);
}

void hud()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0,1000,0,700,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glColor3f(.04f,.06f,.1f);
    glBegin(GL_QUADS);
    glVertex2f(15,440); glVertex2f(250,440);
    glVertex2f(250,680); glVertex2f(15,680);
    glEnd();
    char s[60];
    glColor3f(1,.75f,.15f);
    sprintf(s,"SCORE: %d",score);
    text(35,645,s);
    glColor3f(1,1,1);
    text(35,610,"BASKETBALL GAME");
    text(35,580,"SPACE : SHOOT");
    text(35,555,"A / D : LEFT/RIGHT");
    text(35,530,"Q / E : ROTATE");
    text(35,505,"1 / 2 : DEC/INC");
    text(35,480,"F : REFLECTION");
    text(35,455,"R : RESET");
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void resetBall()
{
    shooting=false; scored=false;
    x=0; y=.43f; z=-2;
    vx=0; vy=6.3f; vz=-7.5f;
    angle=0;
}

void update()
{
    if(!shooting)return;
    float oldZ=z,dt=.016f;
    x+=vx*dt; y+=vy*dt; z+=vz*dt;
    vy-=9.8f*dt; angle+=9;
    if(!scored && oldZ>-8.35f && z<=-8.35f)
        if(fabsf(x)<.23f && y>1.8f && y<2.6f)
        {
            score++;
            scored=true;
        }
    if(y<.43f || z<-13 || fabsf(x)>7) resetBall();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(9,7,11,0,1,-4,0,1,0);
    GLfloat light[]={-4,10,5,1};
    glLightfv(GL_LIGHT0,GL_POSITION,light);
    court(); hoop(); ball(); hud();
    SwapBuffers(dc);
}

void resize(int w,int h)
{
    if(h==0)h=1;
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(48,(double)w/h,.1,100);
    glMatrixMode(GL_MODELVIEW);
}

LRESULT CALLBACK proc(HWND h,UINT m,WPARAM w,LPARAM l)
{
    switch(m)
    {
    case WM_SIZE: resize(LOWORD(l),HIWORD(l)); return 0;
    case WM_KEYDOWN:
        if(w==VK_SPACE&&!shooting)
        { shooting=true; scored=false; vx=0; vy=6.3f; vz=-7.5f; }
        if(w=='A'&&!shooting)x-=.2f;
        if(w=='D'&&!shooting)x+=.2f;
        if(w=='Q')angle-=10;
        if(w=='E')angle+=10;
        if(w=='1'&&scale>.3f)scale-=.25f;
        if(w=='2'&&scale<2.5f)scale+=.25f;
        if(w=='F')reflected=!reflected;
        if(w=='R')resetBall();
        return 0;
    case WM_CLOSE: DestroyWindow(h); return 0;
    case WM_DESTROY:
        running=false; PostQuitMessage(0); return 0;
    }
    return DefWindowProc(h,m,w,l);
}

int main()
{
    HINSTANCE ins=GetModuleHandle(NULL);
    WNDCLASS wc={};
    wc.style=CS_OWNDC;
    wc.lpfnWndProc=proc;
    wc.hInstance=ins;
    wc.lpszClassName="Basketball3D";
    wc.hCursor=LoadCursor(NULL,IDC_ARROW);
    RegisterClass(&wc);

    hwnd=CreateWindow("Basketball3D","3D Basketball Game",
        WS_OVERLAPPEDWINDOW|WS_VISIBLE,100,80,1000,700,
        NULL,NULL,ins,NULL);
    dc=GetDC(hwnd);
    PIXELFORMATDESCRIPTOR p={};
    p.nSize=sizeof(p); p.nVersion=1;
    p.dwFlags=PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
    p.iPixelType=PFD_TYPE_RGBA; p.cColorBits=24; p.cDepthBits=24;
    SetPixelFormat(dc,ChoosePixelFormat(dc,&p),&p);
    rc=wglCreateContext(dc);
    wglMakeCurrent(dc,rc);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    GLfloat diffuse[]={1,1,1,1};
    glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);
    glClearColor(.08f,.10f,.15f,1);

    HFONT font=CreateFontA(22,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,
        ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY,FF_DONTCARE,"Arial");
    SelectObject(dc,font);
    wglUseFontBitmapsA(dc,32,96,1000);
    resize(1000,700);

    MSG msg;
    while(running)
    {
        while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
        { TranslateMessage(&msg); DispatchMessage(&msg); }
        update();
        display();
        Sleep(16);
    }
    wglMakeCurrent(NULL,NULL);
    wglDeleteContext(rc);
    DeleteObject(font);
    ReleaseDC(hwnd,dc);
    return 0;
}
