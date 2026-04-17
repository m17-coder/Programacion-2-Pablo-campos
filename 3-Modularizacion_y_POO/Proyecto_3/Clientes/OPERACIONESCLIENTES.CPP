#include "OPERACIONESCLIENTES.HPP"
#include "../persistencia/GestorArchivos.hpp"
#include <ctime>
#include <cstring>
#include <cctype>

#define ARCHIVO_CLIENTES "datos/clientes.bin"

using namespace std;

// Función auxiliar interna
static void toLowerCaseStr(const char* str, char* out, int maxLen) {
    int i = 0;
    while (str[i] != '\0' && i < maxLen - 1) {
        out[i] = tolower(str[i]);
        i++;
    }
    out[i] = '\0';
}

// 1. REGISTRAR
bool registrarCliente(Cliente& cl) {
    cl.setTotalComprasRealizadas(0);
    cl.setMontoTotalGastado(0.0f);

    time_t t = time(0);
    cl.setFechaCreacion(t);
    cl.setFechaUltimaModificacion(t);
    cl.setActivo(true);

    struct tm * now = localtime(&t);
    char fechaReg[11];
    sprintf(fechaReg, "%04d-%02d-%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
    cl.setFechaRegistro(fechaReg);

    return GestorArchivos::guardarRegistroNuevo<Cliente>(ARCHIVO_CLIENTES, cl);
}

// 2. LISTAR ACTIVOS
int obtenerClientesActivos(Cliente arreglo[], int capacidadMax) {
    Header h = GestorArchivos::leerHeader(ARCHIVO_CLIENTES);
    int contador = 0;

    for (int i = 0; i < h.cantidadRegistros && contador < capacidadMax; i++) {
        Cliente c;
        if (GestorArchivos::leerRegistroPorIndice<Cliente>(ARCHIVO_CLIENTES, i, c)) {
            if (c.getActivo()) {
                arreglo[contador] = c;
                contador++;
            }
        }
    }
    return contador;
}

// 3. BUSCAR POR ID
bool obtenerClientePorId(int idBusqueda, Cliente& clSalida) {
    int pos = GestorArchivos::buscarPosicion<Cliente>(ARCHIVO_CLIENTES, idBusqueda);
    if (pos == -1) return false;

    return GestorArchivos::leerRegistroPorIndice<Cliente>(ARCHIVO_CLIENTES, pos, clSalida);
}

// 4. BUSCAR POR NOMBRE
int buscarClientesPorNombre(const char* nombreBuscar, Cliente arregloSalida[], int capacidadMax) {
    Header h = GestorArchivos::leerHeader(ARCHIVO_CLIENTES);
    int contador = 0;
    char target[200];
    toLowerCaseStr(nombreBuscar, target, sizeof(target));

    for (int i = 0; i < h.cantidadRegistros && contador < capacidadMax; i++) {
        Cliente c;
        if (GestorArchivos::leerRegistroPorIndice<Cliente>(ARCHIVO_CLIENTES, i, c)) {
            if (c.getActivo()) {
                char cName[200];
                toLowerCaseStr(c.getNombre(), cName, sizeof(cName));
                
                if (strstr(cName, target) != nullptr) {
                    arregloSalida[contador] = c;
                    contador++;
                }
            }
        }
    }
    return contador;
}

// EXISTE CEDULA
bool existeCedulaCliente(const char* cedula) {
    Header h = GestorArchivos::leerHeader(ARCHIVO_CLIENTES);
    for (int i = 0; i < h.cantidadRegistros; i++) {
        Cliente c;
        if (GestorArchivos::leerRegistroPorIndice<Cliente>(ARCHIVO_CLIENTES, i, c)) {
            if (c.getActivo() && strcmp(c.getCedula(), cedula) == 0) {
                return true;
            }
        }
    }
    return false;
}

// 5. ACTUALIZAR
bool modificarCliente(int idCliente, const Cliente& datosNuevos) {
    auto modificador = [&](Cliente& c) -> bool {
        if (c.getId() == idCliente && c.getActivo()) {
            c.setNombre(datosNuevos.getNombre());
            c.setCedula(datosNuevos.getCedula());
            c.setTelefono(datosNuevos.getTelefono());
            c.setEmail(datosNuevos.getEmail());
            c.setDireccion(datosNuevos.getDireccion());
            c.setFechaRegistro(datosNuevos.getFechaRegistro());
            c.actualizarUltimaModificacion();
            return true;
        }
        return false;
    };

    return GestorArchivos::procesarRegistro<Cliente>(ARCHIVO_CLIENTES, idCliente, modificador);
}

// 6. INACTIVAR
bool inactivarCliente(int idCliente) {
    auto inactivador = [&](Cliente& c) -> bool {
        if (!c.getActivo()) return false;
        c.setActivo(false);
        c.actualizarUltimaModificacion();
        return true;
    };

    return GestorArchivos::procesarRegistro<Cliente>(ARCHIVO_CLIENTES, idCliente, inactivador);
}

// 7. RESTAURAR
bool activarCliente(int idCliente) {
    auto activador = [&](Cliente& c) -> bool {
        if (c.getActivo()) return false;
        c.setActivo(true);
        c.actualizarUltimaModificacion();
        return true;
    };

    return GestorArchivos::procesarRegistro<Cliente>(ARCHIVO_CLIENTES, idCliente, activador);
}

// 8. LISTAR INACTIVOS
int obtenerClientesInactivos(Cliente arreglo[], int capacidadMax) {
    Header h = GestorArchivos::leerHeader(ARCHIVO_CLIENTES);
    int contador = 0;

    for (int i = 0; i < h.cantidadRegistros && contador < capacidadMax; i++) {
        Cliente c;
        if (GestorArchivos::leerRegistroPorIndice<Cliente>(ARCHIVO_CLIENTES, i, c)) {
            if (!c.getActivo()) {
                arreglo[contador] = c;
                contador++;
            }
        }
    }
    return contador;
}
