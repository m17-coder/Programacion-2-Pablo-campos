#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include "Productos/PRODUCTOS.HPP"
#include "Proveedores/PROVEEDORES.HPP"
#include "Clientes/CLIENTES.HPP"
#include "Transacciones/TRANSACCIONES.HPP"
#include "Tienda/TIENDA.HPP"
#include "persistencia/GestorArchivos.hpp"

using namespace std;

// ======= ESTRUCTURAS FASE 2 EXACTAS ========
struct HeaderF2 {
    int cantidadRegistros;
    int ProximoId;
    int registrosActivos;
    int version;
};

struct ProductoF2 {
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

struct ProveedorF2 {
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

struct ClienteF2 {
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

struct DetalleVentaF2 {
    int idProducto;
    int cantidad;
    float precioUnitario;
};

struct TransaccionF2 {
    int id;
    int idRelacionado;
    float total;
    char fecha[11];
    char descripcion[200];
    DetalleVentaF2 detalles[30];
    int cantidaditems;
    char tipo;
    bool activo;
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

template<typename TF2, typename TF3>
void migrarGenerico(const string& rutaF2, const string& rutaF3, TF3 (*mapear)(const TF2&), int version = 1) {
    ifstream in(rutaF2, ios::binary);
    if(!in) {
        cout << "No se pudo leer " << rutaF2 << endl;
        return;
    }

    HeaderF2 h2;
    if(!in.read(reinterpret_cast<char*>(&h2), sizeof(HeaderF2))) return;

    Header h3;
    h3.cantidadRegistros = h2.cantidadRegistros;
    h3.ProximoId = h2.ProximoId;
    h3.registrosActivos = h2.registrosActivos;
    h3.version = version;

    ofstream out(rutaF3, ios::binary);
    out.write(reinterpret_cast<char*>(&h3), sizeof(Header));

    int leidos = 0;
    while(leidos < h2.cantidadRegistros) {
        TF2 item2;
        if(in.read(reinterpret_cast<char*>(&item2), sizeof(TF2))) {
            TF3 item3 = mapear(item2);
            out.write(reinterpret_cast<char*>(&item3), sizeof(TF3));
            leidos++;
        } else {
            break;
        }
    }
    cout << "Migrados " << leidos << " items en " << rutaF3 << endl;
}

// ======= MAPEOS ========
Producto mapearProducto(const ProductoF2& p2) {
    Producto p3;
    p3.setId(p2.id);
    p3.setCodigo(p2.codigo);
    p3.setNombre(p2.nombre);
    p3.setDescripcion(p2.descripcion);
    p3.setIdProveedor(p2.idProveedor);
    p3.setPrecio(p2.precio);
    p3.setExistencia(p2.existencia);
    p3.setStockminimo(p2.stockminimo);
    p3.setTotalVendido(p2.totalVendido);
    p3.setStockInicial(p2.stockInicial);
    p3.setTotalComprado(p2.totalComprado);
    p3.setFechaRegistro(p2.fechaRegistro);
    p3.setFechaCreacion(p2.fechaCreacion);
    p3.setFechaUltimaModificacion(p2.fechaUltimaModificacion);
    p3.setActivo(p2.activo);
    return p3;
}

Cliente mapearCliente(const ClienteF2& c2) {
    Cliente c3;
    c3.setId(c2.id);
    c3.setNombre(c2.nombre);
    c3.setCedula(c2.cedula);
    c3.setTelefono(c2.telefono);
    c3.setEmail(c2.email);
    c3.setDireccion(c2.direccion);
    c3.setTotalComprasRealizadas(c2.totalComprasRealizadas);
    c3.setMontoTotalGastado(c2.montoTotalGastado);
    c3.setActivo(c2.activo);
    c3.setFechaRegistro(c2.fechaRegistro);
    c3.setFechaCreacion(c2.fechaCreacion);
    c3.setFechaUltimaModificacion(c2.fechaUltimaModificacion);
    return c3;
}

Proveedor mapearProveedor(const ProveedorF2& p2) {
    Proveedor p3;
    p3.setId(p2.id);
    p3.setNombre(p2.nombre);
    p3.setRif(p2.rif);
    p3.setTelefono(p2.telefono);
    p3.setEmail(p2.email);
    p3.setDireccion(p2.direccion);
    p3.setTotalSuministros(p2.totalSuministros);
    p3.setTotalPagado(p2.totalPagado);
    p3.setActivo(p2.activo);
    p3.setFechaRegistro(p2.fechaRegistro);
    p3.setFechaCreacion(p2.fechaCreacion);
    p3.setFechaUltimaModificacion(p2.fechaUltimaModificacion);
    return p3;
}

Transaccion mapearTransaccion(const TransaccionF2& t2) {
    Transaccion t3;
    t3.setId(t2.id);
    t3.setIdRelacionado(t2.idRelacionado);
    t3.setTotal(t2.total);
    t3.setFecha(t2.fecha);
    t3.setDescripcion(t2.descripcion);
    
    t3.setTipo(t2.tipo);
    t3.setActivo(t2.activo);
    t3.setFechaCreacion(t2.fechaCreacion);
    t3.setFechaUltimaModificacion(t2.fechaUltimaModificacion);

    for(int i = 0; i < t2.cantidaditems; i++) {
        DetalleVenta dv;
        dv.setIdProducto(t2.detalles[i].idProducto);
        dv.setCantidad(t2.detalles[i].cantidad);
        dv.setPrecioUnitario(t2.detalles[i].precioUnitario);
        t3.agregarDetalle(dv); // Esto actualizará la cantidaditems internamente
    }

    return t3;
}

int main() {
    string origen = "../../2-Ficheros.bin/Proyecto2/";
    string destino = "datos/";

    // Usar #pragma pack por si es necesario que coincidan exáctamente, pero sino, usar las definiciones genéricas está bien
    // Si la lectura directa falla por el pragma pack, se puede intentar sin él
    migrarGenerico<ClienteF2, Cliente>(origen + "clientes.bin", destino + "clientes.bin", mapearCliente);
    migrarGenerico<ProductoF2, Producto>(origen + "productos.bin", destino + "productos.bin", mapearProducto);
    migrarGenerico<ProveedorF2, Proveedor>(origen + "proveedores.bin", destino + "proveedores.bin", mapearProveedor);
    migrarGenerico<TransaccionF2, Transaccion>(origen + "transacciones.bin", destino + "transacciones.bin", mapearTransaccion);

    cout << "Finalizado!" << endl;
    return 0;
}
