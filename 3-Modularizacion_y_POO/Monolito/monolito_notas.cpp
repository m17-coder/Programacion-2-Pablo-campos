// SISTEMA DE NOTAS - version "final" (eso dijeron la ultima vez)
// hecho por: el pasante que ya se fue
// modificado por: la profesora Martinez (que tambien se fue)
// funciona, no tocar... o si?

#include <iostream>
#include "COLEGIO.hpp"
#include "ESTUDIANTES.hpp"
#include "MENU.hpp"
#include "UTILIDADES.hpp"
#include "VALIDACIONES.hpp"
#include <string>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>

Colegio colegio;
Estudiantes est[80];

// ============ MAIN ============

int main() {
    cargarDatosPrueba();
    colegio.registrarLog("Sistema de notas iniciado");

    int op,p;
    do {
        menu(colegio, est);
        std::cin >> op;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Error: Ingrese un numero valido." << std::endl;
            continue;
        }

        switch (op) {
            case 1: est[colegio.getCantAlumnos()].agregarEstudiante(colegio); break;
            case 2: colegio.verEstudiantes(est); break;
            case 3: 
            std::cout<<"introduce el numero del estudiante: ";
            std::cin>>p;
            est[p].calcularNotaIndividual(colegio); break;
            case 4: colegio.calcularTodas(est); break;
            case 5: colegio.buscarEstudiante(est); break;
            case 6: colegio.estadisticas(est); break;
            case 7: colegio.modificarPesos(); break;
            case 8: colegio.tablaHonor(est); break;
            case 9: colegio.editarEstudiante(est); break;
            case 10:
            std::cout<<"introduce el numero del estudiante: ";
            std::cin>>p;
            colegio.toggleInscripcion(est[p]); break;
            case 11: colegio.reportePorCarrera(est); break;
            case 12: colegio.exportarNotas(est); break;
            case 13: colegio.estudiantesEnRiesgo(est); break;
            case 14: colegio.verHistorialBoletas(); break;
            case 15: colegio.verLog(); break;
            case 0:
                colegio.registrarLog("Sistema de notas cerrado");
                std::cout << "Hasta luego." << std::endl;
                break;
            default: std::cout << "Opcion invalida." << std::endl;
        }

        if (op != 0) {
            std::cout << "\nPresione Enter para continuar...";
            std::cin.ignore();
            std::cin.get();
        }
    } while (op != 0);

    return 0;
}
