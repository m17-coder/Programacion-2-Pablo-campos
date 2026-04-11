#include "ESTUDIANTES.hpp"
#include "COLEGIO.hpp"
#include "UTILIDADES.hpp"
#include "VALIDACIONES.hpp"

extern Colegio colegio;
extern Estudiantes est[80];

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
double Estudiantes::calcularFinal(Colegio& uru) {
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
bool Estudiantes::estaEnRiesgo(Colegio& uru) {
    double final_ = calcularFinal(uru); 
    // en riesgo si: nota entre 55-65, o asistencia baja, o nota < notaMinima con asistencia baja
    if (final_ >= 55 && final_ <= 65) return true;
    if (asistencia< uru.getAsistenciaMinima() && final_ < 70) return true;
    if (final_ < uru.getNotaMinima()) return true;
    return false;
}
void Estudiantes::agregarEstudiante(Colegio& uru) {
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
void Estudiantes::calcularNotaIndividual(Colegio& uru) {
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

