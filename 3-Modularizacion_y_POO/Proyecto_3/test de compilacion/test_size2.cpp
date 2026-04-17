#include <iostream>
#include "Productos/PRODUCTOS.HPP"
#include "Proveedores/PROVEEDORES.HPP"
#include "Clientes/CLIENTES.HPP"
#include "Transacciones/TRANSACCIONES.HPP"
#include "Tienda/TIENDA.HPP"
#include "persistencia/GestorArchivos.hpp"

struct ProductoFase2 {
    int id;
    char codigo[20];
    char nombre[100];
    char descripcion[200];
    int idProveedor;
    float precio;
    int existencia;
    int stockminimo;
    int totalVendido;
    int stockInicial;
    int totalComprado;
    char fechaRegistro[11];
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
    bool activo;
};

struct ProveedorFase2 {
    int id;
    char nombre[100];
    char rif[20];
    char telefono[20];
    char email[100];
    char direccion[200];
    char fechaRegistro[11];
    int totalSuministros;
    float totalPagado;
    bool activo;
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

struct ClienteFase2 {
    int id;
    char nombre[100];
    char cedula[20];
    char telefono[20];
    char email[100];
    char direccion[200];
    int totalComprasRealizadas;
    float montoTotalGastado;
    bool activo;
    char fechaRegistro[11];
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

struct DetalleVentaFase2 {
    int idProducto;
    int cantidad;
    float precioUnitario;
};
struct TransaccionFase2 {
    int id;
    int idRelacionado;
    float total;
    char fecha[11];
    char descripcion[200];
    DetalleVentaFase2 detalles[30];
    int cantidaditems;
    char tipo;
    bool activo;
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

using namespace std;

int main() {
    cout << "sizeof(Header): " << sizeof(Header) << endl;
    cout << "Producto F2: " << sizeof(ProductoFase2) << " | F3: " << sizeof(Producto) << endl;
    cout << "Proveedor F2: " << sizeof(ProveedorFase2) << " | F3: " << sizeof(Proveedor) << endl;
    cout << "Cliente F2: " << sizeof(ClienteFase2) << " | F3: " << sizeof(Cliente) << endl;
    cout << "Transaccion F2: " << sizeof(TransaccionFase2) << " | F3: " << sizeof(Transaccion) << endl;
    return 0;
}