#ifndef OPERACIONESTIENDA_HPP
#define OPERACIONESTIENDA_HPP

#include "TIENDA.HPP"

// Inicializa la tienda si no existe en el archivo. Retorna false si hubo error crítico.
bool inicializarTiendaPorDefecto();

// Obtiene la única instancia de la Tienda
bool obtenerDatosTienda(Tienda& t);

// Modifica la meta-información (nombre y rif)
bool actualizarInfoTienda(const char* nuevoNombre, const char* nuevoRif);

// Procesar Venta (Incrementar caja, subir ingresos y contador de ventas)
bool procesarRegistroVentaTienda(double monto);

// Procesar Compra (Devolución de caja, subir egresos y contador de compras)
bool procesarRegistroCompraTienda(double monto);

// --- Operaciones Integrales ---
void verificarIntegridadReferencial();
void generarReportes();
void crearBackup();

#endif // OPERACIONESTIENDA_HPP
