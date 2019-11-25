#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>


GLfloat light0_ambient[] = { .1, .1, .1, 0.0 };
GLfloat light0_diffuse[] = { 1.0, 0.0, 0.0, 0.0 };
GLfloat light0_specular[] = { 0.6, 0.2, .60, 0.0 };

GLfloat light0_position[] = { 1.0, 1.0, 1.0, 0.0 };

GLfloat amb[] = { 0.5, 0.0, 0.0, 1.0 };
GLfloat dif[] = { 1.0, 0.0, 0.0, 1.0 };


float w, h, tip = 0, turn = 0;

float ORG[3] = { 0,0,0 };

float XP[3] = { 1,0,0 }, XN[3] = { -1,0,0 },
YP[3] = { 0,1,0 }, YN[3] = { 0,-1,0 },
ZP[3] = { 0,0,1 }, ZN[3] = { 0,0,-1 };

void reshape(int nw, int nh)
{
    w = nw;
    h = nh;
}

void Turn(int key, int x, int y)
{
    switch (key) 
    {
    case GLUT_KEY_RIGHT: turn += 5; break;
    case GLUT_KEY_LEFT: turn -= 5; break;
    case GLUT_KEY_UP: tip -= 5; break;
    case GLUT_KEY_DOWN: tip += 5; break;
    }
}

void Draw_Axes(void)
{
    glPushMatrix();
        glTranslatef(-2.4, -1.5, -5);
        glRotatef(tip, 1, 0, 0);
        glRotatef(turn, 0, 1, 0);
        glScalef(0.25, 0.25, 0.25);

        glLineWidth(2.0);

        glBegin(GL_LINES);
            glColor3f(1, 0, 0);   // X axis is red.
            glVertex3fv(ORG);
            glVertex3fv(XP);
            glColor3f(0, 1, 0);   // Y axis is green.
            glVertex3fv(ORG);
            glVertex3fv(YP);
            glColor3f(0, 0, 1);   // z axis is blue.
            glVertex3fv(ORG);
            glVertex3fv(ZP);
        glEnd();
    glPopMatrix();
}

void Draw_Teapot(void)
{
   GLfloat fShine = 127.0;
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
   glMaterialf(GL_FRONT, GL_SHININESS, fShine);
   glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
 //   glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
   glMaterialfv(GL_FRONT, GL_SPECULAR, dif);

    glPushMatrix();
        glTranslatef(0, 0, -5);
        glRotatef(tip, 1, 0, 0);
        glRotatef(turn, 0, 1, 0);

        glColor3f(1.0, 0.5, 0.1);
        glutSolidTeapot(1.0);

        glLineWidth(2.0);
        glColor3f(0.0, 0.2, 0.9);
        glutWireTeapot(1.02);
    glPopMatrix();
}

void display(void)
{
    glViewport(0, 0, w, h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Draw_Teapot();
 //   Draw_Axes();

    glutSwapBuffers();
}
//----------------------------------------------------------------------
// User-input callbacks
//
// processNormalKeys: ESC, q, and Q cause program to exit
// pressSpecialKey: Up arrow = forward motion, down arrow = backwards
// releaseSpecialKey: Set incremental motion to zero
//----------------------------------------------------------------------
void processNormalKeys(unsigned char key, int xx, int yy)
{
    if (key == 'o')
    {
//        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
    }
    else if (key == 'f')
    {
//        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
    }
}
void main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(600, 400);
    glutInitWindowPosition(400, 300);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow("Corner Axes");
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(Turn);
    glutKeyboardFunc(processNormalKeys); // process standard key clicks


    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

    glClearColor(0.1, 0.2, 0.1, 1.0);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    gluPerspective(40.0, 1.5, 1.0, 10.0);
    glMatrixMode(GL_MODELVIEW);

    glutMainLoop();
}