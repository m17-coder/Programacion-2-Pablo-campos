#include <iostream>
using namespace std;

int main () {
    const int tamaño = 5; // Definición del tamaño del arreglo
    int arreglo[tamaño] = {10, 20, 30, 40, 50}; // Declaración e inicialización del arreglo

    int *ptr = arreglo; // Puntero que apunta al primer elemento del arreglo

    cout << "Elementos del arreglo usando puntero:" << endl;
    for (int i = 0; i < tamaño; i++) {
        cout << *(ptr + i) << " "; // Acceso a los elementos del arreglo usando aritmética de punteros
    }
    cout << endl;

    // Modificación de los elementos del arreglo usando el puntero
    for (int i = 0; i < tamaño; i++) {
        *(ptr + i) += 5; // Incrementa cada elemento en 5
    }

    cout << "Elementos del arreglo después de la modificación:" << endl;
    for (int i = 0; i < tamaño; i++) {
        cout << *(ptr + i) << " "; // Imprime los elementos modificados
    }
    cout << endl;
    /*todas estas expresiones son equivalentes:
    numeros[0]==*numeros==*ptr==ptr[0], todas = 10
    */
    
    return 0;
}