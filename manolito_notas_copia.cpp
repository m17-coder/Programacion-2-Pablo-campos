// SISTEMA DE NOTAS - version "final" (eso dijeron la ultima vez)
// hecho por: el pasante que ya se fue
// modificado por: la profesora Martinez (que tambien se fue)
// funciona, no tocar... o si?

#include <iostream>
#include "COLEGIO.hpp"
#include "estudiantes.hpp"
#include "MENU.hpp"
#include "UTILIDADES.hpp"
#include "VALIDACIONES.hpp"
#include <string>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>

using namespace std;

// ============ VARIABLES GLOBALES ============
 Estudiantes est[80];
 Colegio colegio;
int i;



// ============ DATOS DE PRUEBA ============

void cargarDatosPrueba() {
    // Estudiante 1
    est[0].setNombre("Sofia Ramirez");
    est[0].setMatricula("INGC-1001");
    est[0].setCarrera("Ingenieria");
    est[0].setSemestre(4);
    est[0].setNota(85.0,90.0,88.0);
    est[0].setAsistencia(95.0);
    est[0].setInscrito(true);
    est[0].calcularFinal(colegio);

    // Estudiante 2
    est[1].setNombre("Alejandro Vargas");
    est[1].setMatricula("MEDC-2001");
    est[1].setCarrera("Medicina");
    est[1].setSemestre(6);
    est[1].setNota(72.0, 65.0, 58.0);
    est[1].setAsistencia(82.0);
    est[1].setInscrito(true);
    est[1].calcularFinal(colegio);
    

    // Estudiante 3
    est[2].setNombre("Valentina Morales");
    est[2].setMatricula("DERC-3001");
    est[2].setCarrera("Derecho");
    est[2].setSemestre(3);
    est[2].setNota(92.0, 95.0, 97.0);
    est[2].setAsistencia(98.0);
    est[2].setInscrito(true);
    est[2].calcularFinal(colegio);
    

    // Estudiante 4
    est[3].setNombre("Miguel Angel Herrera");
    est[3].setMatricula("INGC-1002");
    est[3].setCarrera("Ingenieria");
    est[3].setSemestre(4);
    est[3].setNota(55.0, 60.0, 62.0);
    est[3].setAsistencia(70.0);
    est[3].setInscrito(true);
    est[3].calcularFinal(colegio);
    

    // Estudiante 5
    est[4].setNombre("Camila Andrea Rios");
    est[4].setMatricula("PSIC-4001");
    est[4].setCarrera("Psicologia");
    est[4].setSemestre(2);
    est[4].setNota(78.0, 82.0, 75.0);
    est[4].setAsistencia(88.0);
    est[4].setInscrito(true);
    est[4].calcularFinal(colegio);

    // Estudiante 6
    est[5].setNombre("Daniel Felipe Castro");
    est[5].setMatricula("ADMC-5001");
    est[5].setCarrera("Administracion");
    est[5].setSemestre(5);
    est[5].setNota(45.0, 50.0, 40.0);
    est[5].setAsistencia(60.0);
    est[5].setInscrito(true);
    est[5].calcularFinal(colegio);

    // Estudiante 7
    est[6].setNombre("Isabella Jimenez");
    est[6].setMatricula("MEDC-2002");
    est[6].setCarrera("Medicina");
    est[6].setSemestre(6);
    est[6].setNota(88.0, 91.0, 93.0);
    est[6].setAsistencia(96.0);
    est[6].setInscrito(true);
    est[6].calcularFinal(colegio);

    // Estudiante 8 - RETIRADO
    est[7].setNombre("Sebastian Acosta");
    est[7].setMatricula("ARQC-6001");
    est[7].setCarrera("Arquitectura");
    est[7].setSemestre(1);
    est[7].setNota(30.0, 25.0, 20.0);
    est[7].setAsistencia(40.0);
    est[7].setInscrito(false);
    est[7].calcularFinal(colegio);

    // Estudiante 9
    est[8].setNombre("Mariana Lopez Vega");
    est[8].setMatricula("INGC-1003");
    est[8].setCarrera("Ingenieria");
    est[8].setSemestre(7);
    est[8].setNota(63.0, 58.0, 61.0);
    est[8].setAsistencia(73.0);
    est[8].setInscrito(true);
    est[8].calcularFinal(colegio);

    // Estudiante 10
    est[9].setNombre("Andres Felipe Mora");
    est[9].setMatricula("DERC-3002");
    est[9].setCarrera("Derecho");
    est[9].setSemestre(3);
    est[9].setNota(70.0, 75.0, 80.0);
    est[9].setAsistencia(90.0);
    est[9].setInscrito(true);
    est[9].calcularFinal(colegio);

    // Estudiante 11
    est[10].setNombre("Paula Garcia Mendez");
    est[10].setMatricula("PSIC-4002");
    est[10].setCarrera("Psicologia");
    est[10].setSemestre(2);
    est[10].setNota(95.0, 92.0, 98.0);
    est[10].setAsistencia(100.0);
    est[10].setInscrito(true);
    est[10].calcularFinal(colegio);

    // Estudiante 12
    est[11].setNombre("Juan Pablo Ortiz");
    est[11].setMatricula("ADMC-5002");
    est[11].setCarrera("Administracion");
    est[11].setSemestre(8);
    est[11].setNota(60.0, 62.0, 65.0);
    est[11].setAsistencia(78.0);
    est[11].setInscrito(true);
    est[11].calcularFinal(colegio);
    colegio.setCantAlumnos(12);
    colegio.registrarLog("Datos de prueba cargados: 12 estudiantes (1 retirado)");
}

// ============ MAIN ============

int main() {
    cargarDatosPrueba();
    colegio.registrarLog("Sistema de notas iniciado");

    int op,p;
    do {
        menu();
        cin >> op;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Error: Ingrese un numero valido." << endl;
            continue;
        }

        switch (op) {
            case 1: est[colegio.getCantAlumnos()].agregarEstudiante(colegio); break;
            case 2: colegio.verEstudiantes(est); break;
            case 3: 
            cout<<"introduce el numero del estudiante: ";
            cin>>p;
            est[p].calcularNotaIndividual(colegio); break;
            case 4: colegio.calcularTodas(est); break;
            case 5: colegio.buscarEstudiante(est); break;
            case 6: colegio.estadisticas(est); break;
            case 7: colegio.modificarPesos(); break;
            case 8: colegio.tablaHonor(&est[80]); break;
            case 9: colegio.editarEstudiante(); break;
            case 10:
            cout<<"introduce el numero del estudiante: ";
            cin>>p;
            colegio.toggleInscripcion(est[p]); break;
            case 11: reportePorCarrera(); break;
            case 12: exportarNotas(); break;
            case 13: colegio.estudiantesEnRiesgo(&est[80]); break;
            case 14: colegio.verHistorialBoletas(); break;
            case 15: colegio.verLog(); break;
            case 0:
                colegio.registrarLog("Sistema de notas cerrado");
                cout << "Hasta luego." << endl;
                break;
            default: cout << "Opcion invalida." << endl;
        }

        if (op != 0) {
            cout << "\nPresione Enter para continuar...";
            cin.ignore();
            cin.get();
        }
    } while (op != 0);

    return 0;
}
