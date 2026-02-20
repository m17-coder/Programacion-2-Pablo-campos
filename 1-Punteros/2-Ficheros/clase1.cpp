#include <iostream>
#include <fstream>
using namespace std;

int main(){

   ofstream archivo("chris.txt" , ios::app); // Abrir el archivo para escritura (crea el archivo si no existe); 

    // Verificar si el archivo se pudo abrir
    if (archivo.is_open()) {
        archivo << "Hola, Programacion 2\n"; // Escribir en el archivo
        archivo << "Este es un ejemplo de manejo de archivos en C++\n";
        archivo.close(); // Cerrar el archivo
        cout << "Datos escritos en el archivo correctamente." << endl;
    } else {
        cout << "No se pudo abrir el archivo para escritura." << endl;
    }

    return 0;
}  