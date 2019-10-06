/*
Author: Samuel Yeomans
Class: ECE 4122
Date: October 6, 2019

Description:
Console program that takes as a command line argument the name of a data file.
The data file should contain a M x N grid of numbers. This program reads
in the data file and determines the largest product of four adjacent numbers
in the same direction (up, down, left, right, or diagonally).
*/

#include <iostream>
#include <fstream>
#include <limits>
#include <iomanip>
#include <omp.h>
using namespace std;

// Initialize global variables
double largestProduct = 0.0;

// Entry point into the application
int main(int argc, char* argv[])
{
    // Get input file name from user
    string fileName = argv[1];

    // Open input file from input string
    ifstream inFile;
    inFile.open(fileName);

    // Make sure the file was opened
    if (!inFile) {
        cerr << "Unable to open file datafile.txt";
        exit(1);   // call system to stop
    }

    // The first two values are number of rows and number of columns
    int rows;
    int columns;
    inFile >> rows >> columns;

    // Create new 2D array to hold the values from the input file
    double** data = new double*[rows];
    for(int i = 0; i < rows; ++i)
    {
        data[i] = new double[columns];
    }

    // Get each value from the input file and store it in the array
    double thisValue;
    int counter = 0; // Count the number of values we've retrieved
    while (inFile >> thisValue) {
        // each row is floor(counter / columns), each column is counter % columns
        data[counter/columns][counter%columns] = thisValue;
        counter ++;
    }

    // Initialize thread variables
    int xOffset [4] = {0, 0, 0, 0};
    int yOffset [4] = {0, 0, 0, 0};
    double thisProduct = 0.0;
    double threadLargestProduct = 0.0;
    int endRow = rows;
    int endColumn = columns;
    int threadId;

    /*
    Create 4 threads and scan with different shaped scanners (-, |, \, and /).
    Scan the data array with a 4-element scanner. The shape of the scanner is
    defined by an xOffset array and a yOffset array. Take the product of the
    4 scanned elements and store the largest value found globally.
    */
    #pragma omp parallel num_threads(5) shared(largestProduct) private(threadId, xOffset, yOffset, thisProduct, threadLargestProduct, endRow, endColumn)
    {
        // Set thread ID
        threadId = omp_get_thread_num();

        // Change the shape of the scanner by thread ID
        switch(threadId)
        {
            case 1: // thread 1 scans with a horizontal scanner (shaped like -)
            {
                for (int i=0; i<4; i++)
                {
                    xOffset[i] = i; // xOffset = {0, 1, 2, 3}
                                    // yOffset = {0, 0, 0, 0} (default)
                }
                endRow = rows - 3; // scanner is 4-wide
                break;
            }
            case 2: // thread 2 scans with a vertical scanner (shaped like |)
            {
                for (int i=0; i<4; i++)
                {
                    yOffset[i] = i; // yOffset = {0, 1, 2, 3}
                                    // xOffset = {0, 0, 0, 0} (default)

                }
                endColumn = columns - 3; // scanner is 4-long
                break;
            }
            case 3: // thread 3 scans with a dexter diagonal scanner (shaped like \)
            {
                for (int i=0; i<4; i++)
                {
                    xOffset[i] = i; // xOffset = {0, 1, 2, 3}
                    yOffset[i] = i; // yOffset = {0, 1, 2, 3}
                }
                endRow = rows - 3; // scanner is 4-wide
                endColumn = columns - 3; // scanner is 4-long
                break;
            }
            case 4: // thread 4 scans with a sinister diagonal scanner (shaped like /)
            {
                for (int i=0; i<4; i++)
                {
                    xOffset[i] = -1*i+3; // xOffset = {3, 2, 1, 0}
                    yOffset[i] = i; // yOffset = {0, 1, 2, 3}
                }
                endRow = rows - 3; // scanner is 4-wide
                endColumn = columns - 3; // scanner is 4-long
                break;
            }
        }

        // Scan the array with this thread's scanner and find the product of the
        // elements inside the scanned area
        if (threadId != 0) // Main thread does not scan
        {
            for (int i=0; i<endRow; i++)
            {
                for (int j=0; j<endColumn; j++)
                {
                    // Take the product of each element that fits in the scanner
                    thisProduct = data[i+yOffset[0]][j+xOffset[0]] * data[i+yOffset[1]][j+xOffset[1]] * data[i+yOffset[2]][j+xOffset[2]] * data[i+yOffset[3]][j+xOffset[3]];

                    // If the product is bigger than the largest found, save it locally
                    if (thisProduct > threadLargestProduct)
                    {
                        threadLargestProduct = thisProduct;
                    }
                }
            }

            // If this thread's largest product is bigger than the global largest product, save it globally
            if (threadLargestProduct > largestProduct)
            {
                largestProduct = threadLargestProduct;
            }
        }
    }

    // Wait until all threads are finished
    #pragma omp barrier

    // Save the largest product to an output file name output2.txt
    ofstream outFile;
    outFile.open("output2.txt");
    outFile << largestProduct;

    // Nothing to return in main
    return 0;
}
