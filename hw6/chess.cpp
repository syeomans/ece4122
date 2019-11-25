/*
Author: Samuel Yeomans
Class: ECE 4122
Date: November 22, 2019
Description:
3D chess set model using OpenGL
*/

#include <iostream>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
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

// Board rotation
float boardAngle = 0.0;
float deltaBoardAngle = 0.0;

// Eye location
float eyeLocationZ = 10.0;
float deltaEyeLocation = 0.0;

// Lighting toggle
bool light1Toggle = true;
bool light0Toggle = true;

//----------------------------------------------------------------------
// Initialize lighting and materials
//----------------------------------------------------------------------
void init(void)
{

  // Enable lighting
  glEnable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);

  // Material properties
  GLfloat mat_specular[] = { 0.5, 0.5, 0.5, 1.0 };
  GLfloat mat_shininess[] = { 50.0 };
  glShadeModel(GL_SMOOTH);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

  // Lighting properties
  GLfloat lightAmbient[] = { 0.2, 0.2, 0.2, 1.0 };
  GLfloat lightDiffuse[] = { 0.5, 0.5, 0.5, 1.0 };
  GLfloat lightSpecular[] = { 0.3, 0.3, 0.3, 1.0 };
  GLfloat spotDirection[] = { 5.0, 5.0, 8.0 };
  GLfloat turnOff[] = {0.0, 0.0, 0.0};
  glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, turnOff);
  glLightfv(GL_LIGHT0, GL_SPECULAR, turnOff);
  glLightfv(GL_LIGHT1, GL_AMBIENT, turnOff);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular);
  glLightfv(GL_LIGHT1, GL_POSITION, spotDirection);
}

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

// Pawn
void drawPawn()
{
  glPushMatrix();
    glTranslatef(0.0, 0.0, 0.5);
    glutSolidSphere(0.5, 20, 16);
    glScalef(0.75, 0.75, 1.0);
  glPopMatrix();
}

// Rook
void drawRook()
{
  glPushMatrix();
    glTranslatef(0.0, 0.0, 0.4);
    glutSolidCube(0.75);
    glScalef(0.5, 0.5, 0.5);
  glPopMatrix();
}

// Knight
void drawKnight()
{
  glPushMatrix();
    glTranslatef(0.0, 0.0, 0.5*0.75+0.01);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glScalef(0.5, 1.0, 0.5);
    glutSolidTeapot(0.5);
  glPopMatrix();
}

// Bishop
void drawBishop()
{
  glPushMatrix();
    glTranslatef(0.0, 0.0, 0.0);
    glScalef(0.75, 0.75, 0.75);
    glutSolidCone(0.5, 1.0, 10, 2);
    glScalef(0.5, 0.5, 0.5);
  glPopMatrix();
}

// King
void drawKing()
{
  glPushMatrix();
    glTranslatef(0.0, 0.0, 0.4);
    glScalef(0.4, 0.4, 0.4);
    glutSolidOctahedron();
  glPopMatrix();
}

// Queen
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
  // Clear color and depth buffers
  glClearColor(0.3, 0.4, 0.5, 1.0); // sky color is light blue
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Use lights 0 and 1 if they're toggled on; else, turn them off
  if (light0Toggle)
    glEnable(GL_LIGHT0);
  else
    glDisable(GL_LIGHT0);
  if (light1Toggle)
    glEnable(GL_LIGHT1);
  else
    glDisable(GL_LIGHT1);

  // Reset transformations
  glLoadIdentity();

  // Set the camera at (4, -5, 10) and point toward the center of the board
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
    // Quit if user presses ESC or q
    if (key == ESC || key == 'q' || key == 'Q')
    {
      exit(0);
    }
    // Rotate board 10 degrees if user presses r
    else if (key == 'r' || key == 'R')
    {
      deltaBoardAngle = 10.0;
    }
    // Move board down by 0.25 meters if user presses d
    else if (key == 'd' || key == 'D')
    {
      deltaEyeLocation = -0.25;
    }
    // Move board up by 0.25 meters if user presses u
    else if (key == 'u' || key == 'U')
    {
      deltaEyeLocation = 0.25;
    }
    // Toggle light 0 on/off if user presses 0
    else if (key == '0')
    {
      light0Toggle = !light0Toggle;
    }
    // Toggle light 1 on/off if user presses 1 
    else if (key == '1')
    {
      light1Toggle = !light1Toggle;
    }
  }


  //----------------------------------------------------------------------
  // Main program  - standard GLUT initializations and callbacks
  //----------------------------------------------------------------------
  int main(int argc, char **argv)
  {
    printf("\n\
    -----------------------------------------------------------------------\n\
    OpenGL Chess Board:\n\
    - r to rotate camera\n\
    - 0 to enable light 0\n\
    - 1 to enable light 1\n\
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
    init();
    glutDisplayFunc(renderScene); // (re)display callback
    glutIdleFunc(update); // incremental update
    glutIgnoreKeyRepeat(1); // ignore key repeat when holding key down
    glutKeyboardFunc(processNormalKeys); // process standard key clicks

    // enter GLUT event processing cycle
    glutMainLoop();

    return 0; // this is just to keep the compiler happy
  }
