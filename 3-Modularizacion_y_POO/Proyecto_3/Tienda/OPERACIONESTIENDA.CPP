#include "OPERACIONESTIENDA.HPP"
#include "../persistencia/GestorArchivos.hpp"
#include "../Productos/OPERACIONESPRODUCTOS.HPP"
#include "../Proveedores/OPERACIONESPROVEEDORES.HPP"
#include "../Clientes/OPERACIONESCLIENTES.HPP"
#include "../Transacciones/OPERACIONESTRANSACCIONES.HPP"
#include "../utilidades/FORMATOS.HPP"
#include <ctime>
#include <iostream>

#define ARCHIVO_TIENDA "datos/tienda.bin"

using namespace std;

static bool tiendaTieneDatosValidos(const Tienda& t) {
    return t.getNombre()[0] != '\0' && t.getRif()[0] != '\0';
}

// 1. INICIALIZAR TIENDA
bool inicializarTiendaPorDefecto() {
    Header h = GestorArchivos::leerHeader(ARCHIVO_TIENDA);
    Tienda tiendaActual;
    bool existeRegistro = GestorArchivos::leerRegistroPorIndice<Tienda>(ARCHIVO_TIENDA, 0, tiendaActual);

    // Si no existe, está vacío o quedó con un registro en blanco, lo reponemos.
    if (h.cantidadRegistros == 0 || !existeRegistro || !tiendaTieneDatosValidos(tiendaActual)) {
        Tienda t;
        t.setId(1);
        t.setNombre("TECNOSTORE");
        t.setRif("J-12345678-9");
        t.setCaja(100000.0);
        t.setIngresosTotales(0.0);
        t.setEgresosTotales(0.0);
        t.setContadorVentas(0);
        t.setContadorCompras(0);
        t.actualizarUltimaTransaccion();

        if (h.cantidadRegistros == 0 || !existeRegistro) {
            return GestorArchivos::guardarRegistroNuevo<Tienda>(ARCHIVO_TIENDA, t);
        }

        auto restaurarTienda = [&](Tienda& tienda) -> bool {
            tienda = t;
            return true;
        };

        return GestorArchivos::procesarRegistro<Tienda>(ARCHIVO_TIENDA, 1, restaurarTienda);
    }
    return true; // Ya existía
}

// 2. OBTENER ÚNICA TIENDA (ID 1)
bool obtenerDatosTienda(Tienda& t) {
    // Al ser solo una siempre será POS 0, ID 1
    return GestorArchivos::leerRegistroPorIndice<Tienda>(ARCHIVO_TIENDA, 0, t);
}

// 3. ACTUALIZAR INFO GENERAL
bool actualizarInfoTienda(const char* nuevoNombre, const char* nuevoRif) {
    auto infoActualizador = [&](Tienda& t) -> bool {
        t.setNombre(nuevoNombre);
        t.setRif(nuevoRif);
        t.actualizarUltimaTransaccion();
        return true;
    };

    return GestorArchivos::procesarRegistro<Tienda>(ARCHIVO_TIENDA, 1, infoActualizador);
}

// 4. PROCESAR VENTA (Ingreso de dinero hacia la tienda)
bool procesarRegistroVentaTienda(double monto) {
    auto ventaActualizador = [&](Tienda& t) -> bool {
        t.agregarCaja(monto);
        t.agregarIngreso(monto);
        t.incrementarVentas();
        t.actualizarUltimaTransaccion();
        return true;
    };

    return GestorArchivos::procesarRegistro<Tienda>(ARCHIVO_TIENDA, 1, ventaActualizador);
}

// 5. PROCESAR COMPRA (Pago a proveedores con egreso de dinero)
bool procesarRegistroCompraTienda(double monto) {
    auto compraActualizador = [&](Tienda& t) -> bool {
        // En un mundo real quizás validas si tiene caja, pero asumimos contabilidad en negativo
        t.restarCaja(monto);
        t.agregarEgreso(monto);
        t.incrementarCompras();
        t.actualizarUltimaTransaccion();
        return true;
    };

    return GestorArchivos::procesarRegistro<Tienda>(ARCHIVO_TIENDA, 1, compraActualizador);
}

// 6. INTEGRIDAD REFERENCIAL
void verificarIntegridadReferencial() {
    Producto productos[100];
    Proveedor proveedores[100];
    Cliente clientes[100];
    Transaccion transacciones[100];

    int cantProductos = obtenerProductosActivos(productos, 100);
    int cantProveedores = obtenerProveedoresActivos(proveedores, 100);
    int cantClientes = obtenerClientesActivos(clientes, 100);
    int cantTransacciones = obtenerTransaccionesActivas(transacciones, 100);

    cout << "\n[i] Verificando integridad referencial..." << endl;
    cout << "Productos activos: " << cantProductos << endl;
    cout << "Proveedores activos: " << cantProveedores << endl;
    cout << "Clientes activos: " << cantClientes << endl;
    cout << "Transacciones activas: " << cantTransacciones << endl;

    int incidencias = 0;
    for (int i = 0; i < cantTransacciones; i++) {
        Transaccion transaccion = transacciones[i];
        if (transaccion.getTipo() == 'V') {
            Cliente cliente;
            if (!obtenerClientePorId(transaccion.getIdRelacionado(), cliente)) {
                incidencias++;
                cout << "[!] Transaccion " << transaccion.getId() << " apunta a un cliente inexistente." << endl;
            }
        } else if (transaccion.getTipo() == 'C') {
            Proveedor proveedor;
            if (!obtenerProveedorPorId(transaccion.getIdRelacionado(), proveedor)) {
                incidencias++;
                cout << "[!] Transaccion " << transaccion.getId() << " apunta a un proveedor inexistente." << endl;
            }
        }

        const DetalleVenta* detalles = transaccion.getDetalles();
        for (int j = 0; j < transaccion.getCantidadItems(); j++) {
            Producto producto;
            if (!obtenerProductoPorId(detalles[j].getIdProducto(), producto)) {
                incidencias++;
                cout << "[!] Transaccion " << transaccion.getId() << " referencia el producto " << detalles[j].getIdProducto() << " inexistente." << endl;
            }
        }
    }

    if (incidencias == 0) {
        cout << "[OK] No se detectaron inconsistencias." << endl;
    }
}

// 7. GENERAR REPORTES ANALITICOS
void generarReportes() {
    Tienda tienda;
    Producto productosActivos[100];
    Producto productosInactivos[100];
    Proveedor proveedoresActivos[100];
    Proveedor proveedoresInactivos[100];
    Cliente clientesActivos[100];
    Cliente clientesInactivos[100];
    Transaccion transaccionesActivas[100];
    Transaccion transaccionesInactivas[100];

    obtenerDatosTienda(tienda);
    int cantProductosActivos = obtenerProductosActivos(productosActivos, 100);
    int cantProductosInactivos = obtenerProductosInactivos(productosInactivos, 100);
    int cantProveedoresActivos = obtenerProveedoresActivos(proveedoresActivos, 100);
    int cantProveedoresInactivos = obtenerProveedoresInactivos(proveedoresInactivos, 100);
    int cantClientesActivos = obtenerClientesActivos(clientesActivos, 100);
    int cantClientesInactivos = obtenerClientesInactivos(clientesInactivos, 100);
    int cantTransaccionesActivas = obtenerTransaccionesActivas(transaccionesActivas, 100);
    int cantTransaccionesInactivas = obtenerTransaccionesInactivas(transaccionesInactivas, 100);

    cout << "\n[i] REPORTES ANALITICOS" << endl;
    cout << "Tienda: " << tienda.getNombre() << endl;
    cout << "RIF: " << tienda.getRif() << endl;
    cout << "Caja: " << Formatos::formatearMoneda(tienda.getCaja()) << endl;
    cout << "Ingresos: " << Formatos::formatearMoneda(tienda.getIngresosTotales()) << endl;
    cout << "Egresos: " << Formatos::formatearMoneda(tienda.getEgresosTotales()) << endl;
    cout << "Productos activos: " << cantProductosActivos << " | inactivos: " << cantProductosInactivos << endl;
    cout << "Proveedores activos: " << cantProveedoresActivos << " | inactivos: " << cantProveedoresInactivos << endl;
    cout << "Clientes activos: " << cantClientesActivos << " | inactivos: " << cantClientesInactivos << endl;
    cout << "Transacciones activas: " << cantTransaccionesActivas << " | anuladas: " << cantTransaccionesInactivas << endl;
}

// 8. CREAR BACKUP DE LA TIENDA
void crearBackup() {
    GestorArchivos::crearBackup();
}
