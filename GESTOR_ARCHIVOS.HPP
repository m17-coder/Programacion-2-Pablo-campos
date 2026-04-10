#ifndef GESTOR_ARCHIVOS_HPP
#define GESTOR_ARCHIVOS_HPP

#include <string>
#include "COLEGIO.hpp"

class GestorArchivos {
public:
    static void agregarEstudiante(Colegio& colegio);
    static void exportarNotas(Colegio& colegio);
    static void cargarDatosPrueba(Colegio& colegio);
};

#endif // GESTOR_ARCHIVOS_HPP