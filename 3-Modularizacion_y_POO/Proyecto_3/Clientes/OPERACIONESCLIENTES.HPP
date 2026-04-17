#ifndef OPERACIONESCLIENTES_HPP
#define OPERACIONESCLIENTES_HPP

#include "CLIENTES.HPP"

// Retorna true si se guardó con éxito
bool registrarCliente(Cliente& cl);

// Retorna la cantidad de clientes activos
int obtenerClientesActivos(Cliente arreglo[], int capacidadMax);

// Retorna la cantidad de clientes inactivos
int obtenerClientesInactivos(Cliente arreglo[], int capacidadMax);

// Retorna true si encontró el cliente
bool obtenerClientePorId(int idBusqueda, Cliente& clSalida);

// Busca clientes por aproximación de nombre
int buscarClientesPorNombre(const char* nombreBuscar, Cliente arregloSalida[], int capacidadMax);

// Verifica si existe cedula/RIF
bool existeCedulaCliente(const char* cedula);

// Modifica datos básicos de un cliente
bool modificarCliente(int idCliente, const Cliente& datosNuevos);

// Borrado lógico
bool inactivarCliente(int idCliente);

// Restauración
bool activarCliente(int idCliente);

#endif // OPERACIONESCLIENTES_HPP
