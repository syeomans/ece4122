/*
Author: Samuel Yeomans
Class: ECE 4122
Date: November 22, 2019
Description:
3D chess set model using OpenGL
*/

#include <iostream>
#include <GL/glut.h> // Include the GLUT header file
#include <stdio.h>
#include <math.h>
#include <stdlib.h> // standard definitions
#define ESC 27


//----------------------------------------------------------------------
// Global variables
//
// The coordinate system is set up so that the (x,y)-coordinate plane
// is the ground, and the z-axis is directed upwards. The y-axis points
// to the north and the x-axis points to the east.
//
// The values (x,y) are the current camera position. The values (lx, ly)
// point in the direction the camera is looking. The variables angle and
// deltaAngle control the camera's angle. The variable deltaMove
// indicates the amount of incremental motion for the camera with each
// redraw cycle. The variables isDragging and xDragStart are used to
// monitor the mouse when it drags (with the left button down).
//----------------------------------------------------------------------

// Camera position
float x = 4.0, y = -5.0; // initially 5 units south of origin
float deltaMove = 0.0; // initially camera doesn't move

// Camera direction
float lx = 0.0, ly = 1.0; // camera points initially along y-axis
float angle = 0.0; // angle of rotation for the camera direction
float deltaAngle = 0.0; // additional angle change when dragging

// Board rotation
float boardAngle = 0.0;
float deltaBoardAngle = 0.0;

// Eye location
float eyeLocationZ = 10.0;
float deltaEyeLocation = 0.0;

// Lighting toggle
bool light1Toggled = true;
bool light0Toggled = true;

//----------------------------------------------------------------------
// Reshape callback
//
// Window size has been set/changed to w by h pixels. Set the camera
// perspective to 45 degree vertical field of view, a window aspect
// ratio of w/h, a near clipping plane at depth 1, and a far clipping
// plane at depth 100. The viewport is the entire window.
//
//----------------------------------------------------------------------
void changeSize(int w, int h)
{
  float ratio = ((float)w) / ((float)h); // window aspect ratio
  glMatrixMode(GL_PROJECTION); // projection matrix is active
  glLoadIdentity(); // reset the projection
  gluPerspective(60.0, ratio, 0.1, 100.0); // perspective transformation
  glMatrixMode(GL_MODELVIEW); // return to modelview mode
  glViewport(0, 0, w, h); // set viewport (drawing area) to entire window
}

//----------------------------------------------------------------------
// Functions to draw chess pieces
//
// Each chess piece is a simple shape, defined below
// Pawn: sphere
// Rook: cube
// Knight: teapot
// Bishop: cone
// King: octahedron
// Queen: tetrahedron
//----------------------------------------------------------------------

void drawPawn()
{
  glPushMatrix();
    glTranslatef(0.0, 0.0, 0.5);
    glutSolidSphere(0.5, 20, 16);
    glScalef(0.75, 0.75, 1.0);
  glPopMatrix();
}

void drawRook()
{
  glPushMatrix();
    glTranslatef(0.0, 0.0, 0.4);
    glutSolidCube(0.75);
    glScalef(0.5, 0.5, 0.5);
  glPopMatrix();
}

void drawKnight()
{
  glPushMatrix();
    glTranslatef(0.0, 0.0, 0.5*0.75+0.01);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glScalef(0.5, 1.0, 0.5);
    glutSolidTeapot(0.5);
  glPopMatrix();
}

void drawBishop()
{
  glPushMatrix();
    glTranslatef(0.0, 0.0, 0.0);
    glScalef(0.75, 0.75, 0.75);
    glutSolidCone(0.5, 1.0, 10, 2);
    glScalef(0.5, 0.5, 0.5);
  glPopMatrix();
}

void drawKing()
{
  glPushMatrix();
    glTranslatef(0.0, 0.0, 0.4);
    glScalef(0.4, 0.4, 0.4);
    glutSolidOctahedron();
  glPopMatrix();
}

void drawQueen()
{
  glPushMatrix();
    glTranslatef(0.0, 0.0, 0.18);
    glRotatef(25.0*5.0, 1.0, 1.0, 0.0);
    glScalef(0.5, 0.5, 0.5);
    glutSolidTetrahedron();
  glPopMatrix();
}


//----------------------------------------------------------------------
// Update with each idle event
//
// This incrementally moves the camera and board, and requests that the
// scene be redrawn.
//----------------------------------------------------------------------
void update(void)
{
  if (deltaMove) { // update camera position
    x += deltaMove * lx * 0.1;
    y += deltaMove * ly * 0.1;
  }

  if (deltaBoardAngle != 0.0)
  {
    boardAngle += deltaBoardAngle;
    deltaBoardAngle = 0.0;
  }

  if (deltaEyeLocation != 0.0)
  {
    eyeLocationZ += deltaEyeLocation;
    deltaEyeLocation = 0.0;
  }

  glutPostRedisplay(); // redisplay everything
}

//----------------------------------------------------------------------
// Draw the entire scene
//
// We first update the camera location based on its distance from the
// origin and its direction.
//----------------------------------------------------------------------
void renderScene(void)
{

  // GLfloat light_position[] = { 0.0, 0.0, 1.5, 1.0 };
  // glClearColor(0.0, 0.7, 1.0, 1.0); // sky color is light blue
  // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //
  //  glPushMatrix();
  //      gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  //      glPushMatrix();
  //          // glRotated(spin, 1.0, 0.0, 0.0);
  //          glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  //      glPopMatrix();
  //      glutSolidTorus (0.275, 0.85, 8, 15);
  //  glPopMatrix();
  //  glFlush();

  /////////////////////////////////////////////////// TEST AREA ///////////////////////////////////////////

  glEnable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);

  glShadeModel(GL_SMOOTH);
  GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
  GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
  GLfloat spot_direction[] = { 5.0, 5.0, 8.0 };
  GLfloat turnOff[] = {0.0, 0.0, 0.0};
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT1, GL_POSITION, spot_direction);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, turnOff);
  glLightfv(GL_LIGHT0, GL_SPECULAR, turnOff);
  // glEnable(GL_LIGHTING);
  if (light0Toggled)
    glEnable(GL_LIGHT0);
  else
    glDisable(GL_LIGHT0);
  if (light1Toggled)
    glEnable(GL_LIGHT1);
  else
    glDisable(GL_LIGHT1);
  // glEnable(GL_COLOR_MATERIAL);
  // glEnable(GL_DEPTH_TEST);

  /////////////////////////////////////////////////// TEST AREA ///////////////////////////////////////////

  // Clear color and depth buffers
  //glClearColor(0.0, 0.7, 1.0, 1.0); // sky color is light blue
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Reset transformations
  glLoadIdentity();

  // Set the camera centered at (x,y,1) and looking along directional
  // vector (lx, ly, 0), with the z-axis pointing up
    gluLookAt(
        4.0, -5.0, eyeLocationZ,
        4.0, 4.0, 0.0,
        0.0, 0.0, 2.0);

    // rotate board
    glTranslatef(4.0, 4.0, 0.0);
    glRotatef(boardAngle, 0.0, 0.0, 1.0);
    glTranslatef(-4.0, -4.0, 0.0);

    // Draw chessboard - 8 x 8 grid of alternating light and dark squares
    for (int i=0; i<8; i++)
    {
      for (int j=0; j<8; j++)
      {
        // Alternate colors
        if ((i+j)%2 == 0)
        {
          glColor3f(0.0, 0.0, 0.0);
        }
        else
        {
          glColor3f(1.0, 1.0, 1.0);
        }
        // Draw squares
        glBegin(GL_QUADS);
        glVertex3f((float)i, (float)j, 0.0);
        glVertex3f((float)i, (float)j + 1.0, 0.0);
        glVertex3f((float)i + 1.0, (float)j + 1.0, 0.0);
        glVertex3f((float)i + 1.0, (float)j, 0.0);
        glEnd();
      }
    }

    // Draw pawns
    for (int i = 0; i < 8; i++)
    {
      // white pieces
      glColor3ub(140, 140, 135);
      glPushMatrix();
        glTranslatef(i+0.5, 1.5, 0.0);
        drawPawn();
      glPopMatrix();

      // black pieces
      glColor3ub(150, 75, 0);
      glPushMatrix();
        glTranslatef(i+0.5, 6.5, 0.0);
        drawPawn();
      glPopMatrix();
    }

    // Draw rooks, knights, and bishops
    for (int i = 0; i < 2; i++)
    {
      // draw bishops
      // white pieces
      glColor3ub(140, 140, 135);
      glPushMatrix();
        glTranslatef(i*7+0.5, 0.5, 0.0);
        drawRook();
      glPopMatrix();

      // black pieces
      glColor3ub(150, 75, 0);
      glPushMatrix();
        glTranslatef(i*7+0.5, 7.5, 0.0);
        drawRook();
      glPopMatrix();

      // Draw knights
      // white pieces
      glColor3ub(140, 140, 135);
      glPushMatrix();
        glTranslatef(i*5+1.5, 0.5, 0.0);
        drawKnight();
      glPopMatrix();

      // black pieces
      glColor3ub(150, 75, 0);
      glPushMatrix();
        glTranslatef(i*5+1.5, 7.5, 0.0);
        drawKnight();
      glPopMatrix();

      // Draw bishops
      // white pieces
      glColor3ub(140, 140, 135);
      glPushMatrix();
        glTranslatef(i*3+2.5, 0.5, 0.0);
        drawBishop();
      glPopMatrix();

      // black pieces
      glColor3ub(150, 75, 0);
      glPushMatrix();
        glTranslatef(i*3+2.5, 7.5, 0.0);
        drawBishop();
      glPopMatrix();
    }

    // draw white queen
    glColor3ub(140, 140, 135);
    glPushMatrix();
      glTranslatef(3.5, 0.5, 0.0);
      drawQueen();
    glPopMatrix();

    // draw black queen
    glColor3ub(150, 75, 0);
    glPushMatrix();
      glTranslatef(3.5, 7.5, 0.0);
      drawQueen();
    glPopMatrix();

    // draw white king
    glColor3ub(140, 140, 135);
    glPushMatrix();
      glTranslatef(4.5, 0.5, 0.0);
      drawKing();
    glPopMatrix();

    // draw black king
    glColor3ub(150, 75, 0);
    glPushMatrix();
      glTranslatef(4.5, 7.5, 0.0);
      drawKing();
    glPopMatrix();

    // Make it all visible
    glutSwapBuffers();
  }

  //----------------------------------------------------------------------
  // User-input callbacks
  //
  // Q: quit program
  // R: rotate board 10 degrees
  // D: move eye location down
  // U: move eye location up
  // 0: toggle light 0
  // 1: toggle light 1
  //----------------------------------------------------------------------
  void processNormalKeys(unsigned char key, int xx, int yy)
  {
    if (key == ESC || key == 'q' || key == 'Q')
    {
      exit(0);
    }
    else if (key == 'r' || key == 'R')
    {
      deltaBoardAngle = 10.0;
    }
    else if (key == 'd' || key == 'D')
    {
      deltaEyeLocation = -0.25;
    }
    else if (key == 'u' || key == 'U')
    {
      deltaEyeLocation = 0.25;
    }
    else if (key == '0')
    {
      light0Toggled = !light0Toggled;
    }
    else if (key == '1')
    {
      light1Toggled = !light1Toggled;
    }
  }


  //----------------------------------------------------------------------
  // Main program  - standard GLUT initializations and callbacks
  //----------------------------------------------------------------------
  int main(int argc, char **argv)
  {
    printf("\n\
    -----------------------------------------------------------------------\n\
    OpenGL Sample Program:\n\
    - Drag mouse left-right to rotate camera\n\
    - Hold up-arrow/down-arrow to move camera forward/backward\n\
    - q or ESC to quit\n\
    -----------------------------------------------------------------------\n");

    // general initializations
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Chessboard");

    // register callbacks
    glutReshapeFunc(changeSize); // window reshape callback
    //init();
    glutDisplayFunc(renderScene); // (re)display callback
    glutIdleFunc(update); // incremental update
    glutIgnoreKeyRepeat(1); // ignore key repeat when holding key down
    glutKeyboardFunc(processNormalKeys); // process standard key clicks

    // // OpenGL init
    // glEnable(GL_LIGHTING);
    // glEnable(GL_LIGHT0);
    // glEnable(GL_LIGHT1);
    // glEnable(GL_COLOR_MATERIAL);
    // glEnable(GL_DEPTH_TEST);

    // enter GLUT event processing cycle
    glutMainLoop();

    return 0; // this is just to keep the compiler happy
  }
