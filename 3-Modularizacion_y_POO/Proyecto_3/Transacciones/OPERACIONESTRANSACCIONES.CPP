#include "OPERACIONESTRANSACCIONES.HPP"
#include "../persistencia/GestorArchivos.hpp"
#include <ctime>

#define ARCHIVO_TRANSACCIONES "datos/transacciones.bin"

using namespace std;

// 1. REGISTRAR
bool registrarTransaccion(Transaccion& t) {
    time_t tStr = time(nullptr);
    t.setFechaCreacion(tStr);
    t.setFechaUltimaModificacion(tStr);
    t.setActivo(true);
    
    // Si no tiene fecha seteada se la ponemos a mano
    if (t.getFecha()[0] == '\0') {
        struct tm * now = localtime(&tStr);
        char fechaReg[11];
        sprintf(fechaReg, "%04d-%02d-%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
        t.setFecha(fechaReg);
    }
    
    // Re-calculamos total por seguridad antes de grabar
    t.calcularTotal();

    return GestorArchivos::guardarRegistroNuevo<Transaccion>(ARCHIVO_TRANSACCIONES, t);
}

// 2. LISTAR ACTIVAS
int obtenerTransaccionesActivas(Transaccion arreglo[], int capacidadMax) {
    Header h = GestorArchivos::leerHeader(ARCHIVO_TRANSACCIONES);
    int contador = 0;

    for (int i = 0; i < h.cantidadRegistros && contador < capacidadMax; i++) {
        Transaccion t;
        if (GestorArchivos::leerRegistroPorIndice<Transaccion>(ARCHIVO_TRANSACCIONES, i, t)) {
            if (t.getActivo()) {
                arreglo[contador] = t;
                contador++;
            }
        }
    }
    return contador;
}

// 3. OBTENER POR ID
bool obtenerTransaccionPorId(int idBusqueda, Transaccion& transSalida) {
    int pos = GestorArchivos::buscarPosicion<Transaccion>(ARCHIVO_TRANSACCIONES, idBusqueda);
    if (pos == -1) return false;

    return GestorArchivos::leerRegistroPorIndice<Transaccion>(ARCHIVO_TRANSACCIONES, pos, transSalida);
}

// 4. OBTENER POR TIPO (Compras 'C' o Ventas 'V')
int obtenerTransaccionesPorTipo(char tipoBusqueda, Transaccion arregloSalida[], int capacidadMax) {
    Header h = GestorArchivos::leerHeader(ARCHIVO_TRANSACCIONES);
    int contador = 0;

    for (int i = 0; i < h.cantidadRegistros && contador < capacidadMax; i++) {
        Transaccion t;
        if (GestorArchivos::leerRegistroPorIndice<Transaccion>(ARCHIVO_TRANSACCIONES, i, t)) {
            if (t.getActivo() && t.getTipo() == tipoBusqueda) {
                arregloSalida[contador] = t;
                contador++;
            }
        }
    }
    return contador;
}

// 5. OBTENER POR RELACIONADO (Proveedor ID / Cliente ID)
int obtenerTransaccionesPorRelacionado(int idRel, Transaccion arregloSalida[], int capacidadMax) {
    Header h = GestorArchivos::leerHeader(ARCHIVO_TRANSACCIONES);
    int contador = 0;

    for (int i = 0; i < h.cantidadRegistros && contador < capacidadMax; i++) {
        Transaccion t;
        if (GestorArchivos::leerRegistroPorIndice<Transaccion>(ARCHIVO_TRANSACCIONES, i, t)) {
            if (t.getActivo() && t.getIdRelacionado() == idRel) {
                arregloSalida[contador] = t;
                contador++;
            }
        }
    }
    return contador;
}

// 6. ANULAR / INACTIVAR TRANSACCIÓN 
bool anularTransaccion(int idTransaccion) {
    auto anulador = [&](Transaccion& t) -> bool {
        if (!t.getActivo()) return false;
        t.setActivo(false);
        t.actualizarUltimaModificacion();
        return true;
    };

    return GestorArchivos::procesarRegistro<Transaccion>(ARCHIVO_TRANSACCIONES, idTransaccion, anulador);
}

// 7. RESTAURAR / ACTIVAR TRANSACCIÓN
bool activarTransaccion(int idTransaccion) {
    auto activador = [&](Transaccion& t) -> bool {
        if (t.getActivo()) return false;
        t.setActivo(true);
        t.actualizarUltimaModificacion();
        return true;
    };

    return GestorArchivos::procesarRegistro<Transaccion>(ARCHIVO_TRANSACCIONES, idTransaccion, activador);
}

// 8. LISTAR INACTIVAS (Anuladas)
int obtenerTransaccionesInactivas(Transaccion arreglo[], int capacidadMax) {
    Header h = GestorArchivos::leerHeader(ARCHIVO_TRANSACCIONES);
    int contador = 0;

    for (int i = 0; i < h.cantidadRegistros && contador < capacidadMax; i++) {
        Transaccion t;
        if (GestorArchivos::leerRegistroPorIndice<Transaccion>(ARCHIVO_TRANSACCIONES, i, t)) {
            if (!t.getActivo()) {
                arreglo[contador] = t;
                contador++;
            }
        }
    }
    return contador;
}
