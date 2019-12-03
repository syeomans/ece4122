/*
Author: Samuel Yeomans
Class: ECE 4122
Date: December 3, 2019

Description:

The company you work for has been selected to develop a half-time show using
UAVs. You need to develop a 3D simulation using MPI and OpenGL to demo the
show to the game organizers for their approval.
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "iomanip"
#include <cmath>
#include <math.h>
#include <cstdlib>
#include <GL/glut.h>
#include <chrono>
#include <thread>
using namespace std;

// Send location, velocity, and acceleration vector in each direction
const int numElements = 6; // x, y, z, vx, vy, vz, ax, ay, az

const int rcvSize = 16 * numElements; // (Main task + 15 UAVs) * numElements

double* rcvBuffer = new double[rcvSize];

double sendBuffer[numElements];


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
  gluPerspective(60.0, ratio, 0.1, 1000.0); // perspective transformation
  glMatrixMode(GL_MODELVIEW); // return to modelview mode
  glViewport(0, 0, w, h); // set viewport (drawing area) to entire window
}
//----------------------------------------------------------------------
// Draw the football field
//
// Draw a green rectangle to visualize the ground and a wire sphere
// to visualize the path of the UAVs 
//----------------------------------------------------------------------
void displayFootballField()
{
  // Draw football field slightly bigger than necessary for looks
  glColor3f(0.0, 0.5, 0.0);
  glBegin(GL_QUADS);
  glVertex3f(60.0, 30, 0.0);
  glVertex3f(60.0, -30, 0.0);
  glVertex3f(-60.0, -30, 0.0);
  glVertex3f(-60.0, 30, 0.0);
  glEnd();

  // Draw wire sphere of radius 10 at (0, 0, 50)
  glPushMatrix();
    glTranslatef(0.0, 0.0, 50.0);
    glutWireSphere(10.0, 20, 16);
  glPopMatrix();
}
//----------------------------------------------------------------------
// Draw the UAVs
//
// Draw each yellow teapot from their position data
//----------------------------------------------------------------------
void drawUAVs()
{
  glColor3ub(255, 255, 0);
  for (int i=0; i<15; i++)
  {
    // Get xyz location
    float xPosition = rcvBuffer[(i+1)*numElements];
    float yPosition = rcvBuffer[(i+1)*numElements + 1];
    float zPosition = rcvBuffer[(i+1)*numElements + 2];

    // Draw a yellow teapot at specified location
    glPushMatrix();
      glTranslatef(xPosition, yPosition, zPosition);
      glRotatef(90.0, 1.0, 0.0, 0.0);
      glScalef(2.0, 2.0, 2.0); // doubling size for visibility
      glutSolidTeapot(0.5);
    glPopMatrix();
  }
}
//----------------------------------------------------------------------
// Draw the entire scene
//
// We first update the camera location based on its distance from the
// origin and its direction.
//----------------------------------------------------------------------
void renderScene()
{

  // Clear color and depth buffers
  glClearColor(0.0, 1.0, 0.0, 1.0); // background color to green??
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Reset transformations
  glLoadIdentity();

  // Eye location and direction
  float eye_x = 0.0;
  float eye_y = -90.0;
  float eye_z = 75.0;
  float center_x = 0.0;
  float center_y = 0.0;
  float center_z = 25.0;

  gluLookAt(eye_x, eye_y, eye_z,
    center_x, center_y, center_z,
    0.0, 0.0, 1.0);

    glMatrixMode(GL_MODELVIEW);

    displayFootballField();

    drawUAVs();

    glutSwapBuffers(); // Make it all visible

  }
  //----------------------------------------------------------------------
  // Calculate the location of the given UAV
  //----------------------------------------------------------------------
  void calcualteUAVsLocation(int rank, bool hitSphere)
  {
    double sphereCenter[3] = { 0.0, 0.0, 50.0 };
    double distanceVec[3] = { 0.0, 0.0, 0.0 };
    double distanceMag = 0.0;
    double gravityVec[3] = { 0.0, 0.0, -9.8 };
    double springVec[3] = { 0.0, 0.0, 0.0 };
    double randomVec[3] = { 0.0, 0.0, 0.0 };
    double initialForceVec[3] = { 0.0, 0.0, 0.0 };
    double initialForceMag = 0.0;
    double brake[3] = { 0.0, 0.0, 0.0 };
    double springConstant = 100.0;
    double randMin = 0.0;
    double randMax = 10.0;
    double sphereRadius = 10.0;

    // Set speed limit
    double speedLimit;
    if (hitSphere)
      speedLimit = 10;
    else
      speedLimit = 2;

    // Calculate the distance from the center of the sphere
    for (int i=0; i<3; i++)
    {
      double x = sphereCenter[i];
      double x0 = rcvBuffer[rank*numElements + i];
      distanceVec[i] = x - x0;
      distanceMag += pow(x - x0, 2);
    }
    distanceMag = sqrt(distanceMag);

    // Check for a collision with another teapot
    for (int i=numElements; i<rcvSize; i+=numElements)
    {
      // Don't check for collision with self
      if (i/numElements == rank)
        continue;

      // Magnitude of distance = sqrt( (x-x0)^2 + (y-y0)^2 + (z-z0)^2 )
      double distanceFromTeapotMag = 0.0;
      double distanceFromTeapot;
      for (int j=0; j<3; j++)
      {
        distanceFromTeapot = rcvBuffer[i + j] - rcvBuffer[rank*numElements + j];
        distanceFromTeapotMag += pow(distanceFromTeapot, 2);
      }
      distanceFromTeapotMag = sqrt(distanceFromTeapotMag);

      // If any teapot is closer than 1 meter (distance between centers of 2
      // bounding boxes) + 1 cm (given tolerance), swap their velocities
      if (distanceFromTeapotMag <= 1.01)
      {
        for (int j=3; j<6; j++)
        {
          // Swap this UAV's velocity with the other's
          // (the other parallel process does the same before allgather called)
          rcvBuffer[rank*numElements + j] = rcvBuffer[i + j];
        }
      }
    } // end collision check

    // If we've touched the surface of the sphere at least once, travel along
    // the outside of the sphere as if a spring was attached to a point on the
    // sphere that's closest to the UAV
    if (hitSphere)
    {
      for (int i=0; i< 3; i++)
      {
        // The point on the outside of the sphere closest to the UAV
        // is given by: [unit distance vector]*sphereRadius + sphereCenter
        double x = -1.0*distanceVec[i]/distanceMag*sphereRadius + sphereCenter[i];
        double x0 = rcvBuffer[rank*numElements + i];
        double v0 = rcvBuffer[rank*numElements + 3 + i];

        // Spring force F = k*s where k is a spring constant and s is the
        // distance stretched/compressed
        springVec[i] = springConstant * (x - x0);

        // Move in a random direction biased toward the direction of travel
        // Generate random double between randMin and randMax
        double randNum = (double)rand() / RAND_MAX;
        randNum = randMin + randNum * (randMax - randMin);
        // Bias toward the direction of travel
        double travelDirection;
        if (v0 > 0.0)
          travelDirection = 1.0;
        else
          travelDirection = -1.0; // Either 1 or -1
        // Update random movement vector
        randomVec[i] = randNum * travelDirection;
      }
    }

    // If we've yet to touch the surface of the sphere, move toward the center
    // of the sphere at 2 m/s
    else
    {
      // Get the direction and magnitude info of the acceleration vector
      for (int i=0; i<3; i++)
      {
        double v = 2.0;
        double x = sphereCenter[i];
        double v0 = rcvBuffer[rank*numElements + 3 + i];
        double x0 = rcvBuffer[rank*numElements + i];

        // Magnitude given by v = v0 + a*t
        // Solved for a if t=0.1: a = 10(v - v0)
        initialForceMag += pow(10.0*(v - v0), 2);
      }
      initialForceMag = sqrt(initialForceMag);

      // Create resulting acceleration vector
      for (int i=0; i<3; i++)
      {
        initialForceVec[i] = distanceVec[i]/distanceMag * initialForceMag;
        initialForceVec[i] *= 0.5; // This damping factor helps. No idea why.
      }
    }

    // Brake if going too fast
    for (int i=0; i<3; i++)
    {
      double v = speedLimit;
      double x = sphereCenter[i];
      double v0 = rcvBuffer[rank*numElements + 3 + i];
      double x0 = rcvBuffer[rank*numElements + i];

      // If going too fast, accelerate backwards and throw out all other
      // sources of controlled acceleration (i.e., not gravity)
      if (abs(v0) > speedLimit)
      {
        // Force is given by v = v0 + a*t
        // Solved for a if t=0.1: a = 10(v - v0)
        brake[i] = 10.0*(v - v0);
        randomVec[i] = 0.0;
        springVec[i] = 0.0;
        initialForceVec[i] = 0.0;
      }
    }

    // Calculate total force in each direction and update sendBuffer's data
    for (int i=0; i<3; i++)
    {
      double v0 = rcvBuffer[rank*numElements + 3 + i];
      double x0 = rcvBuffer[rank*numElements + i];

      double idealAcceleration = initialForceVec[i] - gravityVec[i] + brake[i]
                                  + springVec[i] + randomVec[i];

      // Bind the resulting acceleration vector between -20 and 20
      double actualAcceleration = max(min(idealAcceleration, 20.0),-20.0);

      // Gravity pulls the UAV down at 9.8 m/s^2
      actualAcceleration += gravityVec[i];

      // Update velocity: v = v0 + a*t
      sendBuffer[i+3] = v0 + actualAcceleration*0.1;

      // Update position: x = x0 + v0*t + 0.5*a*t^2
      sendBuffer[i] = x0 + v0/10.0 + actualAcceleration/200.0;
    }
  }
  //----------------------------------------------------------------------
  // timerFunction  - called whenever the timer fires
  //----------------------------------------------------------------------
  void timerFunction(int id)
  {
    glutPostRedisplay();
    glutTimerFunc(100, timerFunction, 0);
    MPI_Allgather(sendBuffer, numElements, MPI_DOUBLE,
                  rcvBuffer, numElements, MPI_DOUBLE,
                  MPI_COMM_WORLD);
  }
  //----------------------------------------------------------------------
  // mainOpenGL  - standard GLUT initializations and callbacks
  //----------------------------------------------------------------------
  void mainOpenGL(int argc, char**argv)
  {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(400, 400);

    glutCreateWindow(argv[0]);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Setup lights as needed
    // ...

    glutReshapeFunc(changeSize);
    glutDisplayFunc(renderScene);
    glutTimerFunc(100, timerFunction, 0);
    glutMainLoop();
  }
  //////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////
  // Main entry point determines rank of the process and follows the
  // correct program path
  //////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////
  int main(int argc, char**argv)

  {

    int numTasks, rank;

    int rc = MPI_Init(&argc, &argv);

    if (rc != MPI_SUCCESS)
    {
      printf("Error starting MPI program. Terminating.\n");
      MPI_Abort(MPI_COMM_WORLD, rc);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &numTasks);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int gsize = 0;

    MPI_Comm_size(MPI_COMM_WORLD, &gsize);

    srand (rank);

    // Initialize rcvBuffer to contain the starting positions of each UAV
    for (int i=0; i<15; i++) // range: i from 0-14
    {
      // x-position mapping function: y = 25x - 50
      rcvBuffer[(i+1)*numElements] = 25.0*(float)(i%5) - 50.0;
      // y-position mapping function: y = -26.5x + 26.5
      rcvBuffer[(i+1)*numElements + 1] = -26.5*(float)floor(i/5) + 26.5;
      // z-position is 0 (on the ground)
      rcvBuffer[(i+1)*numElements + 2] = 0.0;
    }

    if (rank == 0)
    {
      mainOpenGL(argc, argv);
    }
    else
    {
      bool hitSphere = false;
      // Sleep for 5 seconds
      std::this_thread::sleep_for(std::chrono::seconds(5));

      // Run for 1000 cycles
      for (int ii = 0; ii < 1000 ; ii++)
      {
        // Check if hit sphere
        if (hitSphere == false)
        {
          // Find distance from center of sphere
          double xDist = rcvBuffer[rank*numElements];
          double yDist = rcvBuffer[rank*numElements+1];
          double zDist = rcvBuffer[rank*numElements+2] - 50;
          double distFromSphereCenter = sqrt( pow(xDist, 2) + pow(yDist, 2)
                                              + pow(zDist, 2) );
          // If distance < 10, we've hit the sphere
          if (distFromSphereCenter < 10.0)
          {
            hitSphere = true;
          }
        }
        calcualteUAVsLocation(rank, hitSphere);
        MPI_Allgather(sendBuffer, numElements, MPI_DOUBLE,
                      rcvBuffer, numElements, MPI_DOUBLE,
                      MPI_COMM_WORLD);
      }
    }
    return 0;
  }
