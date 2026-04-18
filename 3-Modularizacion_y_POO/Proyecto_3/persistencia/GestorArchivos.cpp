#include "GestorArchivos.hpp"
#include "Constantes.hpp"
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstring>
#include <cstdlib>

using namespace std;

void GestorArchivos::crearBackup() {
    
    system("mkdir datos\\backups 2> nul"); 
    
    time_t t = time(0);
    struct tm * now = localtime(&t);
    char prefijo[100];
    sprintf(prefijo, "datos/backups/backup_%04d%02d%02d_%02d%02d_", 
            now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min);

    const char* archivos[] = {
        ARCHIVO_TIENDA, ARCHIVO_PRODUCTOS, ARCHIVO_PROVEEDORES, 
        ARCHIVO_CLIENTES, ARCHIVO_TRANSACCIONES
    };
    int cont = 0;

    cout << "\n>>> INICIANDO RESPALDO DE SEGURIDAD <<<" << endl;
    for (const char* ruta : archivos) {
        // Extraer solo el nombre (ej. "datos/productos.bin" -> "productos.bin")
        const char* nombreDesto = ruta;
        if (strncmp(ruta, "datos/", 6) == 0) nombreDesto = ruta + 6;

        char destino[200];
        sprintf(destino, "%s%s", prefijo, nombreDesto);

        ifstream src(ruta, ios::binary);
        if (src) {
            ofstream dst(destino, ios::binary);
            dst << src.rdbuf();
            cout << "[OK] Respaldado: " << nombreDesto << endl;
            cont++;
        }
    }
    cout << "---------------------------------------" << endl;
    cout << "[i] Backup finalizado con exito. " << cont << " archivos asegurados en datos/backups/" << endl;
}

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
    crearArchivoBinario("datos/proveedores.bin");
    crearArchivoBinario("datos/clientes.bin");
    crearArchivoBinario("datos/transacciones.bin");
    crearArchivoBinario("datos/tienda.bin");
    return true;
}
