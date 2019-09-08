/*
Author: Samuel Yeomans
Class: ECE 4122
Date: September 8, 2019

Description:
Console program to numerically calculate the value of the following integral, using the
“midpoint” rule: (4/(1+x^2))*dx from 0 to beta.

note: the one- and two-character variable names I'm using below meet mathematical
standards. These variables are the only ones in this code that don't conform
to the coding standards of this assignment. 
*/

#include <iostream>
using namespace std;

/*
Entry point into the application
*/
int main()
{
  // Variable declarations
  double beta;
  unsigned long n;
  double outputValue = 0.0;

  // Get input variables (beta and n)
  cout << "Please enter a value for the upper limit (beta): ";
  cin >> beta;
  cout << "Please enter the number of subdivisions to use: ";
  cin >> n;

  // Determine the step size, dx
  double dx = beta/n;

  // Compute function n times. Start at x=0 and run until x=beta.
  double x = 0.0;
  double y;
  while (x < beta)
  {
    y = (4/(1+x*x))*dx; // Function to be computed
    outputValue += y; // Collect cumulating values in outputValue
    x += dx; // Post-increment
  }

  // Print answer to console
  cout << "The integral evaluates to: " << outputValue << "\n";

  // Nothing to return in main
  return 0;
}
