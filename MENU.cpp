#include "MENU.hpp"
#include "UTILIDADES.hpp"
#include "COLEGIO.hpp"
#include "ESTUDIANTES.hpp"
#include <iostream>
#include <string>
// ============ MENU ============
void menu(Colegio& colegio, Estudiantes est[80]) {
    std::cout << std::endl;
    std::cout << lineaSeparadora(55, '*') << std::endl;
    std::cout << "*   SISTEMA DE GESTION DE NOTAS                     *" << std::endl;
    std::cout << "*   Instituto Tecnologico ABC                        *" << std::endl;
    std::cout << "*   " << obtenerFechaActual() << " | " << obtenerHoraActual() << "                           *" << std::endl;
    std::cout << lineaSeparadora(55, '*') << std::endl;
    std::cout << "  1. Agregar estudiante" << std::endl;
    std::cout << "  2. Ver todos los estudiantes" << std::endl;
    std::cout << "  3. Calcular nota final de un estudiante" << std::endl;
    std::cout << "  4. Calcular notas de todos" << std::endl;
    std::cout << "  5. Buscar estudiante" << std::endl;
    std::cout << "  6. Estadisticas del curso" << std::endl;
    std::cout << "  7. Modificar pesos de evaluacion" << std::endl;
    std::cout << "  8. Tabla de honor" << std::endl;
    std::cout << "  9. Editar estudiante" << std::endl;
    std::cout << " 10. Retirar/Reinscribir estudiante" << std::endl;
    std::cout << " 11. Reporte por carrera" << std::endl;
    std::cout << " 12. Exportar notas a archivo" << std::endl;
    std::cout << " 13. Estudiantes en riesgo academico" << std::endl;
    std::cout << " 14. Ver historial de boletas" << std::endl;
    std::cout << " 15. Ver log del sistema" << std::endl;
    std::cout << "  0. Salir" << std::endl;
    std::cout << lineaSeparadora(55, '-') << std::endl;
    int activos = 0;
    for (int i = 0; i < colegio.getCantAlumnos(); i++) {
        if (est[i].getInscrito()) activos++;
    }
    std::cout << "  Estudiantes inscritos: " << activos << "/" << colegio.getCantAlumnos() << std::endl;
    std::cout << "Seleccione: ";
}
