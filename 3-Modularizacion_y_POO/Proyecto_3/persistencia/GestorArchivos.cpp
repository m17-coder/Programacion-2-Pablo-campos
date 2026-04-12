#include "GestorArchivos.hpp"
#include <iostream>
#include <fstream>

using namespace std;

void GestorArchivos::crearArchivoBinario(const char* nombreArchivo) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo) { // Si no existe, lo creamos
        ofstream nuevoArchivo(nombreArchivo, ios::binary);
        Header h = {0, 1, 0, 1}; // Inicialización por defecto del header
        nuevoArchivo.write(reinterpret_cast<const char*>(&h), sizeof(Header));
        nuevoArchivo.close();
    } else {
        archivo.close();
    }
}

Header GestorArchivos::leerHeader(const char* nombreArchivo) {
    Header h = {0, 1, 0, 1};
    ifstream archivo(nombreArchivo, ios::binary);
    if (archivo) {
        archivo.read(reinterpret_cast<char*>(&h), sizeof(Header));
        archivo.close();
    }
    return h;
}

bool GestorArchivos::inicializarSistemaArchivos() {
    // Creamos la carpeta de datos si no existe (creación remota en terminal, el C++ asume que existe la carpeta)
    // Inicializamos todos los archivos fundamentales:
    crearArchivoBinario("datos/productos.bin");
    // (Luego podemos inicializar tienda.bin, proveedores.bin, etc.)
    return true;
}
