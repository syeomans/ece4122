#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char **argv) {
    for (int i=0; i<10; i++)
    {
        cout << i << endl;
        if (i == 5)
        {
            i = 10;
        }
    }
}
