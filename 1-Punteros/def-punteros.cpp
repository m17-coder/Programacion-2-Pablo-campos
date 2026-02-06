#include <iostream>

using namespace std;  

int main() {
    int var = 20;   // Declaración de una variable entera
    int *ptr;      // Declaración de un puntero a entero

    ptr = &var;    // Asignación de la dirección de 'var' al puntero 'ptr'

    cout << "Valor de var: " << var << endl;               // Imprime el valor de 'var'
    cout << "Dirección de var: " << &var << endl;          // Imprime la dirección de 'var'
    cout << "Valor del puntero ptr: " << ptr << endl;      // Imprime la dirección almacenada en 'ptr'
    cout << "Valor apuntado por ptr: " << *ptr << endl;    /* Imprime el valor al que apunta 'ptr',algo parecido
    a acceder de una cajita a su contenido */

    cout << *(&ptr) << endl; // Imprime el valor del puntero ptr accediendo a su dirección
    return 0;
}