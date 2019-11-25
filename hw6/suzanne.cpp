/*
Author: Samuel Yeomans
Class: ECE 4122
Date: November 22, 2019
Description:
3D chess set model using OpenGL
*/

#include <iostream>
// #include <GL/glew.h> // Include the GLEW header file
#include <GL/glut.h> // Include the GLUT header file
// #include <conio.h>
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
float x = 0.0, y = -5.0; // initially 5 units south of origin
float deltaMove = 0.0; // initially camera doesn't move

// Camera direction
float lx = 0.0, ly = 1.0; // camera points initially along y-axis
float angle = 0.0; // angle of rotation for the camera direction
float deltaAngle = 0.0; // additional angle change when dragging

// Mouse drag control
int isDragging = 0; // true when dragging
int xDragStart = 0; // records the x-coordinate when dragging starts

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
  gluPerspective(45.0, ratio, 0.1, 100.0); // perspective transformation
  glMatrixMode(GL_MODELVIEW); // return to modelview mode
  glViewport(0, 0, w, h); // set viewport (drawing area) to entire window
}

//----------------------------------------------------------------------
// Draw one snowmen (at the origin)
//
// A snowman consists of a large body sphere and a smaller head sphere.
// The head sphere has two black eyes and an orange conical nose. To
// better create the impression they are sitting on the ground, we draw
// a fake shadow, consisting of a dark circle under each.
//
// We make extensive use of nested transformations. Everything is drawn
// relative to the origin. The snowman's eyes and nose are positioned
// relative to a head sphere centered at the origin. Then the head is
// translated into its final position. The body is drawn and translated
// into its final position.
//----------------------------------------------------------------------
void drawSnowman()
{
  // Draw body (a 20x20 spherical mesh of radius 0.75 at height 0.75)
  glColor3f(1.0, 1.0, 1.0); // set drawing color to white
  glPushMatrix();
  glTranslatef(0.0, 0.0, 0.75);
  glutSolidSphere(0.75, 20, 20);
  glPopMatrix();

  // Draw the head (a sphere of radius 0.25 at height 1.75)
  glPushMatrix();
  glTranslatef(0.0, 0.0, 1.75); // position head
  glutSolidSphere(0.25, 20, 20); // head sphere

  // Draw Eyes (two small black spheres)
  glColor3f(0.0, 0.0, 0.0); // eyes are black
  glPushMatrix();
  glTranslatef(0.0, -0.18, 0.10); // lift eyes to final position
  glPushMatrix();
  glTranslatef(-0.05, 0.0, 0.0);
  glutSolidSphere(0.05, 10, 10); // right eye
  glPopMatrix();
  glPushMatrix();
  glTranslatef(+0.05, 0.0, 0.0);
  glutSolidSphere(0.05, 10, 10); // left eye
  glPopMatrix();
  glPopMatrix();

  // Draw Nose (the nose is an orange cone)
  glColor3f(1.0, 0.5, 0.5); // nose is orange
  glPushMatrix();
  glRotatef(90.0, 1.0, 0.0, 0.0); // rotate to point along -y
  glutSolidCone(0.08, 0.5, 10, 2); // draw cone
  glPopMatrix();
  glPopMatrix();

  // Draw a faux shadow beneath snow man (dark green circle)
  glColor3f(0.0, 0.5, 0.0);
  glPushMatrix();
  glTranslatef(0.2, 0.2, 0.001);    // translate to just above ground
  glScalef(1.0, 1.0, 0.0); // scale sphere into a flat pancake
  glutSolidSphere(0.75, 20, 20); // shadow same size as body
  glPopMatrix();
}

// void init(void)
// {
//     GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
//     GLfloat mat_shininess[] = { 50.0 };
//     GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
//     glClearColor(0.0, 0.0, 0.0, 0.0);
//     glShadeModel(GL_SMOOTH);
//
//     glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
//     glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
//     glLightfv(GL_LIGHT0, GL_POSITION, light_position);
//
//     glEnable(GL_LIGHTING);
//     glEnable(GL_LIGHT0);
//     glEnable(GL_DEPTH_TEST);
//
//     // GLfloat light0_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
//     // GLfloat light1_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
//     // GLfloat light1_specular[] = { 0.3, 0.3, 0.3, 1.0 };
// }

void load_obj(const char* filename, vector<glm::vec4> &vertices, vector<glm::vec3> &normals, vector<GLushort> &elements)
{
    ifstream in(filename, ios::in);
    if (!in)
    {
        cerr << "Cannot open " << filename << endl; exit(1);
    }

    string line;
    while (getline(in, line))
    {
        if (line.substr(0,2) == "v ")
        {
            istringstream s(line.substr(2));
            glm::vec4 v; s >> v.x; s >> v.y; s >> v.z; v.w = 1.0f;
            vertices.push_back(v);
        }
        else if (line.substr(0,2) == "f ")
        {
            istringstream s(line.substr(2));
            GLushort a,b,c;
            s >> a; s >> b; s >> c;
            a--; b--; c--;
           elements.push_back(a); elements.push_back(b); elements.push_back(c);
        }
        else if (line[0] == '#')
        {
            /* ignoring this line */
        }
        else
        {
            /* ignoring this line */
        }
    }

    normals.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));
    for (int i = 0; i < elements.size(); i+=3)
    {
        GLushort ia = elements[i];
        GLushort ib = elements[i+1];
        GLushort ic = elements[i+2];
        glm::vec3 normal = glm::normalize(glm::cross(
        glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]),
        glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])));
        normals[ia] = normals[ib] = normals[ic] = normal;
    }
}

void drawSuzanne()
{
  vector<glm::vec4> suzanne_vertices;
  vector<glm::vec3> suzanne_normals;
  vector<GLushort> suzanne_elements;
  // [...]
  load_obj("suzanne.obj", suzanne_vertices, suzanne_normals, suzanne_elements);

  glEnableVertexAttribArray(attribute_v_coord);
  // Describe our vertices array to OpenGL (it can't guess its format automatically)
  glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh_vertices);
  glVertexAttribPointer(
    attribute_v_coord,  // attribute
    4,                  // number of elements per vertex, here (x,y,z,w)
    GL_FLOAT,           // the type of each element
    GL_FALSE,           // take our values as-is
    0,                  // no extra data between each position
    0                   // offset of first element
  );

  glEnableVertexAttribArray(attribute_v_normal);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh_normals);
  glVertexAttribPointer(
    attribute_v_normal, // attribute
    3,                  // number of elements per vertex, here (x,y,z)
    GL_FLOAT,           // the type of each element
    GL_FALSE,           // take our values as-is
    0,                  // no extra data between each position
    0                   // offset of first element
  );

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_mesh_elements);
  int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
  glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);


}

void drawPawn()
{
  glPushMatrix();
    glTranslatef(0.0, 0.0, 0.5*0.75);
    glutSolidSphere(0.5, 20, 16);
    glScalef(0.75, 0.75, 1.0);
  glPopMatrix();
}

void drawRook()
{
  glPushMatrix();
    glTranslatef(0.0, 0.0, 0.5*0.75);
    glutSolidCube(0.75);
    glScalef(0.75, 0.75, 1.0);
  glPopMatrix();
}

void drawKnight()
{
  glPushMatrix();
    glTranslatef(0.0, 0.0, 0.5*0.75);
    glutSolidTeapot(0.5);
    glScalef(0.75, 0.75, 1.0);
  glPopMatrix();
}

void drawBishop()
{
  glPushMatrix();
    glTranslatef(0.0, 0.0, 0.0);
    glScalef(0.75, 0.75, 1.0);
    glutSolidCone(0.5, 1.0, 10, 2);
  glPopMatrix();
}

void drawQueen()
{
  glPushMatrix();
    glTranslatef(0.0, 0.0, 0.5);
    glutSolidTetrahedron();
    glScalef(0.1, 0.1, 0.1);
  glPopMatrix();
}

void drawKing()
{
  glPushMatrix();
    glTranslatef(0.0, 0.0, 0.5*0.75);
    glutSolidOctahedron();
    glScalef(0.75, 0.75, 1.0);
  glPopMatrix();
}



//----------------------------------------------------------------------
// Update with each idle event
//
// This incrementally moves the camera and requests that the scene be
// redrawn.
//----------------------------------------------------------------------
void update(void)
{
  if (deltaMove) { // update camera position
    x += deltaMove * lx * 0.1;
    y += deltaMove * ly * 0.1;
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

  GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
  GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
  GLfloat spot_direction[] = { -5.0, -5.0, 8.0 };
  GLfloat turnOff[] = {0.0, 0.0, 0.0};
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT1, GL_POSITION, spot_direction);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, turnOff);
  glLightfv(GL_LIGHT0, GL_SPECULAR, turnOff);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);

  // Clear color and depth buffers
  //glClearColor(0.0, 0.7, 1.0, 1.0); // sky color is light blue
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Reset transformations
  glLoadIdentity();

  // Set the camera centered at (x,y,1) and looking along directional
  // vector (lx, ly, 0), with the z-axis pointing up
  gluLookAt(
    x, y, 1.0,
    x + lx, y + ly, 1.0,
    0.0, 0.0, 1.0);
    // gluLookAt(
    //     x, y, 10.0,
    //     x + lx, y + ly, 0.5,
    //     0.0, 0.0, 1.0);

    // Draw chessboard - 8 x 8 grid of alternating light and dark squares
    for (int i=0; i<8; i++)
    {
      for (int j=0; j<8; j++)
      {
        if ((i+j)%2 == 0)
        {
          glColor3f(0.0, 0.0, 0.0);
        }
        else
        {
          glColor3f(1.0, 1.0, 1.0);
        }
        glBegin(GL_QUADS);
        glVertex3f((float)i, (float)j, 0.0);
        glVertex3f((float)i, (float)j + 1.0, 0.0);
        glVertex3f((float)i + 1.0, (float)j + 1.0, 0.0);
        glVertex3f((float)i + 1.0, (float)j, 0.0);
        glEnd();
      }
    }

    // // Draw pawns
    // for (int i = 0; i < 8; i++)
    // {
    //   // white pieces
    //   glColor3ub(140, 140, 135);
    //   glPushMatrix();
    //     glTranslatef(i+0.5, 1.5, 0.0);
    //     drawPawn();
    //   glPopMatrix();
    //
    //   // black pieces
    //   glColor3ub(150, 75, 0);
    //   glPushMatrix();
    //     glTranslatef(i+0.5, 6.5, 0.0);
    //     drawPawn();
    //   glPopMatrix();
    // }

    // // Draw rooks
    // for (int i = 0; i < 8; i+=7)
    // {
    //   // white pieces
    //   glColor3ub(140, 140, 135);
    //   glPushMatrix();
    //     glTranslatef(i+0.5, 0.5, 0.0);
    //     drawRook();
    //   glPopMatrix();
    //
    //   // black pieces
    //   glColor3ub(150, 75, 0);
    //   glPushMatrix();
    //     glTranslatef(i+0.5, 7.5, 0.0);
    //     drawRook();
    //   glPopMatrix();
    // }
    //
    // // Draw knights
    // for (int i = 1; i < 7; i+=5)
    // {
    //   // white pieces
    //   glColor3ub(140, 140, 135);
    //   glPushMatrix();
    //     glTranslatef(i+0.5, 0.5, 0.0);
    //     drawKnight();
    //   glPopMatrix();
    //
    //   // black pieces
    //   glColor3ub(150, 75, 0);
    //   glPushMatrix();
    //     glTranslatef(i+0.5, 7.5, 0.0);
    //     drawKnight();
    //   glPopMatrix();
    // }
    //
    // // Draw bishops
    // for (int i = 2; i < 6; i+=3)
    // {
    //   // white pieces
    //   glColor3ub(140, 140, 135);
    //   glPushMatrix();
    //     glTranslatef(i+0.5, 0.5, 0.0);
    //     drawBishop();
    //   glPopMatrix();
    //
    //   // black pieces
    //   glColor3ub(150, 75, 0);
    //   glPushMatrix();
    //     glTranslatef(i+0.5, 7.5, 0.0);
    //     drawBishop();
    //   glPopMatrix();
    // }
    //
    // draw white queen
    glColor3ub(140, 140, 135);
    glPushMatrix();
      glTranslatef(3.5, 0.5, 0.0);
      drawQueen();
    glPopMatrix();

    // draw black queen
    glColor3ub(150, 75, 0);
    glPushMatrix();
      glTranslatef(4.5, 7.5, 0.0);
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
      glTranslatef(3.5, 7.5, 0.0);
      drawKing();
    glPopMatrix();


    glutSwapBuffers(); // Make it all visible
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
    if (key == ESC || key == 'q' || key == 'Q')
    {
      exit(0);
    }
  }

  void pressSpecialKey(int key, int xx, int yy)
  {
    switch (key)
    {
      case GLUT_KEY_UP: deltaMove = 1.0; break;
      case GLUT_KEY_DOWN: deltaMove = -1.0; break;
    }
  }

  void releaseSpecialKey(int key, int x, int y)
  {
    switch (key)
    {
      case GLUT_KEY_UP: deltaMove = 0.0; break;
      case GLUT_KEY_DOWN: deltaMove = 0.0; break;
    }
  }

  //----------------------------------------------------------------------
  // Process mouse drag events
  //
  // This is called when dragging motion occurs. The variable
  // angle stores the camera angle at the instance when dragging
  // started, and deltaAngle is a additional angle based on the
  // mouse movement since dragging started.
  //----------------------------------------------------------------------
  void mouseMove(int x, int y)
  {
    if (isDragging)
    { // only when dragging
      // update the change in angle
      deltaAngle = (x - xDragStart) * 0.005;

      // camera's direction is set to angle + deltaAngle
      lx = -sin(angle + deltaAngle);
      ly = cos(angle + deltaAngle);
    }
  }

  void mouseButton(int button, int state, int x, int y)
  {
    if (button == GLUT_LEFT_BUTTON)
    {
      if (state == GLUT_DOWN)
      { // left mouse button pressed
        isDragging = 1; // start dragging
        xDragStart = x; // save x where button first pressed
      }
      else
      { /* (state = GLUT_UP) */
        angle += deltaAngle; // update camera turning angle
        isDragging = 0; // no longer dragging
      }
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
    glutInitWindowSize(800, 400);
    glutCreateWindow("Winter Wonderland");

    // register callbacks
    glutReshapeFunc(changeSize); // window reshape callback
    //init();
    glutDisplayFunc(renderScene); // (re)display callback
    // glutDisplayFunc(chessboard); // (re)display callback
    glutIdleFunc(update); // incremental update
    glutIgnoreKeyRepeat(1); // ignore key repeat when holding key down
    glutMouseFunc(mouseButton); // process mouse button push/release
    glutMotionFunc(mouseMove); // process mouse dragging motion
    glutKeyboardFunc(processNormalKeys); // process standard key clicks
    glutSpecialFunc(pressSpecialKey); // process special key pressed
    // Warning: Nonstandard function! Delete if desired.
    glutSpecialUpFunc(releaseSpecialKey); // process special key release

    // // OpenGL init
    // glEnable(GL_DEPTH_TEST);

    // enter GLUT event processing cycle
    glutMainLoop();

    return 0; // this is just to keep the compiler happy
  }
