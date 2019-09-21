#pragma once
#include <iostream>
#include <fstream>
#include <limits>
#include <iomanip>
using namespace std;

class ECE_Matrix {
    double **data;
    int rows;
    int columns;
public:
    ECE_Matrix(int N, double defaultValue);
    ECE_Matrix(int N, int M, double defaultValue);
    ECE_Matrix(string fileName);
    friend ostream& operator<<(ostream& os, const ECE_Matrix& matrix);
    ECE_Matrix operator-(double rhs);
    ECE_Matrix operator-(ECE_Matrix rhs);
    ECE_Matrix operator+(double rhs);
    ECE_Matrix operator+(ECE_Matrix rhs);
    ECE_Matrix operator*(double rhs);
    ECE_Matrix operator/(double rhs);
    friend ECE_Matrix operator*(double const &lhs, ECE_Matrix const &rhs);
    ECE_Matrix transpose();
    ECE_Matrix operator+=(ECE_Matrix rhs);
    ECE_Matrix():rows{0}, columns{0}, data{NULL}{};
    ECE_Matrix(const ECE_Matrix &oldObject);
};
