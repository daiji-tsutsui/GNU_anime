//
//  main.cpp
//  GNU_anime
//
//  Created by 筒井 大二 on 2018/08/16.
//  Copyright © 2018年 筒井 大二. All rights reserved.
//

#include <iostream>
#include <OpenGL/OpenGL.h>
#include <GLUT/GLUT.h>
#include <random>

#define P    10.0
#define R    28.0
#define B    8.0/3.0
#define EPS    0.001
#define T    10
using namespace std;
void drawSphere(double x, double y, double z, double r, int div = 5);
void drawCircle(double x0, double y0, double r, int div = 20);

random_device rnd;     // 非決定的な乱数生成器
mt19937 mt(rnd());  // メルセンヌ・ツイスタの32ビット版、引数は初期シード
uniform_real_distribution<> unif(0.0, 1.0);   // [0.0,1.0]上一様に分布させる
normal_distribution<> gauss(0.0, 1.0);   // 平均0.0、標準偏差1.0で分布させる

int lorenzFlg = 0;    // 0: glut, 1: gnuplot
int winh = 800;
int winw = 1000;
double xMin = -20.0;
double xMax = 20.0;
double yMin = -20.0;
double yMax = 20.0;
double zMin = 0.0;
double zMax = 40.0;

int itrNum = T / EPS;
int seqNum = 10000;
double *x, *y, *z;
double inix = 10.0;
double iniy = 10.0;
double iniz = 20.0;
double t = 0.0; double t0 = 0.0;
double s = 0.0; double s0 = 0.0;
double r = 2.5;
int cnt = 0;
int btnFlg = 0;
double px, py;

double dx(double x1, double y1, double z1){
    return -P * x1 + P * y1;
}
double dy(double x1, double y1, double z1){
    return -x1 * z1 + R * x1 - y1;
}
double dz(double x1, double y1, double z1){
    return x1 * y1 - B * z1;
}


/*--For OpenGL-------------------------------------------------------------------------*/
void idle(void){
    glutPostRedisplay();
}
void setup(void) {
    //    glClearColor(1.0, 0.99, 0.91, 1.0);       //Yellow
    glClearColor(0.0, 0.0, 0.1, 1.0);       //Black
}
void resize(int width, int height) {
    glViewport(0, 0, width, height);
    if(lorenzFlg == 0){
        glLoadIdentity();
    }
}
void timer(int value) {
    glutPostRedisplay();
    if(lorenzFlg == 0){
        glutTimerFunc(10 , timer , 0);
    }
    if(cnt % 1000 == 0) cout << cnt << endl;
}
void mouse(int button, int state, int cx, int cy)
{
    if(button == GLUT_LEFT_BUTTON){
        if(state == GLUT_DOWN){
            btnFlg = 1;
            px = cx; py = cy;
        }else if(state == GLUT_UP){
            btnFlg = 0;
            t0 = t;
            s0 = s;
        }
    }
}
void motion(int cx, int cy){
//    if(lorenzFlg == 1 && btnFlg){
//        t = t0 - 0.005 * (cx - px);
//        s = max(min(s0 + 0.005 * (cy - py), M_PI/2), -M_PI/2);
//        glutPostRedisplay();
//    }
}

/*--Display func-------------------------------------------------------------------------*/
void display(void){
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glLoadIdentity();
    if(lorenzFlg == 0){
        glOrtho(-1.5, 1.5, -1.2, 1.8, -1.5, 1.5);
        glRotated(90.0, 1.0, 0.0, 0.0);
    }
    //Runge-Kutta法
    double kx[3], ky[3], kz[3];
    double mx[3], my[3], mz[3];
    for(int i = 0; i < 10; i++){
        for(int k = 0; k < seqNum; k++){
            kx[0] = dx(x[k],y[k],z[k]);
            ky[0] = dy(x[k],y[k],z[k]);
            kz[0] = dz(x[k],y[k],z[k]);
            mx[0] = x[k] + 0.5 * EPS * kx[0];
            my[0] = y[k] + 0.5 * EPS * ky[0];
            mz[0] = z[k] + 0.5 * EPS * kz[0];
            kx[1] = dx(mx[0],my[0],mz[0]);
            ky[1] = dy(mx[0],my[0],mz[0]);
            kz[1] = dz(mx[0],my[0],mz[0]);
            mx[1] = x[k] + 0.5 * EPS * kx[1];
            my[1] = y[k] + 0.5 * EPS * ky[1];
            mz[1] = z[k] + 0.5 * EPS * kz[1];
            kx[2] = dx(mx[1],my[1],mz[1]);
            ky[2] = dy(mx[1],my[1],mz[1]);
            kz[2] = dz(mx[1],my[1],mz[1]);
            mx[2] = x[k] + EPS * kx[2];
            my[2] = y[k] + EPS * ky[2];
            mz[2] = z[k] + EPS * kz[2];
            x[k] += EPS * (kx[0] + 2.0*kx[1] + 2.0*kx[2] + dx(mx[2],my[2],mz[2]))/6.0;
            y[k] += EPS * (ky[0] + 2.0*ky[1] + 2.0*ky[2] + dy(mx[2],my[2],mz[2]))/6.0;
            z[k] += EPS * (kz[0] + 2.0*kz[1] + 2.0*kz[2] + dz(mx[2],my[2],mz[2]))/6.0;
        }
        cnt++;
    }
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);
    glColor4d(1.0, 1.0, 0.0, 0.3);
    //particlesの描画
    for(int k = 0; k < seqNum; k++){
        glTranslated(1.0-2.0*(x[k]-xMin)/(xMax-xMin),
                     1.0-2.0*(y[k]-yMin)/(yMax-yMin),
                     1.0-2.0*(z[k]-zMin)/(zMax-zMin));
        glutSolidSphere(0.005, 10, 5);
        glTranslated(-1.0+2.0*(x[k]-xMin)/(xMax-xMin),
                     -1.0+2.0*(y[k]-yMin)/(yMax-yMin),
                     -1.0+2.0*(z[k]-zMin)/(zMax-zMin));
    }
    //        char filepath[200];
    //        sprintf(filepath, "/Users/daiji/Documents/C/Lorenz-Attractor/Lorenz Attractor/images/output%06d.png", cnt);
    //        capture(filepath);
    
    glutSwapBuffers();
}


/*--Main func-------------------------------------------------------------------------*/
int main(int argc, char * argv[]) {
    /*--trajectoryの計算-------*/
    cout << "itrNum: " << itrNum << endl
        << "seqNum: " << seqNum << endl;
    
    x = (double *)calloc(seqNum, sizeof(double));
    y = (double *)calloc(seqNum, sizeof(double));
    z = (double *)calloc(seqNum, sizeof(double));
    for(int k = 0; k < seqNum; k++){
        x[k] = inix; y[k] = iniy; z[k] = iniz;
        x[k] += unif(mt) * 0.001;
        y[k] += unif(mt) * 0.001;
        z[k] += unif(mt) * 0.001;
    }
    
    
    
//    getchar();
    
    /*--Main loop-------*/
    if(lorenzFlg == 0){
        glutInit(&argc, argv);
        glutInitWindowSize(winw, winh);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
        glutCreateWindow("Lorenz Attractor");
        glutReshapeFunc(resize);
        glutDisplayFunc(display);
        glutMouseFunc(mouse);
    //    glutMotionFunc(motion);
    //    glutIdleFunc(idle);
        glutTimerFunc(100 , timer , 0);
        setup();
        glutMainLoop();
    }
    
    free(x); free(y); free(z);
    return 0;
}


/*--Other func-------------------------------------------------------------------------*/







