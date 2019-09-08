/*
Author: Samuel Yeomans
Class: ECE 4122
Date: September 8, 2019

Description:
Console program that takes in a number n from the console and outputs to the
console the number of 1's that must be added to the positive integer n before the process
above ends.
*/

#include <iostream>
using namespace std;

/*
Entry point into the application
*/
int main()
{
  // Declare variables
  int n = 0;
  int counter = 0;

  // Get and check input variable n
  while (n<1)
  {
    // Get user input
    cout << "Please enter the starting number n: ";
    cin >> n;

    // Warn user if input is not positive
    if (n<1)
    {
      cout << "n must be positive\n";
    }
  }

  // Loop through until n=1
  while(n != 1)
  {
    // If n is divisible by 7, divide it by 7
    if (n%7 == 0)
    {
      n = n/7;
    }
    // Otherwise add 1 and increment counter
    else
    {
      n = n+1;
      counter = counter + 1;
    } // End if-else
  } // End while loop

  // Output to the console the number of 1's that were be added
  cout << "The sequence had " << counter << " instances of the number 1 being added\n";

  // Nothing to return in main
  return 0;
}
