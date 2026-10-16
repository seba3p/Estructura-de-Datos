
#include <iostream>
using namespace std;

int busquedaBinaria(int numeros[], int tam, int buscado) {
    int izquierda = 0;
    int derecha = tam - 1;

    while (izquierda <= derecha) {
        int medio = (izquierda + derecha) / 2;

        if (numeros[medio] == buscado) {
            return medio;
        }
        else if (numeros[medio] < buscado) {
            izquierda = medio + 1;
        }
        else {
            derecha = medio - 1;
        }
    }

    return -1;
}

int main() {
    int numeros[9] = {2, 5, 8, 12, 16, 23, 38, 45, 57};
    int buscado;

    cout << "Ingrese el numero que desea buscar: ";
    cin >> buscado;

    int posicion = busquedaBinaria(numeros, 9, buscado);

    if (posicion != -1) {
        cout << "Numero encontrado en la posicion: "
             << posicion << endl;
    }
    else {
        cout << "El numero no existe en el arreglo." << endl;
    }

    return 0;
}
