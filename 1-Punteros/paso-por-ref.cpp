#include <iostream>
using namespace std;

void incrementar (int x) {
    x = x + 1; // Incrementa el valor de x (copia local)
}

void incrementarPorReferencia (int *ptr) {
    *ptr = *ptr + 1; // Incrementa el valor al que apunta ptr
}
void incrementarPorReferenciaRef (int &ref) {
    ref = ref + 1; // Incrementa el valor referenciado por ref
}
//ejemplos practicos
void interambiar(int &a, int &b) {
    int temp = a;
    a = b;
    b = temp;
}
void calcularCuadradoycubo(int numero, int &cuadrado, int &cubo) {
    cuadrado = numero * numero;
    cubo = numero * numero * numero;
} 

void crearnumero(int **ptr) {
    *ptr = new int; // Asigna memoria para un entero
    **ptr = 42;     // Asigna un valor al entero
}

int main() {
    int x = 5; // Declaración de una variable entera
    cout << "Antes de incrementar por valor: " << x << endl;
    incrementar(x); // Llamada a la función que incrementa por valor
    cout << "Después de incrementar por valor: " << x << endl; // x no cambia
    incrementarPorReferencia(&x); // Llamada a la función que incrementa por referencia
    cout << "Después de incrementar por referencia: " << x << endl; // x cambia
    incrementarPorReferenciaRef(x); // Llamada a la función que incrementa por referencia con referencias
    cout << "Después de incrementar por referencia con referencias: " << x << endl; // x cambia

    // Ejemplo práctico: Intercambiar valores
    int a = 10, b = 20;
    cout << "Antes de intercambiar: a = " << a << ", b = "
            << b << endl;
    interambiar(a, b);
    cout << "Después de intercambiar: a = " << a << ", b = " << b << endl;
    // Ejemplo práctico: Calcular cuadrado y cubo
    int numero = 3, cuadrado, cubo;
    calcularCuadradoycubo(numero, cuadrado, cubo);
    cout << "Número: " << numero << ", Cuadrado: " << cuadrado
            << ", Cubo: " << cubo << endl;
    // Ejemplo práctico: Crear un número dinámicamente
    int *numPtr = nullptr;// Puntero a entero inicializado a nullptr,para evitar errores
    crearnumero(&numPtr);
    cout << "Número creado dinámicamente: " << *numPtr << endl;
    delete numPtr; // Liberar la memoria asignada
    return 0;
}