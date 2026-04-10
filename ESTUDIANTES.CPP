#include "ESTUDIANTES.hpp"
#include "UTILIDADES.hpp"
#include "VALIDACIONES.hpp"
void Estudiantes::setNombre(std::string nombre){
    alumno=nombre;
}
void Estudiantes::setMatricula(std::string mat){
    matricula=mat;
}
void Estudiantes::setCarrera(std::string carr){
    carrera=carr;
}
void Estudiantes::setSemestre(int sem){
    semestre=sem;
}
void Estudiantes::setNota(double n1,double n2,double n3){
        nota1=n1;
        nota2=n2;
        nota3=n3;
}
void Estudiantes::setAsistencia(double asist){
    asistencia=asist;
}
void Estudiantes::setInscrito(bool estado){
    inscrito=estado;
}
double Estudiantes::calcularFinal(Colegio uru) {
    double notaFinal = nota1 * uru.getPesoP1() + nota2 * uru.getPesoP2() + nota3 * uru.getPesoP3();
    notaFinal = redondear(notaFinal, 2);
    return notaFinal;
}
std::string Estudiantes::getNombre(){
    return alumno;
}
std::string Estudiantes::getMatricula(){
    return matricula;
}
std::string Estudiantes::getCarrera(){
    return carrera;
}
int Estudiantes::getSemestre(){
    return semestre;
}
double Estudiantes::getNota1(){
    return nota1;
}
double Estudiantes::getNota2(){
    return nota2;
}
double Estudiantes::getNota3(){
    return nota3;
}
double Estudiantes::getAsistencia(){
    return asistencia;
}
bool Estudiantes::getInscrito(){
    return inscrito;
}
double Estudiantes::getNotaFinal(){
    return notaFinal;
}
bool Estudiantes::estaEnRiesgo(Colegio uru) {
    double final_ = calcularFinal(uru); 
    // en riesgo si: nota entre 55-65, o asistencia baja, o nota < notaMinima con asistencia baja
    if (final_ >= 55 && final_ <= 65) return true;
    if (asistencia< uru.getAsistenciaMinima() && final_ < 70) return true;
    if (final_ < uru.getNotaMinima()) return true;
    return false;
}
void Estudiantes::agregarEstudiante(Colegio uru) {
    if (uru.getCantAlumnos() >= 80) {
        std::cout << "ERROR: Capacidad maxima alcanzada (80 estudiantes)." << std::endl;
        uru.registrarLog("ERROR: Intento de registro con capacidad maxima");
        return;
    }

    std::cout << "\n--- REGISTRO DE NUEVO ESTUDIANTE ---" << std::endl;

    // nombre
    std::string nombre;
    bool nombreValido = false;
    do {
        std::cout << "Nombre completo: ";
        std::cin.ignore();
        std::getline(std::cin, nombre);
        nombre = recortarEspacios(nombre);
        if (!validarNombre(nombre)) {
            std::cout << "Error: Nombre invalido (3-60 caracteres, solo letras y espacios)." << std::endl;
        } else {
            nombreValido = true;
        }
    } while (!nombreValido);

    // matricula
    std::string mat;
    bool matValida = false;
    do {
        std::cout << "Matricula (formato AAAA-NNNN): ";
        std::getline(std::cin, mat);
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
    for (int i = 0; i < uru.getcantidadcarreras(); i++) {
        std::cout << uru.carrerasValidas[i];
        if (i < uru.getcantidadcarreras() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    do {
        std::cout << "Carrera: ";
        std::getline(std::cin, carr);
        carr = recortarEspacios(carr);
        if (!validarCarrera(carr)) {
            std::cout << "Error: Carrera no valida." << std::endl;
        } else {
            for (int i = 0; i < uru.getcantidadcarreras(); i++) {
                if (compararIgnorandoCase(carr, uru.carrerasValidas[i])) {
                    carr = uru.carrerasValidas[i];
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
    setNombre(nombre);
    setMatricula(mat);
    setCarrera(carr);
    setSemestre(sem);
    setNota(n1, n2, n3);
    setAsistencia(asist);
    setInscrito(true);
    notaFinal = calcularFinal(uru);
    uru.setCantAlumnos(uru.getCantAlumnos() + 1);

    std::cout << "\nEstudiante '" << nombre << "' registrado con matricula " << mat << "." << std::endl;
    uru.registrarLog("Estudiante registrado: " + nombre + " (" + mat + ", " + carr + ", Sem " + std::to_string(sem) + ")");
}
void Estudiantes::calcularNotaIndividual(Colegio uru) {
    if (uru.getCantAlumnos() == 0) {
        std::cout << "No hay estudiantes." << std::endl;
        return;
    }
    int num = validarEntradaEntera("Numero de estudiante (1-" + std::to_string(uru.getCantAlumnos()) + "): ", 1, uru.getCantAlumnos());
    num--;
    
    if (!getInscrito()) {
        std::cout << "ADVERTENCIA: Este estudiante esta RETIRADO." << std::endl;
        std::cout << "Desea continuar? (s/n): ";
        char resp;
        std::cin >> resp;
        if (resp != 's' && resp != 'S') return;
    }

    double final_ = calcularFinal(uru);
    std::string letra = uru.obtenerLetra(final_);
    std::string estado = uru.obtenerEstado(final_);
    std::string estadoAsist = uru.obtenerEstadoAsistencia(asistencia);

    std::cout << "\n" << lineaSeparadora(55, '=') << std::endl;
    std::cout << "         BOLETA DE CALIFICACIONES" << std::endl;
    std::cout << "         " << obtenerFechaActual() << std::endl;
    std::cout << lineaSeparadora(55, '=') << std::endl;
    std::cout << "Estudiante:  " << getNombre() << std::endl;
    std::cout << "Matricula:   " << getMatricula() << std::endl;
    std::cout << "Carrera:     " << getCarrera() << std::endl;
    std::cout << "Semestre:    " << getSemestre() << std::endl;
    std::cout << lineaSeparadora(55, '-') << std::endl;
    std::cout << "CALIFICACIONES:" << std::endl;
    std::cout << "  Parcial 1 (" << uru.getPesoP1() * 100 << "%): " << formatoNota(nota1)
         << "  ->  " << formatoNota(nota1 * uru.getPesoP1()) << " pts" << std::endl;
    std::cout << "  Parcial 2 (" << uru.getPesoP2() * 100 << "%): " << formatoNota(nota2)
         << "  ->  " << formatoNota(nota2 * uru.getPesoP2()) << " pts" << std::endl;
    std::cout << "  Parcial 3 (" << uru.getPesoP3() * 100 << "%): " << formatoNota(nota3)
         << "  ->  " << formatoNota(nota3 * uru.getPesoP3()) << " pts" << std::endl;
    std::cout << lineaSeparadora(55, '-') << std::endl;
    std::cout << "  NOTA FINAL: " << formatoNota(final_) << " (" << letra << ") - " << estado << std::endl;
    std::cout << lineaSeparadora(55, '-') << std::endl;
    std::cout << "ASISTENCIA:" << std::endl;
    std::cout << "  " << barraProgreso(asistencia) << std::endl;
    std::cout << "  Estado: " << estadoAsist << " (minimo: " << uru.getAsistenciaMinima() << "%)" << std::endl;

    if (asistencia < uru.getAsistenciaMinima() && final_ >= uru.getNotaMinima()) {
         std::cout << "\n  *** ADVERTENCIA: Aprobado por nota pero NO cumple asistencia ***" << std::endl;
        std::cout << "\n  *** ADVERTENCIA: Aprobado por nota pero NO cumple asistencia ***" << std::endl;
    }

    if (estaEnRiesgo(uru)) {
        std::cout << "\n  >>> ALERTA: ESTUDIANTE EN RIESGO ACADEMICO <<<" << std::endl;
    }

    std::cout << lineaSeparadora(55, '=') << std::endl;

    // guardar en historial
    if (uru.totalHistorial < 300) {
        uru.historialAlumno[uru.totalHistorial] = getNombre();
        uru.historialFecha[uru.totalHistorial] = obtenerFechaActual();
        uru.historialNota[uru.totalHistorial] = final_;
        uru.totalHistorial++;
    }
    uru.registrarLog("Boleta generada para: " + getNombre() + " - Final: " + formatoNota(final_) + " (" + letra + ")");
}