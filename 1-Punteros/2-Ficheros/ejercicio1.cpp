#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;
struct Producto {
    int id = 0;
    string codigo;
    string nombre;
    string descripcion;
    float precio = 0.00;
    int idProveedor=0;
    int stock=0;
    string fechaRegistro;  
};


int main() {
    ifstream archivo("csv_productos.csv");
    string linea;
    if (archivo.is_open()) {
        cout << "Archivo abierto correctamente." << endl;
        Producto producto[60];
        int contador = 0;
        if(getline(archivo, linea)){
        }
    while(getline(archivo, linea) && contador < 60){
        if(linea.empty()) continue;
        stringstream ss(linea);
        string temp;
        getline(ss, temp, ',');
        producto[contador].id = stoi(temp);
        getline(ss, producto[contador].codigo, ',');
        getline(ss, producto[contador].nombre, ',');
        getline(ss, producto[contador].descripcion, ',');
        getline(ss, temp, ',');
        producto[contador].precio = stof(temp);
        getline(ss, temp, ',');
        producto[contador].idProveedor = stoi(temp);
        getline(ss, temp, ',');
        producto[contador].stock = stoi(temp);
        getline(ss, producto[contador].fechaRegistro, ',');
        contador++;
    }
    float mayor = producto[0].precio;
    int alto = producto[0].stock;
    for(int i=0; i < contador; i++){
        if(producto[i].precio > mayor){
            mayor = producto[i].precio;
        }
        if(producto[i].stock > alto){
            alto = producto[i].stock;
        }
    }
    cout << "Producto con mayor precio: " << mayor << endl; 
    cout << "Producto con mayor stock: " << alto << endl; 
    cout << "ingrese id del proveedor a buscar: ";
    int idProveedor;
    cin >> idProveedor;
    
    archivo.close();
    return 0;
}
}