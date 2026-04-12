#ifndef GESTORARCHIVOS_HPP
#define GESTORARCHIVOS_HPP

#include <iostream>
#include <fstream>
#include "Constantes.hpp"

using namespace std;

class GestorArchivos {
public:
    static bool inicializarSistemaArchivos();
    static void crearArchivoBinario(const char* nombreArchivo);
    static Header leerHeader(const char* nombreArchivo);

    // Guarda un nuevo registro y le asigna el próximo ID disponible
    template <typename T>
    static bool guardarRegistroNuevo(const char* archivo, T& registro) {
        Header h = leerHeader(archivo);
        
        // Le asignamos el ID disponible según el header
        registro.setId(h.ProximoId);
        
        fstream dataFile(archivo, ios::in | ios::out | ios::binary);
        if (!dataFile) return false;
        
        // Escribimos al final de los registros
        dataFile.seekp(sizeof(Header) + (h.cantidadRegistros * sizeof(T)), ios::beg);
        dataFile.write(reinterpret_cast<const char*>(&registro), sizeof(T));
        
        // Actualizamos header
        h.cantidadRegistros++;
        h.ProximoId++;
        h.registrosActivos++;
        
        dataFile.seekp(0, ios::beg);
        dataFile.write(reinterpret_cast<const char*>(&h), sizeof(Header));
        dataFile.close();
        
        return true;
    }

    // Lee un registro de una posición específica (índice base 0)
    template <typename T>
    static bool leerRegistroPorIndice(const char* archivo, int indice, T& registro) {
        Header h = leerHeader(archivo);
        if (indice < 0 || indice >= h.cantidadRegistros) return false;

        ifstream dataFile(archivo, ios::binary);
        if (!dataFile) return false;
        
        dataFile.seekg(sizeof(Header) + (indice * sizeof(T)), ios::beg);
        dataFile.read(reinterpret_cast<char*>(&registro), sizeof(T));
        dataFile.close();
        
        return true;
    }

    // Busca la posición de un registro por su ID
    template <typename T>
    static int buscarPosicion(const char* nombreArchivo, int idBuscar) {
        Header h = leerHeader(nombreArchivo);
        ifstream archivo(nombreArchivo, ios::binary);
        if (!archivo) return -1;
        
        T temp;
        for (int i = 0; i < h.cantidadRegistros; i++) {
            archivo.seekg(sizeof(Header) + (i * sizeof(T)), ios::beg);
            archivo.read(reinterpret_cast<char*>(&temp), sizeof(T));
            if (temp.getId() == idBuscar) {
                archivo.close();
                return i;
            }
        }
        archivo.close();
        return -1;
    }

    // Actualiza un registro usando una función Lambda
    template <typename T, typename Func>
    static bool procesarRegistro(const char* nombreArchivo, int idBuscar, Func accion) {
        int pos = buscarPosicion<T>(nombreArchivo, idBuscar);
        if (pos == -1) return false; // No se encontró el UUID
        
        fstream archivo(nombreArchivo, ios::in | ios::out | ios::binary);
        if (!archivo) return false;
        
        T registro;
        // Leemos
        archivo.seekg(sizeof(Header) + (pos * sizeof(T)), ios::beg);
        archivo.read(reinterpret_cast<char*>(&registro), sizeof(T));
        
        // Ejecutamos la lambda (regresa true si hay que guardar, false si se cancela o aborta)
        if (accion(registro)) {
            // Escribimos de vuelta
            archivo.seekp(sizeof(Header) + (pos * sizeof(T)), ios::beg);
            archivo.write(reinterpret_cast<const char*>(&registro), sizeof(T));
            archivo.close();
            return true;
        }
        
        archivo.close();
        return false;
    }
};

#endif
