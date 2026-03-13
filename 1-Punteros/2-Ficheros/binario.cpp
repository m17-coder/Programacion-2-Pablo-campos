#include <iostream>
#include <fstream>
#include <cstring> // Recomendado al usar char[]

using namespace std;

// ============================================================================
// ESTRUCTURA FALTANTE
// ============================================================================
struct Registro {
    int id;
    char nombre[50];
    float saldo;
};

// ============================================================================
// PARÁMETROS DE ios:: PARA ARCHIVOS
// ============================================================================
void demostrarParametrosIOS() {
    cout << "=== PARÁMETROS DE ios:: ===" << endl;
    
    // NOTA: Agregué ios::trunc para asegurar que el archivo se cree si no existe.
    // ios::in | ios::out solos fallan si el archivo no está creado previamente.
    fstream archivo("ejemplo.dat", ios::in | ios::out | ios::binary | ios::trunc);
    
    if (!archivo) {
        cout << "Error: No se pudo abrir el archivo." << endl;
        return;
    }
    
    cout << "✓ Archivo abierto con ios::in | ios::out | ios::binary" << endl;
    archivo.close();
}

// ============================================================================
// MANIPULACIÓN DE POSICIÓN EN ARCHIVOS
// ============================================================================
void demostrarPosicionPunteros() {
    cout << "\n=== MANIPULACIÓN DE POSICIÓN ===" << endl;
    
    fstream archivo("posiciones.dat", ios::in | ios::out | ios::binary | ios::trunc);
    
    if (!archivo) {
        cout << "Error: No se pudo abrir el archivo." << endl;
        return;
    }
    
    // Escribir datos iniciales
    archivo.write("1234567890", 10);
    cout << "✓ Datos escritos: 1234567890" << endl;
    
    // Mover puntero y sobrescribir
    archivo.seekp(2, ios::beg);
    archivo.write("A", 1);
    cout << "✓ Sobrescrito en posición 2: 12A4567890" << endl;
    
    // Mostrar posición actual
    cout << "✓ Posición actual: " << archivo.tellp() << endl;
    
    archivo.close();
}

// ============================================================================
// REINTERPRET_CAST - CONVERSIÓN SIMPLE DE TIPOS
// ============================================================================
void demostrarReinterpretCast() {
    cout << "\n=== REINTERPRET_CAST ===" << endl;
    
    int numero = 42;
    cout << "Número original: " << numero << endl;
    
    ofstream archivo("numero.dat", ios::binary);
    if (!archivo) {
        cout << "Error: No se pudo abrir el archivo." << endl;
        return;
    }
    
    archivo.write(reinterpret_cast<const char*>(&numero), sizeof(int));
    cout << "✓ Número escrito como bytes en archivo" << endl;
    archivo.close();
    
    ifstream archivoLectura("numero.dat", ios::binary);
    if (!archivoLectura) {
        cout << "Error: No se pudo leer el archivo." << endl;
        return;
    }
    
    int numeroLeido;
    archivoLectura.read(reinterpret_cast<char*>(&numeroLeido), sizeof(int));
    cout << "✓ Número leído: " << numeroLeido << endl;
    archivoLectura.close();
}

// ============================================================================
// EJEMPLO PRÁCTICO: SISTEMA DE REGISTROS
// ============================================================================
void ejemploPractico() {
    cout << "\n=== EJEMPLO PRÁCTICO ===" << endl;
    
    Registro registros[] = {
        {1, "Ana Garcia", 2500.50},
        {2, "Carlos Lopez", 1800.25},
        {3, "Maria Rodriguez", 3200.75}
    };
    
    ofstream archivo("registros.dat", ios::binary);
    if (!archivo) {
        cout << "Error: No se pudo crear el archivo." << endl;
        return;
    }
    
    for (int i = 0; i < 3; i++) {
        archivo.write(reinterpret_cast<const char*>(&registros[i]), sizeof(Registro));
    }
    archivo.close();
    
    cout << "✓ 3 registros escritos en archivo binario" << endl;
    
    ifstream archivoLectura("registros.dat", ios::binary);
    if (!archivoLectura) {
        cout << "Error: No se pudo abrir para lectura." << endl;
        return;
    }
    
    Registro registroLeido;
    // ¡ESTA ES LA MAGIA DEL ACCESO DIRECTO!
    archivoLectura.seekg(1 * sizeof(Registro), ios::beg); 
    archivoLectura.read(reinterpret_cast<char*>(&registroLeido), sizeof(Registro));
    
    cout << "✓ Registro leído: ID=" << registroLeido.id 
         << ", Nombre=" << registroLeido.nombre 
         << ", Saldo=" << registroLeido.saldo << endl;
    
    archivoLectura.close();
}

int main() {
    cout << "=== CONCEPTOS FUNDAMENTALES DE ARCHIVOS EN C++ ===" << endl;
    
    demostrarParametrosIOS();
    demostrarPosicionPunteros();
    demostrarReinterpretCast();
    ejemploPractico();
    
    return 0;
}