#include "PRODUCTOS.HPP"
#include <cstring>
#include <iostream>

using namespace std;

// Constructor por defecto
Producto::Producto() {
    id = 0;
    strcpy(codigo, "");
    strcpy(nombre, "");
    strcpy(descripcion, "");
    idProveedor = 0;
    precio = 0.0;
    existencia = 0;
    stockminimo = 0;
    totalVendido = 0;
    stockInicial = 0;
    totalComprado = 0;
    strcpy(fechaRegistro, "");
    fechaCreacion = time(0);
    fechaUltimaModificacion = time(0);
    activo = true;
}

// Getters
int Producto::getId() const { return id; }
const char* Producto::getCodigo() const { return codigo; }
const char* Producto::getNombre() const { return nombre; }
const char* Producto::getDescripcion() const { return descripcion; }
int Producto::getIdProveedor() const { return idProveedor; }
float Producto::getPrecio() const { return precio; }
int Producto::getExistencia() const { return existencia; }
int Producto::getStockminimo() const { return stockminimo; }
int Producto::getTotalVendido() const { return totalVendido; }
int Producto::getStockInicial() const { return stockInicial; }
int Producto::getTotalComprado() const { return totalComprado; }
const char* Producto::getFechaRegistro() const { return fechaRegistro; }
time_t Producto::getFechaCreacion() const { return fechaCreacion; }
time_t Producto::getFechaUltimaModificacion() const { return fechaUltimaModificacion; }
bool Producto::getActivo() const { return activo; }

// Setters
void Producto::setId(int nuevoId) { id = nuevoId; }
void Producto::setCodigo(const char* nuevoCodigo) { strcpy(codigo, nuevoCodigo); }
void Producto::setNombre(const char* nuevoNombre) { strcpy(nombre, nuevoNombre); }
void Producto::setDescripcion(const char* nuevaDescripcion) { strcpy(descripcion, nuevaDescripcion); }
void Producto::setIdProveedor(int nuevoIdProveedor) { idProveedor = nuevoIdProveedor; }
void Producto::setPrecio(float nuevoPrecio) { 
    if (nuevoPrecio >= 0) {
        precio = nuevoPrecio;
    }
}
void Producto::setExistencia(int nuevaExistencia) { existencia = nuevaExistencia; }
void Producto::setStockminimo(int nuevoStockminimo) { 
    if (nuevoStockminimo >= 0) {
        stockminimo = nuevoStockminimo;
    }
}
void Producto::setTotalVendido(int nuevoTotalVendido) { totalVendido = nuevoTotalVendido; }
void Producto::setStockInicial(int nuevoStockInicial) { stockInicial = nuevoStockInicial; }
void Producto::setTotalComprado(int nuevoTotalComprado) { totalComprado = nuevoTotalComprado; }
void Producto::setFechaRegistro(const char* nuevaFechaRegistro) { strcpy(fechaRegistro, nuevaFechaRegistro); }
void Producto::setFechaCreacion(time_t nuevaFechaCreacion) { fechaCreacion = nuevaFechaCreacion; }
void Producto::setFechaUltimaModificacion(time_t nuevaFechaUltimaModificacion) { fechaUltimaModificacion = nuevaFechaUltimaModificacion; }
void Producto::setActivo(bool nuevoActivo) { activo = nuevoActivo; }

int Producto::obtenerTamano() {
    return sizeof(Producto);
}
