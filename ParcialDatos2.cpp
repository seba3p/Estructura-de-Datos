#include <iostream>
using namespace std;

void intercambiar(double& x, double& y)
{
    double aux = x;
    x = y;
    y = aux;
}

void ordIntercambio(double matriz[3][5], int filas, int columnas)
{
    int i, j;

    for (i = 0; i < filas * columnas - 1; i++)
    {
        for (j = i + 1; j < filas * columnas; j++)
        {
            int fila1 = i / columnas;
            int col1 = i % columnas;

            int fila2 = j / columnas;
            int col2 = j % columnas;

            if (matriz[fila1][col1] > matriz[fila2][col2])
            {
                intercambiar(matriz[fila1][col1],matriz[fila2][col2]);
            }
        }
    }
}

int main()
{
    double arregloTabla[3][5] = 
    {
        {13, 10.4, 7, 45, 4},
        {17, 3, 2, 6, 7},
        {3.5, 8, 9, 9, 78}
    };

    int filas = 3;
    int columnas = 5;
    int tam = filas * columnas;
    double sumaTotal = 0;

    ordIntercambio(arregloTabla, filas, columnas);

    cout << "El menor de todos los elementos es: "
        << arregloTabla[0][0] << endl;

    cout << "El mayor de todos los elementos es: "
        << arregloTabla[filas - 1][columnas - 1] << endl;

    for (int i = 0; i < filas; i++)
    {
        for (int j = 0; j < columnas; j++)
        {
            sumaTotal += arregloTabla[i][j];
        }
    }
    cout << "La suma de todos los elementos es: "<< sumaTotal << endl;
    cout << "El promedio de todos los elementos es: "<< sumaTotal / tam << endl;
    cout << "Matriz ordenada:" << endl;

    for (int i = 0; i < filas; i++)
    {
        for (int j = 0; j < columnas; j++)
        {
            cout << arregloTabla[i][j] << "\t";
        }
        cout << endl;
    }
    return 0;
}
