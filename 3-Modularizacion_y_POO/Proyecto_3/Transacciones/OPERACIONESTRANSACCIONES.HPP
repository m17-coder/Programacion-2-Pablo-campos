#ifndef OPERACIONESTRANSACCIONES_HPP
#define OPERACIONESTRANSACCIONES_HPP

#include "TRANSACCIONES.HPP"

// Registrar nueva transacción
bool registrarTransaccion(Transaccion& t);

// Listar todas activas
int obtenerTransaccionesActivas(Transaccion arreglo[], int capacidadMax);

// Listar inactivos (anulados)
int obtenerTransaccionesInactivas(Transaccion arreglo[], int capacidadMax);

// Buscar por id
bool obtenerTransaccionPorId(int idBusqueda, Transaccion& transSalida);

// Filtrar por tipo: 'C' compras / 'V' ventas
int obtenerTransaccionesPorTipo(char tipoBusqueda, Transaccion arregloSalida[], int capacidadMax);

// Filtrar por ID de Entidad (Proveedor/Cliente)
int obtenerTransaccionesPorRelacionado(int idRel, Transaccion arregloSalida[], int capacidadMax);

// Anular o Inactivar (borrado lógico) de transacción (debería requerir revertir stocks/saldos idealmente)
bool anularTransaccion(int idTransaccion);

// Restaurar / Activar una transacción anulada
bool activarTransaccion(int idTransaccion);

#endif // OPERACIONESTRANSACCIONES_HPP
