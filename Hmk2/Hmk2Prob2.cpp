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
using namespace std;


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

    // Scan the array with 4 scanners shaped like -, |, /, and \
    // Each shape is 4 elements long. The product is taken from the elements
    // that fit in the scanner. Save largest product.
    long int largestProduct = 0;
    long int thisProduct = 0;
    int xOffset1; // Each shape is defined by offsetting the starting position
    int xOffset2;
    int xOffset3;
    int xOffset4;
    int yOffset1;
    int yOffset2;
    int yOffset3;
    int yOffset4;

    // Create a 4x1 horizontal line scanner (shaped like -)
    xOffset1 = 0;
    xOffset2 = 1;
    xOffset3 = 2;
    xOffset4 = 3;
    yOffset1 = 0;
    yOffset2 = 0;
    yOffset3 = 0;
    yOffset4 = 0;

    // Scan with the 4x1 horizontal line (-) scanner and save result
    for (int i=0; i<rows; i++) // Need every row
    {
        for (int j=0; j<columns-3; j++) // Scanner is 4 wide. End 3 columns early.
        {
            // Take the product of each element that fits in the scanner
            thisProduct = data[i+yOffset1][j+xOffset1] * data[i+yOffset2][j+xOffset2] * data[i+yOffset3][j+xOffset3] * data[i+yOffset4][j+xOffset4];

            // If the product is bigger than the largest found, record it
            if (thisProduct > largestProduct)
            {
                largestProduct = thisProduct;
            }
        }
    }

    // Create a 1x4 vertical line scanner (shaped like |)
    xOffset1 = 0;
    xOffset2 = 0;
    xOffset3 = 0;
    xOffset4 = 0;
    yOffset1 = 0;
    yOffset2 = 1;
    yOffset3 = 2;
    yOffset4 = 3;

    // Scan with the 1x4 vertical line (|) scanner and save result
    for (int i=0; i<rows-3; i++) // Scanner is 4 long. End 3 rows early.
    {
        for (int j=0; j<columns; j++) // Need all the columns
        {
            // Take the product of each element that fits in the scanner
            thisProduct = data[i+yOffset1][j+xOffset1] * data[i+yOffset2][j+xOffset2] * data[i+yOffset3][j+xOffset3] * data[i+yOffset4][j+xOffset4];

            // If the product is bigger than the largest found, record it
            if (thisProduct > largestProduct)
            {
                largestProduct = thisProduct;
            }
        }
    }

    // Create a 4x4 dexter diagonal scanner (shaped like \)
    xOffset1 = 0;
    xOffset2 = 1;
    xOffset3 = 2;
    xOffset4 = 3;
    yOffset1 = 0;
    yOffset2 = 1;
    yOffset3 = 2;
    yOffset4 = 3;

    // Scan with the 4x4 dexter diagonal (\) scanner and save result
    for (int i=0; i<rows-3; i++) // Scanner is 4 long. End 3 rows early.
    {
        for (int j=0; j<columns-3; j++) // Scanner is 4 wide. End 3 columns early.
        {
            // Take the product of each element that fits in the scanner
            thisProduct = data[i+yOffset1][j+xOffset1] * data[i+yOffset2][j+xOffset2] * data[i+yOffset3][j+xOffset3] * data[i+yOffset4][j+xOffset4];

            // If the product is bigger than the largest found, record it
            if (thisProduct > largestProduct)
            {
                largestProduct = thisProduct;
            }
        }
    }

    // Create a 4x4 sinister diagonal scanner (shaped like /)
    xOffset1 = 3;
    xOffset2 = 2;
    xOffset3 = 1;
    xOffset4 = 0;
    yOffset1 = 0;
    yOffset2 = 1;
    yOffset3 = 2;
    yOffset4 = 3;

    // Scan with the 4x4 sinister diagonal (/) scanner and save result
    for (int i=0; i<rows-3; i++) // Scanner is 4 long. End 3 rows early.
    {
        for (int j=0; j<columns-3; j++) // Scanner is 4 wide. End 3 columns early.
        {
            // Take the product of each element that fits in the scanner
            thisProduct = data[i+yOffset1][j+xOffset1] * data[i+yOffset2][j+xOffset2] * data[i+yOffset3][j+xOffset3] * data[i+yOffset4][j+xOffset4];

            // If the product is bigger than the largest found, record it
            if (thisProduct > largestProduct)
            {
                largestProduct = thisProduct;
            }
        }
    }

    // Save the largest product to an output file name output2.txt
    ofstream outFile;
    outFile.open("output2.txt");
    outFile << largestProduct;

    // Nothing to return in main
    return 0;
}
