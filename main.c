#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main()
{
    printf("%d", pixelEnergy());
    return 0;
}

int pixelEnergy()
{
    int Rx = 127 - 92;
    int Gx = 114 - 92;
    int Bx = 75 - 92;

    int Ry = 242 - 104;
    int Gy = 201 - 104;
    int By = 184 - 104;

    int DLTx = pow(Rx, 2) + pow(Gx, 2) + pow(Bx, 2);
    int DLTy = pow(Ry, 2) + pow(Gy, 2) + pow(By, 2);

    return(DLTx + DLTy);
}
