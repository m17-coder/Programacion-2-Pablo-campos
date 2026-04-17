#ifndef OPERACIONESPROVEEDORES_HPP
#define OPERACIONESPROVEEDORES_HPP

#include "PROVEEDORES.HPP"

// Retorna el ID asignado o 0 si falló
bool registrarProveedor(Proveedor& prov);

// Retorna la cantidad de proveedores activos leídos
int obtenerProveedoresActivos(Proveedor arreglo[], int capacidadMax);

// Retorna la cantidad de proveedores inactivos
int obtenerProveedoresInactivos(Proveedor arreglo[], int capacidadMax);

// Retorna true si encontró el proveedor, llenando la referencia
bool obtenerProveedorPorId(int idBusqueda, Proveedor& provSalida);

// Busca proveedores que coincidan con un nombre
int buscarProveedoresPorNombre(const char* nombreBuscar, Proveedor arregloSalida[], int capacidadMax);

// Retorna true si la modificación fue exitosa
bool modificarProveedor(int idProveedor, const Proveedor& datosNuevos);

// Inactiva un proveedor (borrado lógico)
bool inactivarProveedor(int idProveedor);

// Activa un proveedor previamente inactivado
bool activarProveedor(int idProveedor);

// Verifica si un RIF ya existe
bool existeRifProveedor(const char* rif);

#endif // OPERACIONESPROVEEDORES_HPP
