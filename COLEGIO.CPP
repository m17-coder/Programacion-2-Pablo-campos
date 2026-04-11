#include "COLEGIO.hpp"
#include "VALIDACIONES.hpp"
#include "UTILIDADES.hpp"
#include <iostream>
#include <cmath>
#include<fstream>

bool Colegio::validarCarrera(std::string carr){
    for (int i = 0; i < totalCarreras; i++) {
        if (compararIgnorandoCase(carr, carrerasValidas[i])) return true;
    }
    return false;
}
void Colegio::registrarLog(std::string evento) {
    if (totalLog >= 500) {
        for (int i = 0; i < 499; i++) {
            logEventos[i] = logEventos[i + 1];
        }
        totalLog = 499;
    }
    logEventos[totalLog] = "[" + obtenerFechaActual() + " " + obtenerHoraActual() + "] " + evento;
    totalLog++;
}
void Colegio::verLog() {
    if (totalLog == 0) {
        std::cout << "No hay eventos." << std::endl;
        return;
    }

    std::cout << "\n--- LOG DEL SISTEMA ---" << std::endl;
    int inicio = totalLog > 30 ? totalLog - 30 : 0;
    for (int i = inicio; i < totalLog; i++) {
        std::cout << logEventos[i] << std::endl;
    }
    std::cout << lineaSeparadora(50, '-') << std::endl;
    std::cout << "Mostrando ultimos " << totalLog - inicio << " de " << totalLog << " eventos." << std::endl;
}
void Colegio::modificarPesos() {
    std::cout << "\n--- CONFIGURACION ACADEMICA ---" << std::endl;
    std::cout << "Pesos actuales -> P1: " << pesoP1 * 100 << "%, P2: " << pesoP2 * 100
         << "%, P3: " << pesoP3 * 100 << "%" << std::endl;
    std::cout << "Nuevo peso P1 (ej: 0.25 para 25%): ";
    std::cin >> pesoP1;
    std::cout << "Nuevo peso P2: ";
    std::cin >> pesoP2;
    std::cout << "Nuevo peso P3: ";
    std::cin >> pesoP3;

    double total = pesoP1 + pesoP2 + pesoP3;
    if (abs(total - 1.0) > 0.01) {
        std::cout << "ADVERTENCIA: Los pesos suman " << total * 100 << "%, no 100%." << std::endl;
        std::cout << "Se normalizaran automaticamente." << std::endl;
        pesoP1 /= total;
        pesoP2 /= total;
        pesoP3 /= total;
    }
    std::cout << "Pesos actualizados: P1=" << pesoP1 * 100 << "%, P2=" << pesoP2 * 100
         << "%, P3=" << pesoP3 * 100 << "%" << std::endl;

    std::cout << "\nNota minima aprobatoria actual: " << notaMinima << std::endl;
    notaMinima = validarEntradaDecimal("Nueva nota minima: ", 0, 100);

    std::cout << "Nota minima para tabla de honor actual: " << notaMinimaHonor << std::endl;
    notaMinimaHonor = validarEntradaDecimal("Nueva nota minima honor: ", 0, 100);

    std::cout << "Asistencia minima actual: " << asistenciaMinima << "%" << std::endl;
    asistenciaMinima = validarEntradaDecimal("Nueva asistencia minima (%): ", 0, 100);

    std::cout << "Configuracion guardada." << std::endl;
    registrarLog("Configuracion academica modificada");
}
void Colegio::calcularTodas(Estudiantes est[80]) {
    if (cantAlumnos == 0) {
        std::cout << "No hay estudiantes." << std::endl;
        return;
    }
    aprobados = 0;
    reprobados = 0;

    std::cout << "\n" << lineaSeparadora(80, '=') << std::endl;
    std::cout << "                NOTAS FINALES - " << obtenerFechaActual() << std::endl;
    std::cout << lineaSeparadora(80, '=') << std::endl;
    std::cout << rellenarDerecha("NOMBRE", 25) << rellenarDerecha("CARRERA", 16)
        << rellenarDerecha("FINAL", 8) << rellenarDerecha("LETRA", 7)
        << rellenarDerecha("ESTADO", 12) << rellenarDerecha("ASIST", 10) << std::endl;
    std::cout << lineaSeparadora(80, '-') << std::endl;

    for (int i = 0; i < cantAlumnos; i++) {
        if (!est[i].getInscrito()) continue;
        double f = est[i].calcularFinal(* this);
        std::string estado = obtenerEstado(f);
        std::string letra = obtenerLetra(f);
        std::string estadoAsist = obtenerEstadoAsistencia(est[i].getAsistencia());

        std::cout << rellenarDerecha(est[i].getNombre(), 25)
            << rellenarDerecha(est[i].getCarrera(), 16)
            << rellenarDerecha(formatoNota(f), 8)
            << rellenarDerecha(letra, 7)
            << rellenarDerecha(estado, 12)
            << rellenarDerecha(estadoAsist, 10) << std::endl;

        if (f >= notaMinima) aprobados++;
        else reprobados++;
    }

    std::cout << lineaSeparadora(80, '-') << std::endl;
    std::cout << "Aprobados: " << aprobados << " | Reprobados: " << reprobados << std::endl;
    std::cout << lineaSeparadora(80, '=') << std::endl;
    registrarLog("Calculo masivo de notas realizado");
}
std::string Colegio::obtenerLetra(double nota) {
    if (nota >= 90) return "A";
    if (nota >= 80) return "B";
    if (nota >= 70) return "C";
    if (nota >= 60) return "D";
    return "F";
}
std::string Colegio::obtenerEstado(double nota) {
    if (nota >= notaMinima) return "APROBADO";
    return "REPROBADO";
}
void Colegio::verEstudiantes(Estudiantes est[80]) {
    if (cantAlumnos == 0) {
        std::cout << "No hay estudiantes registrados." << std::endl;
        return;
    }

    std::cout << "\n" << lineaSeparadora(120, '=') << std::endl;
    std::cout << rellenarDerecha("ID", 5) << rellenarDerecha("NOMBRE", 25) << rellenarDerecha("MATRICULA", 12)
        << rellenarDerecha("CARRERA", 16) << rellenarDerecha("SEM", 5) << rellenarDerecha("P1", 7)
        << rellenarDerecha("P2", 7) << rellenarDerecha("P3", 7) << rellenarDerecha("FINAL", 8)
        << rellenarDerecha("ASIST", 8) << rellenarDerecha("ESTADO", 12) << std::endl;
    std::cout << lineaSeparadora(120, '-') << std::endl;

    for (int i = 0; i < cantAlumnos; i++) {
        std::string estado = est[i].getInscrito() ? "Inscrito" : "Retirado";
        std::cout << rellenarDerecha(std::to_string(i + 1), 5)
            << rellenarDerecha(est[i].getNombre(), 25)
            << rellenarDerecha(est[i].getMatricula(), 12)
            << rellenarDerecha(est[i].getCarrera(), 16)
            << rellenarDerecha(std::to_string(est[i].getSemestre()), 5)
            << rellenarDerecha(formatoNota(est[i].getNota1()), 7)
            << rellenarDerecha(formatoNota(est[i].getNota2()), 7)
            << rellenarDerecha(formatoNota(est[i].getNota3()), 7)
            << rellenarDerecha(formatoNota(est[i].getNotaFinal()), 8)
            << rellenarDerecha(formatoNota(est[i].getAsistencia()) + "%", 8)
            << rellenarDerecha(estado, 12) << std::endl;
    }
    std::cout << lineaSeparadora(120, '=') << std::endl;
    std::cout << "Total: " << cantAlumnos << " estudiantes" << std::endl;
    registrarLog("Consulta de lista de estudiantes");
}
void Colegio::verHistorialBoletas() {
    if (totalHistorial == 0) {
        std::cout << "No hay historial de boletas." << std::endl;
        return;
    }

    std::cout << "\n--- HISTORIAL DE BOLETAS GENERADAS ---" << std::endl;
    std::cout << rellenarDerecha("FECHA", 15) << rellenarDerecha("ESTUDIANTE", 25)
        << rellenarDerecha("NOTA", 10) << std::endl;
    std::cout << lineaSeparadora(50, '-') << std::endl;

    int inicio = totalHistorial > 20 ? totalHistorial - 20 : 0;
    for (int i = inicio; i < totalHistorial; i++) {
        std::cout << rellenarDerecha(historialFecha[i], 15)
            << rellenarDerecha(historialAlumno[i], 25)
            << rellenarDerecha(formatoNota(historialNota[i]), 10) << std::endl;
    }
    std::cout << lineaSeparadora(50, '-') << std::endl;
    std::cout << "Mostrando ultimos " << totalHistorial - inicio << " de " << totalHistorial << std::endl;
}
void Colegio::estudiantesEnRiesgo(Estudiantes est[80]) {
    if (cantAlumnos == 0) {
        std::cout << "No hay estudiantes." << std::endl;
        return;
    }

    std::cout << "\n" << lineaSeparadora(75, '=') << std::endl;
    std::cout << "         ESTUDIANTES EN RIESGO ACADEMICO" << std::endl;
    std::cout << lineaSeparadora(75, '=') << std::endl;

    int enRiesgo = 0;
    for (int i = 0; i < cantAlumnos; i++) {
        if (!est[i].getInscrito()) continue;
        if (est[i].estaEnRiesgo(*this)) {
            double f = est[i].getNotaFinal();
            std::string razones = "";
            if (f < notaMinima) razones += "Nota baja";
            if (est[i].getAsistencia() < asistenciaMinima) {
                if (razones.length() > 0) razones += ", ";
                razones += "Baja asistencia";
            }
            if (f >= 55 && f <= 65) {
                if (razones.length() > 0 && razones.find("Nota baja") == std::string::npos) razones += ", ";
                if (razones.find("Nota baja") == std::string::npos) razones += "Zona critica (55-65)";
            }

            std::cout << "  " << rellenarDerecha(est[i].getNombre(), 25)
                << " | " << est[i].getMatricula()
                << " | Final: " << formatoNota(f)
                << " | Asist: " << formatoNota(est[i].getAsistencia()) << "%"
                << " | Razon: " << razones << std::endl;
            enRiesgo++;
        }
    }

    if (enRiesgo == 0) {
        std::cout << "  No hay estudiantes en riesgo academico." << std::endl;
    } else {
        std::cout << lineaSeparadora(75, '-') << std::endl;
        std::cout << "  Total en riesgo: " << enRiesgo << " estudiantes" << std::endl;
    }
    std::cout << lineaSeparadora(75, '=') << std::endl;
    registrarLog("Consulta de estudiantes en riesgo: " + std::to_string(enRiesgo) + " encontrados");
}
void Colegio::tablaHonor(Estudiantes est[80]) {
    if (cantAlumnos == 0) {
        std::cout << "No hay estudiantes." << std::endl;
        return;
    }

    // calcular todas primero
    for (int i = 0; i < cantAlumnos; i++) {
        if (est[i].getInscrito()) est[i].calcularFinal(*this);
    }

    // copiar activos con nota >= notaMinimaHonor y asistencia >= asistenciaMinima
    std::string nombresHonor[80];
    std::string matHonor[80];
    std::string carrHonor[80];
    double notasHonor[80];
    double asistHonor[80];
    int cantHonor = 0;

    for (int i = 0; i < cantAlumnos; i++) {
        if (!est[i].getInscrito()) continue;
        if (est[i].getNotaFinal() >= notaMinimaHonor && est[i].getAsistencia() >= asistenciaMinima) {
            nombresHonor[cantHonor] = est[i].getNombre();
            matHonor[cantHonor] = est[i].getMatricula();
            carrHonor[cantHonor] = est[i].getCarrera();
            notasHonor[cantHonor] = est[i].getNotaFinal();
            asistHonor[cantHonor] = est[i].getAsistencia();
            cantHonor++;
        }
    }

    if (cantHonor == 0) {
        std::cout << "No hay estudiantes que cumplan los requisitos para la tabla de honor." << std::endl;
        std::cout << "(Nota >= " << notaMinimaHonor << " y Asistencia >= " << asistenciaMinima << "%)" << std::endl;
        return;
    }

    // ordenar por nota (burbuja)
    for (int i = 0; i < cantHonor - 1; i++) {
        for (int j = 0; j < cantHonor - i - 1; j++) {
            if (notasHonor[j] < notasHonor[j + 1]) {
                double tempN = notasHonor[j]; notasHonor[j] = notasHonor[j + 1]; notasHonor[j + 1] = tempN;
                double tempA = asistHonor[j]; asistHonor[j] = asistHonor[j + 1]; asistHonor[j + 1] = tempA;
                std::string tempS = nombresHonor[j]; nombresHonor[j] = nombresHonor[j + 1]; nombresHonor[j + 1] = tempS;
                std::string tempM = matHonor[j]; matHonor[j] = matHonor[j + 1]; matHonor[j + 1] = tempM;
                std::string tempC = carrHonor[j]; carrHonor[j] = carrHonor[j + 1]; carrHonor[j + 1] = tempC;
            }
        }
    }

    std::cout << "\n" << lineaSeparadora(75, '=') << std::endl;
    std::cout << "         TABLA DE HONOR" << std::endl;
    std::cout << "         (Nota >= " << notaMinimaHonor << " | Asistencia >= " << asistenciaMinima << "%)" << std::endl;
    std::cout << lineaSeparadora(75, '=') << std::endl;
    std::cout << rellenarDerecha("POS", 5) << rellenarDerecha("NOMBRE", 25) << rellenarDerecha("MATRICULA", 12)
         << rellenarDerecha("CARRERA", 16) << rellenarDerecha("NOTA", 8) << rellenarDerecha("ASIST", 8) << std::endl;
    std::cout << lineaSeparadora(75, '-') << std::endl;

    for (int i = 0; i < cantHonor; i++) {
        std::string medalla = "";
        if (i == 0) medalla = " ***";
        else if (i == 1) medalla = " **";
        else if (i == 2) medalla = " *";

        std::cout << rellenarDerecha(std::to_string(i + 1), 5)
            << rellenarDerecha(nombresHonor[i] + medalla, 25)
            << rellenarDerecha(matHonor[i], 12)
            << rellenarDerecha(carrHonor[i], 16)
            << rellenarDerecha(formatoNota(notasHonor[i]), 8)
            << rellenarDerecha(formatoNota(asistHonor[i]) + "%", 8) << std::endl;
    }
    std::cout << lineaSeparadora(75, '=') << std::endl;
    registrarLog("Tabla de honor generada con " + std::to_string(cantHonor) + " estudiantes");
}
std::string Colegio::obtenerEstadoAsistencia(double pctAsist) {
    if (pctAsist >= asistenciaMinima) return "Cumple";
    return "NO CUMPLE";
}
double Colegio::getPesoP1() { return pesoP1; }
double Colegio::getPesoP2() { return pesoP2; }
double Colegio::getPesoP3() { return pesoP3; }
double Colegio::getNotaMinima() { return notaMinima; }
double Colegio::getNotaMinimaHonor() { return notaMinimaHonor; }
double Colegio::getAsistenciaMinima() { return asistenciaMinima; }
int Colegio::getAprobados() { return aprobados; }
int Colegio::getReprobados() { return reprobados; }
int Colegio::getCantAlumnos() { return cantAlumnos; }
void Colegio::setCantAlumnos(int cant) { cantAlumnos = cant; }
void Colegio::setAprovados(int ap) { aprobados = ap; }
void Colegio::setReprobados(int rep) { reprobados = rep; }
void Colegio::toggleInscripcion(Estudiantes est) {
    if (cantAlumnos == 0) {
        std::cout << "No hay estudiantes." << std::endl;
        return;
    }
    int num = validarEntradaEntera("Numero de estudiante (1-" + std::to_string(cantAlumnos) + "): ", 1, cantAlumnos);
    num--;

    std::cout << est.getNombre() << " esta actualmente: " << (est.getInscrito() ? "INSCRITO" : "RETIRADO") << std::endl;
    std::cout << "Desea " << (est.getInscrito() ? "RETIRAR" : "REINSCRIBIR") << "? (s/n): ";
    char resp;
    std::cin >> resp;
    if (resp == 's' || resp == 'S') {
        est.setInscrito(!est.getInscrito());
        std::string nuevoEstado = est.getInscrito() ? "reinscrito" : "retirado";
        std::cout << "Estudiante " << nuevoEstado << "." << std::endl;
        registrarLog("Estudiante " + nuevoEstado + ": " + est.getNombre());
    }
}
int Colegio:: getcantidadcarreras(){
    return totalCarreras;
}

void Colegio::buscarEstudiante(Estudiantes est[]) {
    if (cantAlumnos == 0) {
        std::cout << "No hay estudiantes." << std::endl;
        return;
    }
    std::cout << "Buscar por: 1) Nombre  2) Matricula  3) Carrera" << std::endl;
    int tipo = validarEntradaEntera("Opcion: ", 1, 3);

    std::string busq;
    std::cout << "Texto a buscar: ";
    std::cin.ignore();
    std::getline(std::cin, busq);
    busq = recortarEspacios(busq);

    bool found = false;
    int resultados = 0;

    std::cout << "\n--- RESULTADOS ---" << std::endl;
    for (int i = 0; i < cantAlumnos; i++) {
        bool match = false;
        switch (tipo) {
            case 1: match = contieneTexto(est[i].getNombre(), busq); break;
            case 2: match = contieneTexto(est[i].getMatricula(), busq); break;
            case 3: match = contieneTexto(est[i].getCarrera(), busq); break;
        }
        if (match) {
            std::string estado = est[i].getInscrito() ? "Inscrito" : "Retirado";
            std::cout << "#" << i + 1 << " | " << est[i].getNombre() << " | " << est[i].getMatricula()
                 << " | " << est[i].getCarrera() << " | Sem " << est[i].getSemestre()
                 << " | Final: " << formatoNota(est[i].getNotaFinal())
                 << " | " << estado << std::endl;
            found = true;
            resultados++;
        }
    }
    if (!found) {
        std::cout << "No se encontraron resultados." << std::endl;
    } else {
        std::cout << "Total encontrados: " << resultados << std::endl;
    }
    registrarLog("Busqueda: '" + busq + "' - " + std::to_string(resultados) + " resultados");
}

void Colegio::estadisticas(Estudiantes est[]) {
    if (cantAlumnos == 0) {
        std::cout << "No hay datos." << std::endl;
        return;
    }

    double suma = 0, maxNota = 0, minNota = 100;
    std::string mejorAlumno = "", peorAlumno = "";
    aprobados = 0; reprobados = 0;
    int cantActivos = 0;
    double sumaAsistencia = 0;
    int bajosAsistencia = 0;

    for (int i = 0; i < cantAlumnos; i++) {
        if (!est[i].getInscrito()) continue;
        cantActivos++;
        double f = est[i].getNotaFinal();
        suma += f;
        sumaAsistencia += est[i].getAsistencia();
        if (est[i].getAsistencia() < asistenciaMinima) bajosAsistencia++;
        if (f > maxNota) { maxNota = f; mejorAlumno = est[i].getNombre(); }
        if (f < minNota) { minNota = f; peorAlumno = est[i].getNombre(); }
        if (f >= notaMinima) aprobados++;
        else reprobados++;
    }

    if (cantActivos == 0) {
        std::cout << "No hay estudiantes inscritos." << std::endl;
        return;
    }

    double promedio = suma / cantActivos;
    double promedioAsist = sumaAsistencia / cantActivos;

    // desviacion estandar
    double sumaDif = 0;
    for (int i = 0; i < cantAlumnos; i++) {
        if (!est[i].getInscrito()) continue;
        sumaDif += pow(est[i].getNotaFinal() - promedio, 2);
    }
    double desviacion = sqrt(sumaDif / cantActivos);

    // mediana
    double notasOrdenadas[80];
    int k = 0;
    for (int i = 0; i < cantAlumnos; i++) {
        if (!est[i].getInscrito()) continue;
        notasOrdenadas[k++] = est[i].getNotaFinal();
    }
    // burbuja para ordenar
    for (int i = 0; i < k - 1; i++) {
        for (int j = 0; j < k - i - 1; j++) {
            if (notasOrdenadas[j] > notasOrdenadas[j + 1]) {
                double temp = notasOrdenadas[j];
                notasOrdenadas[j] = notasOrdenadas[j + 1];
                notasOrdenadas[j + 1] = temp;
            }
        }
    }
    double mediana;
    if (k % 2 == 0) {
        mediana = (notasOrdenadas[k / 2 - 1] + notasOrdenadas[k / 2]) / 2.0;
    } else {
        mediana = notasOrdenadas[k / 2];
    }

    // contar por letra
    int contA = 0, contB = 0, contC = 0, contD = 0, contF = 0;
    for (int i = 0; i < cantAlumnos; i++) {
        if (!est[i].getInscrito()) continue;
        std::string letra = obtenerLetra(est[i].getNotaFinal());
        if (letra == "A") contA++;
        else if (letra == "B") contB++;
        else if (letra == "C") contC++;
        else if (letra == "D") contD++;
        else contF++;
    }

    double pctAprobados = (aprobados * 100.0) / cantActivos;

    std::cout << "\n" << lineaSeparadora(60, '=') << std::endl;
    std::cout << "         ESTADISTICAS DEL CURSO" << std::endl;
    std::cout << "         " << obtenerFechaActual() << std::endl;
    std::cout << lineaSeparadora(60, '=') << std::endl;
    std::cout << "POBLACION:" << std::endl;
    std::cout << "  Total registrados:   " << cantAlumnos << std::endl;
    std::cout << "  Inscritos activos:   " << cantActivos << std::endl;
    std::cout << "  Retirados:           " << cantAlumnos - cantActivos << std::endl;
    std::cout << lineaSeparadora(60, '-') << std::endl;
    std::cout << "NOTAS:" << std::endl;
    std::cout << "  Promedio general:    " << formatoNota(promedio) << std::endl;
    std::cout << "  Mediana:             " << formatoNota(mediana) << std::endl;
    std::cout << "  Desviacion estandar: " << formatoNota(desviacion) << std::endl;
    std::cout << "  Nota mas alta:       " << mejorAlumno << " (" << formatoNota(maxNota) << ")" << std::endl;
    std::cout << "  Nota mas baja:       " << peorAlumno << " (" << formatoNota(minNota) << ")" << std::endl;
    std::cout << lineaSeparadora(60, '-') << std::endl;
    std::cout << "APROBACION:" << std::endl;
    std::cout << "  Aprobados:           " << aprobados << " (" << formatoNota(pctAprobados) << "%)" << std::endl;
    std::cout << "  Aprobacion: " << barraProgreso(pctAprobados, 30) << std::endl;
    std::cout << "  Reprobados:          " << reprobados << std::endl;
    std::cout << lineaSeparadora(60, '-') << std::endl;
    std::cout << "ASISTENCIA:" << std::endl;
    std::cout << "  Promedio asistencia: " << formatoNota(promedioAsist) << "%" << std::endl;
    std::cout << "  Debajo del minimo:   " << bajosAsistencia << " estudiantes" << std::endl;
    std::cout << lineaSeparadora(60, '-') << std::endl;
    std::cout << "DISTRIBUCION POR LETRA:" << std::endl;
    std::cout << "  A (90-100): " << contA << "  " << barraProgreso(contA * 100.0 / cantActivos, 15) << std::endl;
    std::cout << "  B (80-89):  " << contB << "  " << barraProgreso(contB * 100.0 / cantActivos, 15) << std::endl;
    std::cout << "  C (70-79):  " << contC << "  " << barraProgreso(contC * 100.0 / cantActivos, 15) << std::endl;
    std::cout << "  D (60-69):  " << contD << "  " << barraProgreso(contD * 100.0 / cantActivos, 15) << std::endl;
    std::cout << "  F (0-59):   " << contF << "  " << barraProgreso(contF * 100.0 / cantActivos, 15) << std::endl;
    std::cout << lineaSeparadora(60, '=') << std::endl;
    registrarLog("Estadisticas del curso generadas");
}

void Colegio:: editarEstudiante(Estudiantes est[80]) {
    if (getCantAlumnos() == 0) {
        std::cout << "No hay estudiantes." << std::endl;
        return;
    }
    int num = validarEntradaEntera("Numero de estudiante (1-" + std::to_string(getCantAlumnos()) + "): ", 1, getCantAlumnos());
    num--;

    std::cout << "\nEditando: " << est[num].getNombre() << " (" << est[num].getMatricula() << ")" << std::endl;
    std::cout << "1. Nombre (" << est[num].getNombre() << ")" << std::endl;
    std::cout << "2. Carrera (" << est[num].getCarrera() << ")" << std::endl;
    std::cout << "3. Semestre (" << est[num].getSemestre() << ")" << std::endl;
    std::cout << "4. Nota P1 (" << formatoNota(est[num].getNota1()) << ")" << std::endl;
    std::cout << "5. Nota P2 (" << formatoNota(est[num].getNota2()) << ")" << std::endl;
    std::cout << "6. Nota P3 (" << formatoNota(est[num].getNota3()) << ")" << std::endl;
    std::cout << "7. Asistencia (" << formatoNota(est[num].getAsistencia()) << "%)" << std::endl;
    std::cout << "8. Cancelar" << std::endl;

    int campo = validarEntradaEntera("Campo a editar: ", 1, 8);

    switch (campo) {
        case 1: {
            std::string nuevoNombre;
            std::cout << "Nuevo nombre: ";
            std::cin.ignore();
            std::getline(std::cin, nuevoNombre);
            nuevoNombre = recortarEspacios(nuevoNombre);
            if (validarNombre(nuevoNombre)) {
                registrarLog("Nombre cambiado: " + est[num].getNombre() + " -> " + nuevoNombre);
                est[num].setNombre(nuevoNombre);
                std::cout << "Nombre actualizado." << std::endl;
            } else {
                std::cout << "Nombre invalido." << std::endl;
            }
            break;
        }
        case 2: {
            std::string nuevaCarrera;
            std::cout << "Carreras: ";
            for (int i = 0; i < totalCarreras; i++) std::cout << carrerasValidas[i] << " ";
            std::cout << "\nNueva carrera: ";
            std::cin.ignore();
            std::getline(std::cin, nuevaCarrera);
            nuevaCarrera = recortarEspacios(nuevaCarrera);
            if (validarCarrera(nuevaCarrera)) {
                for (int i = 0; i < totalCarreras; i++) {
                    if (compararIgnorandoCase(nuevaCarrera, carrerasValidas[i])) {
                        nuevaCarrera = carrerasValidas[i];
                        break;
                    }
                }
                registrarLog("Carrera cambiada para " + est[num].getNombre());
                est[num].setCarrera(nuevaCarrera);
                std::cout << "Carrera actualizada." << std::endl;
            } else {
                std::cout << "Carrera no valida." << std::endl;
            }
            break;
        }
        case 3:
            est[num].setSemestre(validarEntradaEntera("Nuevo semestre (1-12): ", 1, 12));
            std::cout << "Semestre actualizado." << std::endl;
            break;
        case 4:
            est[num].setNota(validarEntradaDecimal("Nueva nota P1 (0-100): ", 0, 100), 0, 0);
            std::cout << "Nota P1 actualizada." << std::endl;
            registrarLog("Nota P1 actualizada para " + est[num].getNombre());
            break;
        case 5:
            est[num].setNota(0, validarEntradaDecimal("Nueva nota P2 (0-100): ", 0, 100), 0);
            std::cout << "Nota P2 actualizada." << std::endl;
            registrarLog("Nota P2 actualizada para " + est[num].getNombre());
            break;
        case 6:
            est[num].setNota(0, 0, validarEntradaDecimal("Nueva nota P3 (0-100): ", 0, 100));
            std::cout << "Nota P3 actualizada." << std::endl;
            registrarLog("Nota P3 actualizada para " + est[num].getNombre());
            break;
        case 7:
            est[num].setAsistencia(validarEntradaDecimal("Nueva asistencia (0-100%): ", 0, 100));
            std::cout << "Asistencia actualizada." << std::endl;
            break;
        case 8:
            std::cout << "Edicion cancelada." << std::endl;
            return;
    }
}
void Colegio:: reportePorCarrera(Estudiantes est[]) {
    if (cantAlumnos == 0) {
        std::cout << "No hay estudiantes." << std::endl;
        return;
    }

    std::cout << "\n" << lineaSeparadora(65, '=') << std::endl;
    std::cout << "         REPORTE POR CARRERA" << std::endl;
    std::cout << lineaSeparadora(65, '=') << std::endl;

    for (int c = 0; c < totalCarreras; c++) {
        int cantEnCarrera = 0;
        double sumaNotas = 0;
        double sumaAsist = 0;
        int aprobCarrera = 0;

        for (int i = 0; i < cantAlumnos; i++) {
            if (!est[i].getInscrito()) continue;
            if (compararIgnorandoCase(est[i].getCarrera(), carrerasValidas[c])) {
                cantEnCarrera++;
            }
        }

        if (cantEnCarrera == 0) continue;

        std::cout << "\n>> " << aMayusculas(carrerasValidas[c]) << " (" << cantEnCarrera << " estudiantes)" << std::endl;
        std::cout << lineaSeparadora(60, '-') << std::endl;

        for (int i = 0; i < cantAlumnos; i++) {
            if (!est[i].getInscrito()) continue;
            if (compararIgnorandoCase(est[i].getCarrera(), carrerasValidas[c])) {
                double f = est[i].calcularFinal(*this);
                sumaNotas += f;
                sumaAsist += est[i].getAsistencia();
                if (f >= notaMinima) aprobCarrera++;
                std::cout << "  " << rellenarDerecha(est[i].getNombre(), 25)
                     << " | Sem " << est[i].getSemestre()
                     << " | Final: " << rellenarDerecha(formatoNota(f), 7)
                     << " (" << obtenerLetra(f) << ")"
                     << " | Asist: " << formatoNota(est[i].getAsistencia()) << "%" << std::endl;
            }
        }

        std::cout << "  " << lineaSeparadora(55, '.') << std::endl;
        std::cout << "  Promedio: " << formatoNota(sumaNotas / cantEnCarrera)
             << " | Asist. promedio: " << formatoNota(sumaAsist / cantEnCarrera) << "%"
             << " | Aprobacion: " << formatoNota(aprobCarrera * 100.0 / cantEnCarrera) << "%" << std::endl;
    }

    std::cout << "\n" << lineaSeparadora(65, '=') << std::endl;
    registrarLog("Reporte por carrera generado");
}

void Colegio::exportarNotas(Estudiantes est[80]){
    if (cantAlumnos == 0) {
        std::cout << "No hay datos para exportar." << std::endl;
        return;
    }

    std::string nombreArchivo = "notas_" + obtenerFechaActual() + ".txt"; 
    for (int i = 0; i < (int)nombreArchivo.length(); i++) {
        if (nombreArchivo[i] == '/') nombreArchivo[i] = '-';
    }

    std::ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        std::cout << "Error: No se pudo crear el archivo." << std::endl;
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
        if (!est[i].getInscrito()) continue;
        double f = est[i].calcularFinal(*this);
        archivo << rellenarDerecha(est[i].getNombre(), 25)
                << rellenarDerecha(est[i].getMatricula(), 12)
                << rellenarDerecha(est[i].getCarrera(), 16)
                << rellenarDerecha(formatoNota(est[i].getNota1()), 7)
                << rellenarDerecha(formatoNota(est[i].getNota2()), 7)
                << rellenarDerecha(formatoNota(est[i].getNota3()), 7)
                << rellenarDerecha(formatoNota(f), 8)
                << rellenarDerecha(obtenerEstado(f), 12) << std::endl;
    }

    archivo << lineaSeparadora(90, '=') << std::endl;
    archivo << "Aprobados: " << aprobados << " | Reprobados: " << reprobados << std::endl;
    archivo.close();

    std::cout << "Notas exportadas a: " << nombreArchivo << std::endl;
    registrarLog("Notas exportadas a: " + nombreArchivo);
}
