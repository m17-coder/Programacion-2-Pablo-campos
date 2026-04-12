#ifndef OPERACIONESPRODUCTOS_HPP
#define OPERACIONESPRODUCTOS_HPP

#include "PRODUCTOS.HPP"

// Lógica pura de negocio: recibe un producto, aplica reglas internas y persiste. No usa cout/cin.
bool registrarProducto(Producto& p);

// Lógica pura: obtiene y devuelve una lista de productos activos para que quien lo llame decida cómo mostrarlos.
int obtenerProductosActivos(Producto* listaDestino, int maxTamano);

// Lógica pura: Busca un producto puntual por su ID y lo deposita en p. Devuelve true si lo encuentra.
bool obtenerProductoPorId(int idBuscar, Producto& p);

// Lógica pura: Busca productos cuyo nombre coincida parcialmente y los agrega a un arreglo. Devuelve cuántos halló.
int buscarProductosPorNombre(const char* nombreBuscar, Producto* listaDestino, int maxTamano);

// Lógica pura: Actualiza cualquier campo básico de un producto existente. Devuelve true si logra editarlo en el archivo.
bool modificarProducto(int idBuscar, const Producto& datosActualizados);

// Lógica pura: Desactiva un producto enviándolo a "Papelera" virtualmente (borrado logico).
bool inactivarProducto(int idBuscar);

// Lógica pura: Restablece un producto que estaba en "Papelera"
bool activarProducto(int idBuscar);

// Lógica pura: Devuelve productos inactivos.
int obtenerProductosInactivos(Producto* listaDestino, int maxTamano);

#endif
