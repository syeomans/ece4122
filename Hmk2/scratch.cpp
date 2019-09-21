#include <iostream>
#include <fstream>
#include <limits>
using namespace std;

class ECE_Matrix
{
	int rows;
    int columns;
	double **data;

public:
	ECE_Matrix(int N, int M, double defaultValue); // Constructor
};

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
}

int main(){
    const ECE_Matrix M1(3, 3, 5.5);
    // cout << M1.rows << endl;
}
