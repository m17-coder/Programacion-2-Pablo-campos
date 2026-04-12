#include "OPERACIONESPRODUCTOS.HPP"
#include "../persistencia/GestorArchivos.hpp"
#include <ctime>
#include <cstring>
#include <cctype>

using namespace std;

// 1. REGISTRAR
bool registrarProducto(Producto& p) {
    p.setExistencia(p.getStockInicial()); 
    p.setTotalVendido(0);
    p.setTotalComprado(0);
    
    time_t t = time(0);
    p.setFechaCreacion(t);
    p.setFechaUltimaModificacion(t);
    p.setActivo(true);
    
    struct tm * now = localtime(&t);
    char fechaReg[11];
    sprintf(fechaReg, "%04d-%02d-%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
    p.setFechaRegistro(fechaReg);

    return GestorArchivos::guardarRegistroNuevo("datos/productos.bin", p);
}

// 2. LISTAR ACTIVOS
int obtenerProductosActivos(Producto* listaDestino, int maxTamano) {
    Header h = GestorArchivos::leerHeader("datos/productos.bin");
    int contador = 0;

    for (int i = 0; i < h.cantidadRegistros && contador < maxTamano; i++) {
        Producto p;
        if (GestorArchivos::leerRegistroPorIndice("datos/productos.bin", i, p)) {
            if (p.getActivo()) {
                listaDestino[contador] = p;
                contador++;
            }
        }
    }
    return contador;
}

// 3. BUSCAR POR ID Y RETORNAR OBJETO
bool obtenerProductoPorId(int idBuscar, Producto& pDestino) {
    int pos = GestorArchivos::buscarPosicion<Producto>("datos/productos.bin", idBuscar);
    if (pos == -1) return false;
    
    return GestorArchivos::leerRegistroPorIndice("datos/productos.bin", pos, pDestino);
}

// (FUNCIÓN DE APOYO PRIVADA PARA CONVERTIR CADENAS A MINUSCULAS TEMPORALMENTE Y BUSCAR)
bool contieneTextoIgnorandoMayusculas(const char* baseTexto, const char* buscado) {
    char t1[200], t2[200];
    strcpy(t1, baseTexto);
    strcpy(t2, buscado);
    
    for (int i = 0; t1[i]; i++) t1[i] = tolower(t1[i]);
    for (int i = 0; t2[i]; i++) t2[i] = tolower(t2[i]);
    
    return strstr(t1, t2) != nullptr;
}

// 4. BUSCAR POR NOMBRE (SUBSTRING)
int buscarProductosPorNombre(const char* nombreBuscar, Producto* listaDestino, int maxTamano) {
    Header h = GestorArchivos::leerHeader("datos/productos.bin");
    int contador = 0;

    for (int i = 0; i < h.cantidadRegistros && contador < maxTamano; i++) {
        Producto p;
        if (GestorArchivos::leerRegistroPorIndice("datos/productos.bin", i, p)) {
            // Verifica que este activo y el texto buscado esté contenido en el nombre principal
            if (p.getActivo() && contieneTextoIgnorandoMayusculas(p.getNombre(), nombreBuscar)) {
                listaDestino[contador] = p;
                contador++;
            }
        }
    }
    return contador;
}

// 5. ACTUALIZAR (Usando tu Lambda mágica)
bool modificarProducto(int idBuscar, const Producto& datosActualizados) {
    return GestorArchivos::procesarRegistro<Producto>("datos/productos.bin", idBuscar, [&datosActualizados](Producto& pExistente) -> bool {
        // Intercambiamos solo los campos permitidos y lógicos
        pExistente.setNombre(datosActualizados.getNombre());
        pExistente.setDescripcion(datosActualizados.getDescripcion());
        pExistente.setCodigo(datosActualizados.getCodigo());
        pExistente.setPrecio(datosActualizados.getPrecio());
        pExistente.setStockminimo(datosActualizados.getStockminimo());
        
        pExistente.setFechaUltimaModificacion(time(0));
        
        // Si editamos exitosamente, regresamos true para que se sobreescriba tu lambda!
        return true;
    });
}

// 6. ELIMINAR O INACTIVAR
bool inactivarProducto(int idBuscar) {
    return GestorArchivos::procesarRegistro<Producto>("datos/productos.bin", idBuscar, [](Producto& pExistente) -> bool {
        if (!pExistente.getActivo()) return false; // ya estaba borrado logicamente
        
        pExistente.setActivo(false);
        pExistente.setFechaUltimaModificacion(time(0));
        return true;
    });
}

// 7. RESTAURAR (Mismo principio)
bool activarProducto(int idBuscar) {
    return GestorArchivos::procesarRegistro<Producto>("datos/productos.bin", idBuscar, [](Producto& pExistente) -> bool {
        if (pExistente.getActivo()) return false; 
        
        pExistente.setActivo(true);
        pExistente.setFechaUltimaModificacion(time(0));
        return true;
    });
}

// 8. LISTAR INACTIVOS (Para ver la papelera visual)
int obtenerProductosInactivos(Producto* listaDestino, int maxTamano) {
    Header h = GestorArchivos::leerHeader("datos/productos.bin");
    int contador = 0;

    for (int i = 0; i < h.cantidadRegistros && contador < maxTamano; i++) {
        Producto p;
        if (GestorArchivos::leerRegistroPorIndice("datos/productos.bin", i, p)) {
            if (!p.getActivo()) {
                listaDestino[contador] = p;
                contador++;
            }
        }
    }
    return contador;
}

