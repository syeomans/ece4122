/*
Author: Samuel Yeomans
Class: ECE 4122
Date: September 20, 2019

Description:
Header file for ECE_Matrix.
ECE_Matrix holds a dynamic, two-dimensional array with M rows and N columns of
element type double. Supports overloaded operations for +, -, *, /, +=, and -=.
*/

#pragma once
#include <iostream>
#include <fstream>
#include <limits>
#include <iomanip>
using namespace std;

/*
ECE_Matrix class holds a dynamic, two-dimensional array with
M rows and N columns of element type double
*/
class ECE_Matrix {
    double **data;
    int rows;
    int columns;
public:
    ECE_Matrix(int N, double defaultValue);
    ECE_Matrix(int N, int M, double defaultValue);
    ECE_Matrix(string fileName);
    friend ostream& operator<<(ostream& os, const ECE_Matrix& matrix);
    ECE_Matrix operator-(double const &rhs) const;
    ECE_Matrix operator-(ECE_Matrix const &rhs) const;
    ECE_Matrix operator+(double const &rhs) const;
    ECE_Matrix operator+(ECE_Matrix const &rhs) const;
    ECE_Matrix operator*(double const &rhs) const;
    ECE_Matrix operator/(double const &rhs) const;
    friend ECE_Matrix operator*(double const &lhs, ECE_Matrix const &rhs);
    ECE_Matrix transpose() const;
    ECE_Matrix operator+=(ECE_Matrix const &rhs) const;
    ECE_Matrix():rows{0}, columns{0}, data{NULL}{};
    ECE_Matrix(ECE_Matrix const &oldObject);
    ECE_Matrix operator-=(ECE_Matrix const &rhs) const;
    friend ECE_Matrix operator+(double const &lhs, ECE_Matrix const &rhs);
    friend ECE_Matrix operator-(double const &lhs, ECE_Matrix const &rhs);
};
