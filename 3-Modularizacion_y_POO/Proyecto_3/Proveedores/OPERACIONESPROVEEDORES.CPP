#include "OPERACIONESPROVEEDORES.HPP"
#include "../persistencia/GestorArchivos.hpp"
#include "../persistencia/Constantes.hpp"
#include <cstring>
#include <cctype>

#define ARCHIVO_PROVEEDORES "datos/proveedores.bin"

// Usa el GestorArchivos del sistema
using namespace std;

// --- Funciones internas (helpers) ---
// Retorna cadena a minúsculas temporalmente para búsquedas case-insensitive
static void toLowerCaseStr(const char* str, char* out, int maxLen) {
    int i = 0;
    while (str[i] != '\0' && i < maxLen - 1) {
        out[i] = tolower(str[i]);
        i++;
    }
    out[i] = '\0';
}

// -------------------- LÓGICA CRUD ----------------------- //

bool registrarProveedor(Proveedor& prov) {
    if (GestorArchivos::guardarRegistroNuevo<Proveedor>(ARCHIVO_PROVEEDORES, prov)) {
        return true;
    }
    return false;
}

int obtenerProveedoresActivos(Proveedor arreglo[], int capacidadMax) {
    Header h = GestorArchivos::leerHeader(ARCHIVO_PROVEEDORES);
    int contador = 0;

    for (int i = 0; i < h.cantidadRegistros && contador < capacidadMax; i++) {
        Proveedor p;
        if (GestorArchivos::leerRegistroPorIndice<Proveedor>(ARCHIVO_PROVEEDORES, i, p)) {
            if (p.getActivo()) {
                arreglo[contador] = p;
                contador++;
            }
        }
    }
    return contador;
}

int obtenerProveedoresInactivos(Proveedor arreglo[], int capacidadMax) {
    Header h = GestorArchivos::leerHeader(ARCHIVO_PROVEEDORES);
    int contador = 0;

    for (int i = 0; i < h.cantidadRegistros && contador < capacidadMax; i++) {
        Proveedor p;
        if (GestorArchivos::leerRegistroPorIndice<Proveedor>(ARCHIVO_PROVEEDORES, i, p)) {
            if (!p.getActivo()) {
                arreglo[contador] = p;
                contador++;
            }
        }
    }
    return contador;
}

bool obtenerProveedorPorId(int idBusqueda, Proveedor& provSalida) {
    bool encontrado = false;

    // Calculamos el índice asumiendo ID correlativos que empiezan en 1
    int indice = idBusqueda - 1;
    
    if (indice >= 0) {
        encontrado = GestorArchivos::leerRegistroPorIndice<Proveedor>(ARCHIVO_PROVEEDORES, indice, provSalida);
    }
    
    // Verificamos si realmente el ID coincide por si caso
    if (encontrado && provSalida.getId() == idBusqueda) {
        return true;
    }

    return false;
}

int buscarProveedoresPorNombre(const char* nombreBuscar, Proveedor arregloSalida[], int capacidadMax) {
    Header h = GestorArchivos::leerHeader(ARCHIVO_PROVEEDORES);
    int contador = 0;
    char target[200];
    toLowerCaseStr(nombreBuscar, target, sizeof(target));

    for (int i = 0; i < h.cantidadRegistros && contador < capacidadMax; i++) {
        Proveedor p;
        if (GestorArchivos::leerRegistroPorIndice<Proveedor>(ARCHIVO_PROVEEDORES, i, p)) {
            if (p.getActivo()) {
                char pName[200];
                toLowerCaseStr(p.getNombre(), pName, sizeof(pName));
                
                if (strstr(pName, target) != nullptr) {
                    arregloSalida[contador] = p;
                    contador++;
                }
            }
        }
    }
    return contador;
}

bool existeRifProveedor(const char* rif) {
    Header h = GestorArchivos::leerHeader(ARCHIVO_PROVEEDORES);
    for (int i = 0; i < h.cantidadRegistros; i++) {
        Proveedor p;
        if (GestorArchivos::leerRegistroPorIndice<Proveedor>(ARCHIVO_PROVEEDORES, i, p)) {
            if (p.getActivo() && strcmp(p.getRif(), rif) == 0) {
                return true;
            }
        }
    }
    return false;
}

bool modificarProveedor(int idProveedor, const Proveedor& datosNuevos) {
    auto modificador = [&](Proveedor& p) -> bool {
        if (p.getId() == idProveedor && p.getActivo()) {
            p.setNombre(datosNuevos.getNombre());
            p.setRif(datosNuevos.getRif());
            p.setTelefono(datosNuevos.getTelefono());
            p.setEmail(datosNuevos.getEmail());
            p.setDireccion(datosNuevos.getDireccion());
            p.setFechaRegistro(datosNuevos.getFechaRegistro());
            p.actualizarUltimaModificacion();
            return true;
        }
        return false;
    };

    return GestorArchivos::procesarRegistro<Proveedor>(ARCHIVO_PROVEEDORES, idProveedor, modificador);
}

bool inactivarProveedor(int idProveedor) {
    auto inactivador = [&](Proveedor& p) -> bool {
        p.setActivo(false);
        p.actualizarUltimaModificacion();
        return true;
    };

    return GestorArchivos::procesarRegistro<Proveedor>(ARCHIVO_PROVEEDORES, idProveedor, inactivador);
}

bool activarProveedor(int idProveedor) {
    auto activador = [&](Proveedor& p) -> bool {
        p.setActivo(true);
        p.actualizarUltimaModificacion();
        return true;
    };

    return GestorArchivos::procesarRegistro<Proveedor>(ARCHIVO_PROVEEDORES, idProveedor, activador);
}
