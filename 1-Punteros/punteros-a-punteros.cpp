#include <iostream>
using namespace std;

int main() {
    int numero = 42;
    int *ptr = &numero; // puntero a numero
    int **ptrPtr = &ptr; // puntero a puntero a numero
    int ***ptrPtrPtr = &ptrPtr; // puntero a puntero a puntero a numero
    cout << "Valor de numero: " << numero << endl;
    cout << "Valor al que apunta ptr: " << *ptr << endl;
    cout << "Valor al que apunta ptrPtr: " << **ptrPtr << endl;
    cout << "Valor al que apunta ptrPtrPtr: " << ***ptrPtrPtr << endl;
    return 0;
}