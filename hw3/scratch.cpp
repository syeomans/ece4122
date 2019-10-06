/*
Author: Samuel Yeomans
Class: ECE 4122
Date: October 6, 2019

Description:
Console program that takes as command line arguments the height and width
of a lattice. Uses multithreading to determine the number of possible paths
through the lattice quickly.

Closed form solution: (height + width)! / (height! * width!)

Input parameters (command line arguments):
-h: height of the lattice
-w: width of the lattice
Example for a lattice 20 high and 10 wide: "Problem1.exe –h 20 –w 10"

Outputs to the console the number of paths exactly like the following:
"Number of Routes: 30045015"
*/

#include <iostream>
#include <iomanip>
#include <omp.h>
#include <string.h>
#include <vector>
#include <bits/stdc++.h>
using namespace std;

/*
Find all possible permutations of a given string (a) and store in the
destination vector (b), passed by reference.

Input parameters
string a: string to find permutations of
int l: index of the leftmost character in the string (typically 0)
int r: index of the rightmost character in the string (typically sizeof(a))
vector <string>& b: vector of strings to store all possible permutations of a

Output: void

Adapted from:
https://www.geeksforgeeks.org/write-a-c-program-to-print-all-permutations-of-a-given-string/amp/
*/
void permute(string a, int l, int r, vector <string>& b)
{
    if (l == r)
    {
        b.push_back(a);
    }
    else
    {
        for (int i=1; i<= r; i++)
        {
            cout << a << endl;
            swap(a[l], a[i]); // swap
            permute (a, l+1, r, b); // recursion
            swap(a[l], a[i]); // backtrack
        }
    }
}

long int solution(int n) {
    long int c = 1;
    for (int i = 1; i <= n; i++)
        c = c * (n + i) / i;
    return c;
}

long int solution2(int h, int w)
{
    //product(numbers from h+w to h-w+1) / product(numbers from h-w to 1)
    double numerator = 1;
    double denominator = 1;
    for (int i=h+w; i>h; i--)
    {
        numerator *= i;
        //cout << i << "\tnumerator" << endl;
    }
    for (int i=h; i>0; i--)
    {
        denominator *= i;
        //cout << i << "\tdenominator" << endl;
    }
    long int output = static_cast<long int>(numerator/denominator);
    return(output);
    // float var_x = 9.99;
    // int   var_y = static_cast<int>(var_x);
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

    // unsigned long long int a = 1, b = 1, c = 1;
    // for (int i=2; i<=height+width; i++)
    // {
    //     a *= i;
    //     if (i == height)
    //         b = a;
    //     if (i == width)
    //         c = a;
    // }
    // cout << a << " " << b << " " << c << endl;
    // unsigned long long int output = a / (b * c);
    // cout << output << endl;

    // Every path through the lattice takes [height] moves down and
    // [width] moves right.
    // Generate the set of moves that must be made as a string.
    // ex: a 2x2 lattice would have this set of moves: "ddrr"
    char moves[height+width+1]; // create an array of characters to be interpreted as a string later
    for (int i=0; i<height+width; i++)
    {
        // add a 'd' for every move down
        if (i<height)
            moves[i] = 'd';
        // add a 'r' for every move right
        else
            moves[i] = 'r';
    }
    // null-terminate to form a string
    moves[height+width] = '\0';

    cout << "Here 1" << endl;

    // // Find all permutations of the 'moves' string. The 'moves' string
    // // currently has all the d's on the left and r's on the right
    // string movesString = moves; // Turn 'moves' into a string
    // vector <string> permutations; // Vector to store all possible permutations of the moves string
    // cout << movesString << endl;
    // // Swap [k] letters at a time (k from 1 up to and including [height])
    // for (int k=1; k<=height; k++)
    // {
    //     // Repeat the swapping [j] times until all d's are on the right
    //     for (int j=0; j<height; j++)
    //     {
    //         // Walk the string from left to right and swap what's needed above
    //         for (int i=0; i<height+width; i++)
    //         {
    //             // Determine if this iteration needs a swap
    //             if (movesString[i] == 'd' and movesString[i+k] == 'r')
    //             {
    //                 // Make the swap and append the new string to the vector
    //                 swap(movesString[i], movesString[i+k]);
    //                 permutations.push_back(movesString);
    //                 cout << movesString << endl;
    //             }
    //         }
    //     }
    //     // Reset to what 'moves' was originally and then repeat the k loop
    //     movesString = moves;
    // }
    //
    // // This permutation algorithm is not free of duplicates. These need to be removed.
    // // Sort permutations vector to make it easier to remove duplicates
    // sort(permutations.begin(), permutations.end());
    // // Remove duplicates.
    // permutations.erase( unique( permutations.begin(), permutations.end() ), permutations.end() );

    // // print sorted vector
    // cout << "Sorted \n";
    // for (auto x : permutations)
    //     cout << x << " ";
    // cout << endl;
    //
    // cout << permutations.capacity() << endl;

    // long int test = 2;
    // long int k = 0;
    // bool running = true;
    // //for (long int k=0; k < height+width; k++)
    // while (running)
    // {
    //     running = false;
    //     for (int i=height+width-1; i>=0; i--)
    //     {
    //         int bit = (k & ( 1 << i )) >> i;
    //         cout << bit;
    //         if (bit == 0)
    //             running = true;
    //     } cout << endl;
    //     k++;
    //     cout << k << endl;
    // }
    long int test1 = solution(20);
    cout << test1 << endl;
    long int test2 = solution2(20, 20);
    cout << test2 << endl;


    // #pragma omp parallel for
    // for(int n=0; n<size; ++n)
    //   //sinTable[n] = std::sin(2 * M_PI * n / size);
    return 0;
}
