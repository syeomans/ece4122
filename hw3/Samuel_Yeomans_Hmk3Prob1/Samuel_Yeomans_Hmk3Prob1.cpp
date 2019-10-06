/*
Author: Samuel Yeomans
Class: ECE 4122
Date: October 6, 2019

Description:
Console program that takes as command line arguments the height and width
of a lattice. Uses multithreading to determine the number of possible paths
through the lattice quickly.

Input parameters (command line arguments):
-h: height of the lattice
-w: width of the lattice
Example for a lattice 20 high and 10 wide: "Problem1.exe –h 20 –w 10"

Outputs to the console the number of paths exactly like the following:
"Number of Routes: 30045015"
*/

#include <bits/stdc++.h>
#include <thread>
#include <future>
using namespace std;

/*
Function to find the number of paths through the lattice.

Every path through the lattice takes [height] moves down and
[width] moves right, so the number of paths is the number of permutations of
[height] moves down and [width] moves right. A closed form solution to this
problem exists.

Closed form solution: (height + width)! / (height! * width!)
This formula reduces to:
product(numbers from h+w to max(h, w)+1) / product(numbers from min(h, w) to 1)

Input parameters:
int height: height of the lattice
int width: width of the lattice

Returns the number of paths in the lattice as a long int
*/
long int findPaths(int height, int width)
{
    // Initialize numerator and denominator to 1
    double numerator = 1;
    double denominator = 1;

    // Numerator is the product of all integers from h+w to max(h, w)+1
    for (int i=height+width; i>max(height,width); i--)
        numerator *= i;
    // Denominator is the product of all integers from min(h, w) to 1
    for (int i=min(height, width); i>0; i--)
        denominator *= i;

    // Divide numerator and denominator doubles. Format output as a long int.
    long int output = static_cast<long int>(numerator/denominator);
    return(output);
}

// Entry point into the application
int main(int argc, char* argv[])
{
    // Check if the number of arguments is correct
    if (argc != 5)
    {
        cout << "Incorrect number of arguments" << endl;
        return 1;
    }

    // Get the height and width of the lattice from the command line arguments
    int height, width;
    for (int i=0; i<argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0)
            height = stoi(argv[i+1]);
        if (strcmp(argv[i], "-w") == 0)
            width = stoi(argv[i+1]);
    }

    // Spawn a thread to perform the calculation, return the number of routes
    auto future = async(findPaths, height, width);
    long int routes = future.get();

    // Output number of routes to the console
    cout << "Number of Routes: " << routes << endl;

    return 0;
}
