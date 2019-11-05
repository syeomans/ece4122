/*
Author: Samuel Yeomans
Class: ECE 4122
Date: November 5, 2019
Description:
Battlestar Buzzy has just finished defending GaTech from the evil space
bulldogs. It was a fierce battle and only seven yellow jackets are still
operational and need to dock as soon as possible. The yellow jackets are all
damaged, flying blind, and having propulsion issues.

This script is a distributed MPI program that is able to guide the yellow
jackets safely back to Buzzy so that they can dock.
*/

/*
Pseudocode

Buzzy loop:
Send update to all 7 yellow jackets
Get responses from all 7 yellow jackets
Check that at least 1 jacket is active
Move buzzy
Print message
Check if all jackets are docked


Jackets loop:
Get message from Buzzy
Find distances from 6 other jackets
Check for a collision   <-- updates status
Check if docked     <-- updates status
Fly()   <-- updates acceleration and velocity
Move Yellow Jacket <-- updates position
Send response to Buzzy


fly()
{
if close to buzzy:
  match buzzy's velocity
  stay back [dockingThreshold] meters
if readyToDock:
  move toward [dockingThreshold]/2 meters behind buzzy
  then move toward buzzy until docked
else:
  move forward at full speed
}

*/


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include "mpi.h"
#include <stddef.h>
#include <limits>
#include <iomanip>
#include <cmath>
#include <cstdlib>

using namespace std;

// Custom struct to store relevant ship data
typedef struct ship {
  int rankId;             // 0: Buzzy, 1-7: Yellow Jackets
  int status = 1;         // 0: crashed, 1: active, 2: docked
  float position[3];      // [x, y, z] location vector
  float velocity[3];      // [x, y, z] speed vector
  float acceleration[3];  // [x, y, z] acceleration vector
} ship;

int main(int argc, char**argv)
{
  int  numtasks, rank, rc, nTimeSteps;
  float maxThrust;
  int yellowJacketMass = 10000; // mass in kg
  ship ships[8];
  const int tag = 13;

  rc = MPI_Init(&argc, &argv);

  if (rc != MPI_SUCCESS)
  {
    printf("Error starting MPI program. Terminating.\n");
    MPI_Abort(MPI_COMM_WORLD, rc);
  }

  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Make ship strict an MPI struct
  const int    nitems=5;
  int          blocklengths[5] = {1, 1, 3, 3, 3};
  MPI_Datatype types[5] = {MPI_INT, MPI_INT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT};
  MPI_Datatype mpi_ship_type;
  MPI_Aint     offsets[5];
  offsets[0] = offsetof(ship, rankId);
  offsets[1] = offsetof(ship, status);
  offsets[2] = offsetof(ship, position);
  offsets[3] = offsetof(ship, velocity);
  offsets[4] = offsetof(ship, acceleration);
  MPI_Type_struct(nitems, blocklengths, offsets, types, &mpi_ship_type);
  MPI_Type_commit(&mpi_ship_type);

  // Get the global data from input file
  string line;
  ifstream myfile ("in.dat");
  int lineCount = 1;
  if (myfile.is_open())
  while ( getline (myfile,line) )
  {
    // line 1: seconds before Buzzy jumps to ludicrous speed
    if (lineCount == 1)
    {
      nTimeSteps = stoi(line);
    }
    // line 2: maximum thrust of each yellow jacket
    else if (lineCount == 2)
    {
      maxThrust = stof(line);
    }
    lineCount++;
  }
  myfile.close();

  // Seed the random number generator to get different results each time
  srand(rank);

  // Main thread reads input file to initialize the array of ships
  if (rank == 0)
  {
    // Load in.dat file
    string line;
    ifstream myfile ("in.dat");
    vector<string> dataVector;
    int lineCount = 1;
    if (myfile.is_open())
    {
      while ( getline (myfile,line) )
      {
        // line 1: seconds before Buzzy jumps to ludicrous speed
        if (lineCount == 1)
        {
          nTimeSteps = stoi(line);
        }
        // line 2: maximum thrust of each yellow jacket
        else if (lineCount == 2)
        {
          maxThrust = stof(line);
        }
        // lines 3+: direction and speed of each ship
        else
        {
          // Create a vector of strings to store data
          dataVector.clear();
          string word = "";
          for (auto x : line)
          {
            if (x == ' ')
            {
              dataVector.push_back(word);
              word = "";
            }
            else
            {
              word = word + x;
            }
          }
          dataVector.push_back(word);

          // Fill ships with data
          for (int i=0; i<3; i++)
          {
            // Force is 0 because thrusters are not running yet
            ships[lineCount-3].acceleration[i] = 0.0;
            // Position is given in the input data
            ships[lineCount-3].position[i] = stof(dataVector[i]);
            // Velocity in the xyz direction is velocity * direction vector
            ships[lineCount-3].velocity[i] = stof(dataVector[3]) * stof(dataVector[4+i]);
            // rankId is also the index of this ship in ships array
            ships[lineCount-3].rankId = lineCount-3;
          }
        }
        lineCount++;
      }
      myfile.close();
    }
    else cout << "Unable to open file";
  }

  // Variable declarations
  bool myTurn = false;
  bool readyToDock = false;


  // Main loop
  for (int round = 0; round < nTimeSteps; ++round)
  {

    //// Buzzy controls
    if (rank == 0)
    {
      // Buzzy inits
      MPI_Status status;

      // Send and receive with each ship
      for (int i=1; i < 8; i++)
      {
        // Send to this ship
        rc = MPI_Send(&ships, 8, mpi_ship_type, i, 0, MPI_COMM_WORLD);
        // Error checking
        if (rc != MPI_SUCCESS)
        {
          cout << "Rank " << rank
          << " send failed, rc " << rc << endl;
          MPI_Finalize();
          exit(1);
        }

        // Receive from each ship
        rc = MPI_Recv(&ships[i], 1, mpi_ship_type, i, 0, MPI_COMM_WORLD, &status);
        // Error checking
        if (rc != MPI_SUCCESS)
        {
          MPI_Finalize();
          exit(1);
        }
      }

      // Move buzzy
      for (int i=0; i<3; i++)
      {
        // x = x0 + v0*t + 0.5*a*t^2, where t=1 and a=0
        ships[0].position[i] = ships[0].position[i] + ships[0].velocity[i];
      }

      // Print message
      cout << endl << "round " << round << ":" << endl;
      cout << "rankID, status, x, y, z, Fx, Fy, Fz" << endl;
      for (int i=0; i<8; i++)
      {
        cout << scientific << setprecision(3) << ships[i].rankId << ", " << ships[i].status;
        for (int j=0; j<3; j++)
        {
          cout << ", " << ships[i].position[j];
        }
        for (int j=0; j<3; j++)
        {
          cout << ", " << ships[i].acceleration[j]*yellowJacketMass;
        }
        cout << endl;
      }

      // Check that at least 1 jacket is active
      int nonActiveCount = 0;
      for (int i=1; i<8; i++)
      {
        if (ships[i].status != 1)
          nonActiveCount ++;
      }
      // Stop the loop if all yellow jackets are inactive
      if (nonActiveCount == 7)
      {
        cout << endl << "Success! Stopping threads..." << endl << endl;
        MPI_Abort(MPI_COMM_WORLD, MPI_SUCCESS);
        break;
      }
    }



    //// Yellow Jacket controls
    else
    {
      MPI_Status status;

      // Receive message from Buzzy
      rc = MPI_Recv(&ships, 8, mpi_ship_type, 0, 0, MPI_COMM_WORLD, &status);
      // Error chcecking
      if (rc != MPI_SUCCESS)
      {
        cout << "Rank " << rank << " recv failed, rc " << rc << endl;
        MPI_Finalize();
        exit(1);
      }

      // Find distances from 6 other jackets
      float distances[8];
      for (int i=0; i<8; i++)
      {
        // Distance is magnitude of vector: sqrt( (x-x1)^2 + (y-y1)^2 + (z-z1)^2 )
        distances[i] = 0.0;
        for (int j=0; j<3; j++)
        {
          distances[i] += pow((ships[i].position[j] - ships[rank].position[j]),2.0);
        }
        distances[i] = sqrt(distances[i]);
      }

      // Check for a collision with another active Jacket
      for (int i=1; i<8; i++)
      {
        if (distances[i] < 250 && i != rank && ships[i].status == 1 && ships[rank].status == 1)
        {
          ships[rank].status = 0;
        }
      }

      // Check if docked
      if (distances[0] < 50)
      {
        float angle;
        float jacketVMag; // magnitude of velocity vector
        float buzzyVMag; // magnitude of velocity vector for buzzy
        float dotProduct; // scalar product of jacketVMag and buzzyVMag

        // Find dot product
        dotProduct = 0.0;
        for (int i=0; i<3; i++)
        {
          dotProduct += ships[0].velocity[i] * ships[rank].velocity[i];
        }

        // Find the magnitudes of Buzzy and ship's velocity vectors:
        // sqrt( (x-x1)^2 + (y-y1)^2 + (z-z1)^2 )
        buzzyVMag = 0.0;
        jacketVMag = 0.0;
        for (int i=0; i<3; i++)
        {
          buzzyVMag += pow(ships[0].velocity[i],2.0);
          jacketVMag += pow(ships[rank].velocity[i],2.0);
        }
        buzzyVMag = sqrt(buzzyVMag);
        jacketVMag = sqrt(jacketVMag);

        // Find the angle between Buzzy and the Yellow Jacket
        angle = dotProduct / (buzzyVMag * jacketVMag);

        // angle must be < 0.8 and jacketVMag must be < 1.1*buzzyVMag
        if (angle > 0.8 && jacketVMag < 1.1 * buzzyVMag)
        {
          ships[rank].status = 2;
        }
        // or else the jacket crashed into Buzzy
        else
        {
          ships[rank].status = 0;
        }
      }

      // Choose a direction to fly (update acceleration)
      if (ships[rank].status == 1)
      {
        // Check if it's this ship's turn to dock
        myTurn = true;
        for (int i=1; i<rank; i++)
        {
          if (ships[i].status == 1)
          {
            myTurn = false;
            break;
          }
        }

        // My turn, but not ready to dock
        if (myTurn == true && readyToDock == false)
        {
          // Move to a point 300 feet behind Buzzy
          int velocityGoodCount = 0;
          ships[0].position[2] -= 300;
          for (int i=0; i<3; i++)
          {
            // Rearranged kinematics formula to solve for acceleration: a = 2(x - x0 - v0)
            float idealThrust = 2*(ships[0].position[i]-ships[rank].position[i]-ships[rank].velocity[i]);
            ships[rank].acceleration[i] = min(idealThrust, maxThrust)*0.8;
          }
          // Check if the jacket's position matches Buzzy's in the x and y directions
          float xDistance = abs(ships[0].position[0] - ships[rank].position[0]);
          float yDistance = abs(ships[0].position[1] - ships[rank].position[1]);
          float zDistance = abs(ships[0].position[2] - ships[rank].position[2]);
          // If the jacket's position matches Buzzy's close enough, we're ready to dock
          if (xDistance < cbrt(50) && yDistance < cbrt(50) && zDistance < 300)
          {
            readyToDock = true;
          }
        }

        // My turn and ready to dock
        else if(myTurn == true && readyToDock == true)
        {
          // Find the magnitudes of each ship's velocity
          float buzzyVMag = 0.0;
          float jacketVMag = 0.0;
          for (int i=0; i<3; i++)
          {
            buzzyVMag += pow(ships[0].velocity[i],2.0);
            jacketVMag += pow(ships[rank].velocity[i],2.0);
          }
          buzzyVMag = sqrt(buzzyVMag);
          jacketVMag = sqrt(jacketVMag);

          // Match Buzzy's speed and attempt to dock
          for (int i=0; i<3; i++)
          {
            float idealThrust;

            // Rearranged kinematics formula to solve for acceleration: a = v - v0
            if (i == 2)
            {
              // z direction has to be less than 1.1 * Buzzy's, or else we crash
              idealThrust = ships[0].velocity[i]*1.08 - ships[rank].velocity[i];
            }
            else
            {
              idealThrust = ships[0].velocity[i] - ships[rank].velocity[i];
            }

            // Update acceleration
            ships[rank].acceleration[i] = min(idealThrust, maxThrust);
          }
        }

        // Not my turn yet
        else{
          // Stay 1000 meters behind Buzzy
          ships[0].position[2] -= 1000;
          // Match Buzzy's xyzVelocity.
          float idealThrust = 2*(ships[0].position[2]-ships[rank].position[2]-ships[rank].velocity[2]);
          ships[rank].acceleration[2] = min(idealThrust, maxThrust)*0.8;
          ships[rank].acceleration[1] = 0.0;
          ships[rank].acceleration[0] = 0.0;
        }

        // Update position
        for (int i=0; i<3; i++)
        {
          // Scale acceleration by a random number between 0.8 and 1.2 to simulate damaged thrusters
          float randFloat = 0.8 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(1.2-0.8)));
          ships[rank].acceleration[i] *= randFloat;

          // Update position
          // x = x0 + v0*t + 0.5*a*t^2, where t=1
          ships[rank].position[i] += ships[rank].velocity[i];
          ships[rank].position[i] += 0.5 * ships[rank].acceleration[i];

          // Update velocity
          // v_x = v_x0 + a_x*t, where t = 1
          ships[rank].velocity[i] += ships[rank].acceleration[i];
        }
      }

      // Send message to Buzzy
      rc = MPI_Send(&ships[rank], 1, mpi_ship_type, 0, 0, MPI_COMM_WORLD);
      // Error checking
      if (rc != MPI_SUCCESS)
      {
        cout << "Rank " << rank
        << " send failed, rc " << rc << endl;
        MPI_Finalize();
        exit(1);
      }
    }
  }

  MPI_Finalize();
  exit(1);

}
