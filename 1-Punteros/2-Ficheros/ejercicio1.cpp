#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring> 
#include <clocale>

using namespace std;
 // Capacidad máxima del arreglo

struct Producto {
    int id;
    char codigo[10];
    char nombre[100];
    char descripcion[100];
    float precio;
    int idProveedor;
    int stock;
    char FechaIngreso[11];
    bool activo;
};

struct header{
    int cantidadregistros;
    int proximoid;
    int regitrosactivos;
    int version;
};



int main() {
    setlocale(LC_ALL, "spanish"); // Establecer el locale para manejar caracteres especiales
    // 1. Crear el arreglo y el contador
    Producto lista[60];
    int contador = 0;
    int mayorPrecioIndex = 0; // Para almacenar el índice del producto con mayor precio
    int mayorStockIndex = 0; // Para almacenar el índice del producto con mayor stock
    int mayorPrecio = 0; // Para almacenar el mayor precio encontrado
    int mayorStock = 0; // Para almacenar el mayor stock encontrado
    header h1;
    ifstream archivo("csv_productos.csv");
    string linea;
    if (!archivo.is_open()) {
        cout << "Error al abrir el archivo." << endl;
        return 1;
    }
    string cabecera;
    getline(archivo, cabecera); // Lee y descarta la primera fila
    // 2. Leer línea por línea
    while (getline(archivo, linea) && contador < 60) {
        stringstream ss(linea);
        string temporal;

        getline(ss, temporal, ','); // Leer ID
        lista[contador].id = stoi(temporal);

        // Leer CÓDIGO
        getline(ss, temporal, ',');
        strncpy(lista[contador].codigo, temporal.c_str(), 9);
        lista[contador].codigo[9] = '\0'; // Asegurar cierre de cadena

        // Leer NOMBRE
        getline(ss, temporal, ',');
        strncpy(lista[contador].nombre, temporal.c_str(), 49);
        lista[contador].nombre[49] = '\0';

        getline(ss, temporal, ','); // Leer DESCRIPCIÓN
        strncpy(lista[contador].descripcion, temporal.c_str(), 99);
        lista[contador].descripcion[99] = '\0';
        
        // Leer ID PROVEEDOR
        getline(ss, temporal, ','); 
        lista[contador].idProveedor = stoi(temporal);

        // Leer PRECIO
        getline(ss, temporal, ',');
        lista[contador].precio = stof(temporal);
        // Leer STOCK
        getline(ss, temporal, ',');
        lista[contador].stock = stoi(temporal);

        // Leer FECHA DE INGRESO
        getline(ss, temporal, ',');
        strncpy(lista[contador].FechaIngreso, temporal.c_str(), 10);
        lista[contador].FechaIngreso[10] = '\0';
        // Incrementar el índice para el siguiente producto
        contador++;
        bool activo = true; // Asumimos que todos los productos leídos están activos
        lista[contador-1].activo = activo; // Asignar el valor de activo
        h1.cantidadregistros = contador;
        h1.proximoid = contador + 1; // El próximo ID sería el contador
        h1.regitrosactivos = contador; // Asumimos que todos los registros leídos están activos
        h1.version = 1; // Versión del formato de archivo
    }
    for (int i = 0; i < contador; i++) {
        // Verificar si el precio es el mayor encontrado
        if (lista[i].precio > mayorPrecio) {
            mayorPrecio = lista[i].precio;
            mayorPrecioIndex = i;
        }

        // Verificar si el stock es el mayor encontrado
        if (lista[i].stock > mayorStock) {
            mayorStock = lista[i].stock;
            mayorStockIndex = i;
        }
    }

    cout << "Producto con mayor precio: " << lista[mayorPrecioIndex].nombre << " (Precio: " << lista[mayorPrecioIndex].precio << ")" << endl;
    cout << "Producto con mayor stock: " << lista[mayorStockIndex].nombre << " (Stock: " << lista[mayorStockIndex].stock << ")" << endl;
    
    ofstream archivoBin("productos.dat", ios::binary);
    if (!archivoBin) {
        cout << "Error al crear el archivo binario." << endl;
        return 1;
    }
    // Escribir el header al inicio del archivo
    archivoBin.write(reinterpret_cast<const char*>(&h1), sizeof(header));
    // Escribir los productos en el archivo binario
    for (int i = 0; i < contador; i++) {
        archivoBin.write(reinterpret_cast<const char*>(&lista[i]), sizeof(Producto));
    }
    ifstream archivoBinLectura("productos.dat", ios::binary);
    if (!archivoBinLectura) {
        cout << "Error al abrir el archivo binario para lectura." << endl;
        return 1;
    }
    
    // Leer el header del archivo binario
    archivoBinLectura.read(reinterpret_cast<char*>(&h1), sizeof(header));
    cout << "Cantidad de registros: " << h1.cantidadregistros << endl;
    cout << "Próximo ID: " << h1.proximoid << endl;
    cout << "Registros activos: " << h1.regitrosactivos << endl;
    cout << "Versión: " << h1.version << endl;
        // Leer los productos del archivo binario
    for (int i = 0; i < h1.cantidadregistros; i++) {
        Producto p1;
        archivoBinLectura.read(reinterpret_cast<char*>(&p1), sizeof(Producto));
        cout << "ID: " << p1.id<<endl; 
        cout<< "CÓDIGO: " << p1.codigo << endl;
        cout << "NOMBRE: " << p1.nombre << endl;
        cout << "DESCRIPCIÓN: " << p1.descripcion << endl;
        cout << "ID PROVEEDOR: " << p1.idProveedor << endl;
        cout << "PRECIO: " << p1.precio << endl;
        cout << "STOCK: " << p1.stock << endl;
        cout << "FECHA DE INGRESO: " << p1.FechaIngreso << endl;
        cout << "-----------------------------" << endl;
    }
    if (archivoBinLectura.is_open()) {
        archivoBinLectura.close();
    }
    return 0;
}