#include "GESTOR_ARCHIVOS.hpp"
#include <string>
#include <fstream>
#include <iostream>

if (cantAlumnos >= 80) {
        std::cout << "ERROR: Capacidad maxima alcanzada (80 estudiantes)." << std::endl;
        registrarLog("ERROR: Intento de registro con capacidad maxima");
        return;
    }

    std::cout << "\n--- REGISTRO DE NUEVO ESTUDIANTE ---" << std::endl;

    // nombre
    std::string nombre;
    bool nombreValido = false;
    do {
        std::cout << "Nombre completo: ";
        cin.ignore();
        getline(cin, nombre);
        nombre = recortarEspacios(nombre);
        if (!validarNombre(nombre)) {
            cout << "Error: Nombre invalido (3-60 caracteres, solo letras y espacios)." << std::endl;
        } else {
            nombreValido = true;
        }
    } while (!nombreValido);

    // matricula
    std::string mat;
    bool matValida = false;
    do {
        std::cout << "Matricula (formato AAAA-NNNN): ";
        getline(cin, mat);
        mat = recortarEspacios(mat);
        if (!validarMatricula(mat)) {
            std::cout << "Error: Formato invalido. Use 4 letras, guion, 4 numeros." << std::endl;
        } else if (!validarMatriculaUnica(mat)) {
            std::cout << "Error: Ya existe un estudiante con esa matricula." << std::endl;
        } else {
            mat = aMayusculas(mat);
            matValida = true;
        }
    } while (!matValida);

    // carrera
    std::string carr;
    bool carrValida = false;
    std::cout << "Carreras disponibles: ";
    for (int i = 0; i < totalCarreras; i++) {
        cout << carrerasValidas[i];
        if (i < totalCarreras - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    do {
        std::cout << "Carrera: ";
        getline(cin, carr);
        carr = recortarEspacios(carr);
        if (!validarCarrera(carr)) {
            cout << "Error: Carrera no valida." << std::endl;
        } else {
            for (int i = 0; i < totalCarreras; i++) {
                if (compararIgnorandoCase(carr, carrerasValidas[i])) {
                    carr = carrerasValidas[i];
                    break;
                }
            }
            carrValida = true;
        }
    } while (!carrValida);

    // semestre
    int sem = validarEntradaEntera("Semestre (1-12): ", 1, 12);

    // notas
    double n1 = validarEntradaDecimal("Nota parcial 1 (0-100): ", 0, 100);
    double n2 = validarEntradaDecimal("Nota parcial 2 (0-100): ", 0, 100);
    double n3 = validarEntradaDecimal("Nota parcial 3 (0-100): ", 0, 100);

    // asistencia
    double asist = validarEntradaDecimal("Porcentaje de asistencia (0-100): ", 0, 100);

    // guardar
    int idx = cantAlumnos;
    alumno[idx] = nombre;
    matricula[idx] = mat;
    carrera[idx] = carr;
    semestre[idx] = sem;
    nota1[idx] = n1;
    nota2[idx] = n2;
    nota3[idx] = n3;
    asistencia[idx] = asist;
    inscrito[idx] = true;
    notaFinal[idx] = 0;
    cantAlumnos++;

    std::cout << "\nEstudiante '" << nombre << "' registrado con matricula " << mat << "." << std::endl;
    registrarLog("Estudiante registrado: " + nombre + " (" + mat + ", " + carr + ", Sem " + to_string(sem) + ")");

}

void exportarNotas() { // Exporta el archivo de texto con los datos de prueba, va en el gestor de archivos 
    if (cantAlumnos == 0) {
        std::cout << "No hay datos para exportar." << std::endl;
        return;
    }

    std::string nombreArchivo = "notas_" + obtenerFechaActual() + ".txt"; 
    for (int i = 0; i < (int)nombreArchivo.length(); i++) {
        if (nombreArchivo[i] == '/') nombreArchivo[i] = '-';
    }

    ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        std::cout << "Error: No se pudo crear el archivo." << endl;
        return;
    }

    archivo << "REPORTE DE NOTAS - INSTITUTO TECNOLOGICO ABC" << std::endl;
    archivo << "Fecha: " << obtenerFechaActual() << " " << obtenerHoraActual() << std::endl;
    archivo << lineaSeparadora(90, '=') << std::endl;
    archivo << rellenarDerecha("NOMBRE", 25) << rellenarDerecha("MATRICULA", 12)
            << rellenarDerecha("CARRERA", 16) << rellenarDerecha("P1", 7)
            << rellenarDerecha("P2", 7) << rellenarDerecha("P3", 7)
            << rellenarDerecha("FINAL", 8) << rellenarDerecha("ESTADO", 12) << std::endl;
    archivo << lineaSeparadora(90, '-') << std::endl;

    for (int i = 0; i < cantAlumnos; i++) {
        if (!inscrito[i]) continue;
        double f = calcularFinal(i);
        archivo << rellenarDerecha(alumno[i], 25)
                << rellenarDerecha(matricula[i], 12)
                << rellenarDerecha(carrera[i], 16)
                << rellenarDerecha(formatoNota(nota1[i]), 7)
                << rellenarDerecha(formatoNota(nota2[i]), 7)
                << rellenarDerecha(formatoNota(nota3[i]), 7)
                << rellenarDerecha(formatoNota(f), 8)
                << rellenarDerecha(obtenerEstado(f), 12) << endl;
    }

    archivo << lineaSeparadora(90, '=') << std::endl;
    archivo << "Aprobados: " << aprobados << " | Reprobados: " << reprobados << std::endl;
    archivo.close();

    std::cout << "Notas exportadas a: " << nombreArchivo << std::endl;
    registrarLog("Notas exportadas a: " + nombreArchivo);
}

// ============ DATOS DE PRUEBA ============

void cargarDatosPrueba() {
    // Estudiante 1
    alumno[0] = "Sofia Ramirez";
    matricula[0] = "INGC-1001";
    carrera[0] = "Ingenieria";
    semestre[0] = 4;
    nota1[0] = 85.0; nota2[0] = 90.0; nota3[0] = 88.0;
    asistencia[0] = 95.0;
    inscrito[0] = true;
    notaFinal[0] = 0;

    // Estudiante 2
    alumno[1] = "Alejandro Vargas";
    matricula[1] = "MEDC-2001";
    carrera[1] = "Medicina";
    semestre[1] = 6;
    nota1[1] = 72.0; nota2[1] = 65.0; nota3[1] = 58.0;
    asistencia[1] = 82.0;
    inscrito[1] = true;
    notaFinal[1] = 0;

    // Estudiante 3
    alumno[2] = "Valentina Morales";
    matricula[2] = "DERC-3001";
    carrera[2] = "Derecho";
    semestre[2] = 3;
    nota1[2] = 92.0; nota2[2] = 95.0; nota3[2] = 97.0;
    asistencia[2] = 98.0;
    inscrito[2] = true;
    notaFinal[2] = 0;

    // Estudiante 4
    alumno[3] = "Miguel Angel Herrera";
    matricula[3] = "INGC-1002";
    carrera[3] = "Ingenieria";
    semestre[3] = 4;
    nota1[3] = 55.0; nota2[3] = 60.0; nota3[3] = 62.0;
    asistencia[3] = 70.0;
    inscrito[3] = true;
    notaFinal[3] = 0;

    // Estudiante 5
    alumno[4] = "Camila Andrea Rios";
    matricula[4] = "PSIC-4001";
    carrera[4] = "Psicologia";
    semestre[4] = 2;
    nota1[4] = 78.0; nota2[4] = 82.0; nota3[4] = 75.0;
    asistencia[4] = 88.0;
    inscrito[4] = true;
    notaFinal[4] = 0;

    // Estudiante 6
    alumno[5] = "Daniel Felipe Castro";
    matricula[5] = "ADMC-5001";
    carrera[5] = "Administracion";
    semestre[5] = 5;
    nota1[5] = 45.0; nota2[5] = 50.0; nota3[5] = 40.0;
    asistencia[5] = 60.0;
    inscrito[5] = true;
    notaFinal[5] = 0;

    // Estudiante 7
    alumno[6] = "Isabella Jimenez";
    matricula[6] = "MEDC-2002";
    carrera[6] = "Medicina";
    semestre[6] = 6;
    nota1[6] = 88.0; nota2[6] = 91.0; nota3[6] = 93.0;
    asistencia[6] = 96.0;
    inscrito[6] = true;
    notaFinal[6] = 0;

    // Estudiante 8 - RETIRADO
    alumno[7] = "Sebastian Acosta";
    matricula[7] = "ARQC-6001";
    carrera[7] = "Arquitectura";
    semestre[7] = 1;
    nota1[7] = 30.0; nota2[7] = 25.0; nota3[7] = 20.0;
    asistencia[7] = 40.0;
    inscrito[7] = false;
    notaFinal[7] = 0;

    // Estudiante 9
    alumno[8] = "Mariana Lopez Vega";
    matricula[8] = "INGC-1003";
    carrera[8] = "Ingenieria";
    semestre[8] = 7;
    nota1[8] = 63.0; nota2[8] = 58.0; nota3[8] = 61.0;
    asistencia[8] = 73.0;
    inscrito[8] = true;
    notaFinal[8] = 0;

    // Estudiante 10
    alumno[9] = "Andres Felipe Mora";
    matricula[9] = "DERC-3002";
    carrera[9] = "Derecho";
    semestre[9] = 3;
    nota1[9] = 70.0; nota2[9] = 75.0; nota3[9] = 80.0;
    asistencia[9] = 90.0;
    inscrito[9] = true;
    notaFinal[9] = 0;

    // Estudiante 11
    alumno[10] = "Paula Garcia Mendez";
    matricula[10] = "PSIC-4002";
    carrera[10] = "Psicologia";
    semestre[10] = 2;
    nota1[10] = 95.0; nota2[10] = 92.0; nota3[10] = 98.0;
    asistencia[10] = 100.0;
    inscrito[10] = true;
    notaFinal[10] = 0;

    // Estudiante 12
    alumno[11] = "Juan Pablo Ortiz";
    matricula[11] = "ADMC-5002";
    carrera[11] = "Administracion";
    semestre[11] = 8;
    nota1[11] = 60.0; nota2[11] = 62.0; nota3[11] = 65.0;
    asistencia[11] = 78.0;
    inscrito[11] = true;
    notaFinal[11] = 0;

    cantAlumnos = 12;
    registrarLog("Datos de prueba cargados: 12 estudiantes (1 retirado)");
}
