#include "test.h"
#include <iostream>
#include <fstream>
#include <limits>
#include <iomanip>
using namespace std;

// NxN constructor
ECE_Matrix::ECE_Matrix(const int N, const double defaultValue): rows(N), columns(N)
{
    data = new double*[rows];
    for(int i = 0; i < rows; ++i)
    {
        data[i] = new double[columns];
        for(int j=0; j<columns; j++)
        {
            data[i][j] = defaultValue;
        }
    }
};

// NxM constructor
ECE_Matrix::ECE_Matrix(int N, int M, double defaultValue): rows(N), columns(M)
{
    data = new double*[rows];
    for(int i = 0; i < rows; ++i)
    {
        data[i] = new double[columns];
        for(int j=0; j<columns; j++)
        {
            data[i][j] = defaultValue;
        }
    }
};


// Text file constructor
ECE_Matrix::ECE_Matrix(string fileName)
{
    // Open input file from input string
    ifstream inFile;
    inFile.open(fileName);

    // Make sure the file was opened
    if (!inFile) {
        cerr << "Unable to open file datafile.txt";
        exit(1);   // call system to stop
    }

    // The first two values are number of rows and number of columns
    inFile >> rows >> columns;

    // Create new 2D array
    data = new double*[rows];
    for(int i = 0; i < rows; ++i)
    {
        data[i] = new double[columns];
    }

    // Get each value and store it in the array
    double thisValue;
    int counter = 0; // Count the number of values we've retrieved
    while (inFile >> thisValue) {
        // each row is floor(counter / M), each column is counter % M
        data[counter/columns][counter%columns] = thisValue;
        counter ++;
    }
};

ostream& operator<<(ostream& os, const ECE_Matrix& matrix)
{
    for (int i=0; i<matrix.rows; i++)
    {
        for (int j=0; j<matrix.columns; j++)
        {
            os << scientific << setw(12) << setprecision(3) << matrix.data[i][j];
        }
        os << endl;
    }
    return os;
};

ECE_Matrix ECE_Matrix::operator-(double rhs)
{
    ECE_Matrix newMatrix(rows, columns, 0.0);
    for (int i=0; i<rows; i++)
    {
        for (int j=0; j<columns; j++)
        {
            newMatrix.data[i][j] = data[i][j] - rhs;
        }
    }
    return newMatrix;
};

ECE_Matrix ECE_Matrix::operator-(ECE_Matrix rhs)
{
    // Compare the two matrices and record the largest
    int largestRows = max(rows, rhs.rows);
    int largestColumns = max(columns, rhs.columns);

    // Make a new matrix from the largest row/column sizes
    ECE_Matrix newMatrix(largestRows,largestColumns, 0.0);

    // perform matrix subtraction, treating anything out-of-bounds as 0
    for (int i=0; i<largestRows; i++)
    {
        for (int j=0; j<largestColumns; j++)
        {
            // if both the left and right hand sides are out of range, insert 0
            if (((rows <= i) || (columns <= j))&&((rhs.rows <= i) || (rhs.columns <= j)))
                newMatrix.data[i][j] = 0;
            // if left hand side is out of range, treat lhs as 0
            else if ((rows <= i) || (columns <= j))
                newMatrix.data[i][j] = 0 - rhs.data[i][j];
            // if right hand side is out of range, treat rhs as 0
            else if ((rhs.rows <= i) || (rhs.columns <= j))
                newMatrix.data[i][j] = data[i][j];
            // else, subtract as normal
            else
                newMatrix.data[i][j] = data[i][j] - rhs.data[i][j];
        }
    }
    return newMatrix;
};

ECE_Matrix ECE_Matrix::operator+(double rhs)
{
    ECE_Matrix newMatrix(rows, columns, 0.0);
    for (int i=0; i<rows; i++)
    {
        for (int j=0; j<columns; j++)
        {
            newMatrix.data[i][j] = data[i][j] + rhs;
        }
    }
    return newMatrix;
};

ECE_Matrix ECE_Matrix::operator+(ECE_Matrix rhs)
{
    // Compare the two matrices and record the largest
    int largestRows = max(rows, rhs.rows);
    int largestColumns = max(columns, rhs.columns);

    // Make a new matrix from the largest row/column sizes
    ECE_Matrix newMatrix(largestRows,largestColumns, 0.0);

    // perform matrix subtraction, treating anything out-of-bounds as 0
    for (int i=0; i<largestRows; i++)
    {
        for (int j=0; j<largestColumns; j++)
        {
            // if both the left and right hand sides are out of range, insert 0
            if (((rows <= i) || (columns <= j))&&((rhs.rows <= i) || (rhs.columns <= j)))
                newMatrix.data[i][j] = 0;
            // if left hand side is out of range, treat lhs as 0
            else if ((rows <= i) || (columns <= j))
                newMatrix.data[i][j] = rhs.data[i][j];
            // if right hand side is out of range, treat rhs as 0
            else if ((rhs.rows <= i) || (rhs.columns <= j))
                newMatrix.data[i][j] = data[i][j];
            // else, subtract as normal
            else
                newMatrix.data[i][j] = data[i][j] + rhs.data[i][j];
        }
    }
    return newMatrix;
};

ECE_Matrix ECE_Matrix::operator*(double rhs)
{
    // Create a new matrix
    ECE_Matrix newMatrix(rows,columns, 0.0);
    // Scale every element in the original matrix by the right hand side
    for (int i=0; i<rows; i++)
    {
        for (int j=0; j<columns; j++)
        {
            newMatrix.data[i][j] = data[i][j] * rhs;
        }
    }
    return newMatrix;
};

ECE_Matrix ECE_Matrix::operator/(double rhs)
{
    // Create a new matrix
    ECE_Matrix newMatrix(rows, columns, 0.0);
    // Divide every element in the original matrix by the right hand side
    for (int i=0; i<rows; i++)
    {
        for (int j=0; j<columns; j++)
        {
            newMatrix.data[i][j] = data[i][j] / rhs;
        }
    }
    return newMatrix;
};

ECE_Matrix operator*(double const &lhs, ECE_Matrix const &rhs)
{
    // Create a new matrix
    ECE_Matrix newMatrix(rhs.rows, rhs.columns, 0.0);
    // Scale every element in the original matrix by the right hand side
    for (int i=0; i<rhs.rows; i++)
    {
        for (int j=0; j<rhs.columns; j++)
        {
            newMatrix.data[i][j] = rhs.data[i][j] * lhs;
        }
    }
    return newMatrix;
}

ECE_Matrix ECE_Matrix::transpose()
{
    ECE_Matrix newMatrix(columns, rows, 0.0);
    for (int i=0; i<rows; i++)
    {
        for (int j=0; j<columns; j++)
        {
            newMatrix.data[j][i] = data[i][j];
        }
    }
    return newMatrix;
}

ECE_Matrix ECE_Matrix::operator+=(ECE_Matrix rhs)
{
    // Compare the two matrices and record the largest
    int largestRows = max(rows, rhs.rows);
    int largestColumns = max(columns, rhs.columns);

    // Make a new matrix from the largest row/column sizes
    ECE_Matrix newMatrix(largestRows,largestColumns, 0.0);
    //newMatrix = this + rhs;

    // perform matrix addition, treating anything out-of-bounds as 0
    for (int i=0; i<largestRows; i++)
    {
        for (int j=0; j<largestColumns; j++)
        {
            // if both the left and right hand sides are out of range, insert 0
            if (((rows <= i) || (columns <= j))&&((rhs.rows <= i) || (rhs.columns <= j)))
                newMatrix.data[i][j] = 0;
            // if left hand side is out of range, treat lhs as 0
            else if ((rows <= i) || (columns <= j))
                newMatrix.data[i][j] = rhs.data[i][j];
            // if right hand side is out of range, treat rhs as 0
            else if ((rhs.rows <= i) || (rhs.columns <= j))
                newMatrix.data[i][j] = data[i][j];
            // else, subtract as normal
            else
                newMatrix.data[i][j] = data[i][j] + rhs.data[i][j];
        }
    }
    return newMatrix;
}

// Copy constructor
ECE_Matrix::ECE_Matrix(const ECE_Matrix &oldObject)//: rows(oldObject.rows), columns(oldObject.columns)
{
    rows = oldObject.rows;
    columns -= oldObject.columns;
    data = new double*[rows];
    for(int i = 0; i < rows; ++i)
    {
        data[i] = new double[columns];
        for(int j=0; j<columns; j++)
        {
            data[i][j] = oldObject.data[i][j];
        }
    }
};

int main()
{
    return 0;
}
