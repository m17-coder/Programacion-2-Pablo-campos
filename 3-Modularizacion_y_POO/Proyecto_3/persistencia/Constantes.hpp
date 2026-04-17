#ifndef CONSTANTES_HPP
#define CONSTANTES_HPP

const char* const ARCHIVO_TIENDA = "datos/tienda.bin";
const char* const ARCHIVO_PRODUCTOS = "datos/productos.bin";
const char* const ARCHIVO_PROVEEDORES = "datos/proveedores.bin";
const char* const ARCHIVO_CLIENTES = "datos/clientes.bin";
const char* const ARCHIVO_TRANSACCIONES = "datos/transacciones.bin";

struct Header {
    int cantidadRegistros;
    int ProximoId;
    int registrosActivos;
    int version;
};

#endif
