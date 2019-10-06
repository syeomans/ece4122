/*
Author: Samuel Yeomans
Class: ECE 4122
Date: September 20, 2019

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
#include <thread>
#include <mutex>
using namespace std;

// Create a mutex to make global variable largestProduct thread safe
mutex productMutex;
double largestProduct = 0.0;

/*
Scan an array with a 4-element scanner. The shape of the scanner is defined by
an xOffset array and a yOffset array. Take the product of the 4 elements and
return the largest value found.
*/
void scan(double** array, int rows, int columns, int xOffset [4], int yOffset [4])
{
    // Initialize variables
    double thisProduct = 0.0;
    double threadLargestProduct = 0.0;

    // Scan with the 4x1 horizontal line (-) scanner and save result
    for (int i=0; i<rows; i++) // Need every row
    {
        for (int j=0; j<columns; j++) // Scanner is 4 wide. End 3 columns early.
        {
            // Take the product of each element that fits in the scanner
            thisProduct = array[i+yOffset[0]][j+xOffset[0]] * array[i+yOffset[1]][j+xOffset[1]] * array[i+yOffset[2]][j+xOffset[2]] * array[i+yOffset[3]][j+xOffset[3]];

            // If the product is bigger than the largest found, record it
            if (thisProduct > threadLargestProduct)
            {
                threadLargestProduct = thisProduct;
            }
        }
    }
    productMutex.lock();
    if (threadLargestProduct > largestProduct)
    {
        largestProduct = threadLargestProduct;
    }
    productMutex.unlock();
}


// Entry point into the application
int main()
{
    // Get input file name
    string fileName;
    cout << "Please enter the name of your input file: ";
    cin >> fileName;

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

    // Create new 2D array
    double** data = new double*[rows];
    for(int i = 0; i < rows; ++i)
    {
        data[i] = new double[columns];
    }

    // Get each value from the input file and store it in the array
    double thisValue;
    int counter = 0; // Count the number of values we've retrieved
    while (inFile >> thisValue) {
        // each row is floor(counter / M), each column is counter % M
        data[counter/columns][counter%columns] = thisValue;
        counter ++;
    }

    // Create a 4x1 horizontal line scanner (shaped like -)
    int xOffsetHorizontal [4] = {0, 1, 2, 3};
    int yOffsetHorizontal [4] = {0, 0, 0, 0};
    // Scanner is 4 wide. End 3 columns early.

    thread horizontal(scan, data, rows, columns-3, xOffsetHorizontal, yOffsetHorizontal);

    // Create a 1x4 vertical line scanner (shaped like |)
    int xOffsetVertical [4] = {0, 0, 0, 0};
    int yOffsetVertical [4] = {0, 1, 2, 3};
    // Scanner is 4 long. End 3 rows early.
    thread vertical(scan, data, rows-3, columns, xOffsetVertical, yOffsetVertical);

    // Create a 4x4 dexter diagonal scanner (shaped like \)
    int xOffsetDexter [4] = {0, 1, 2, 3};
    int yOffsetDexter [4] = {0, 1, 2, 3};
    // Scanner is 4 wide and 4 long. End 3 columns and 3 rows early.
    thread dexter(scan, data, rows-3, columns-3, xOffsetDexter, yOffsetDexter);

    // Create a 4x4 sinister diagonal scanner (shaped like /)
    int xOffsetSinister [4] = {3, 2, 1, 0};
    int yOffsetSinister [4] = {0, 1, 2, 3};
    // Scanner is 4 wide and 4 long. End 3 columns and 3 rows early.
    thread sinister(scan, data, rows-3, columns-3, xOffsetSinister, yOffsetSinister);

    // Wait until all threads are finished
    horizontal.join();
    vertical.join();
    dexter.join();
    sinister.join();

    // Save the largest product to an output file name output2.txt
    ofstream outFile;
    outFile.open("output2.txt");
    outFile << largestProduct;

    // Nothing to return in main
    return 0;
}
