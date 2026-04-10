// SISTEMA DE NOTAS - version "final" (eso dijeron la ultima vez)
// hecho por: el pasante que ya se fue
// modificado por: la profesora Martinez (que tambien se fue)
// funciona, no tocar... o si?

#include <iostream>
#include <string>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>

using namespace std;

// ============ VARIABLES GLOBALES ============
string alumno[80];
string matricula[80];
string carrera[80];
int semestre[80];
double nota1[80], nota2[80], nota3[80], notaFinal[80];
double asistencia[80]; // porcentaje de asistencia 0-100
bool inscrito[80]; // si el alumno esta activo o se retiro
int cantAlumnos = 0;

// configuracion academica
double pesoP1 = 0.30, pesoP2 = 0.30, pesoP3 = 0.40;
double notaMinima = 60.0;
double notaMinimaHonor = 90.0;
double asistenciaMinima = 75.0; // porcentaje minimo
int aprobados = 0, reprobados = 0;

// carreras validas
string carrerasValidas[] = {"Ingenieria", "Medicina", "Derecho", "Administracion", "Psicologia", "Arquitectura"};
int totalCarreras = 6;

// log del sistema
string logEventos[500];
int totalLog = 0;

// historial de consultas de boletas
string historialAlumno[300];
string historialFecha[300];
double historialNota[300];
int totalHistorial = 0;

// ============ FUNCIONES DE UTILIDAD ============

string obtenerFechaActual() {
    time_t ahora = time(0);
    tm *ltm = localtime(&ahora);
    string fecha = "";
    int dia = ltm->tm_mday;
    int mes = 1 + ltm->tm_mon;
    int anio = 1900 + ltm->tm_year;
    if (dia < 10) fecha += "0";
    fecha += to_string(dia) + "/";
    if (mes < 10) fecha += "0";
    fecha += to_string(mes) + "/";
    fecha += to_string(anio);
    return fecha;
}

string obtenerHoraActual() {
    time_t ahora = time(0);
    tm *ltm = localtime(&ahora);
    string hora = "";
    if (ltm->tm_hour < 10) hora += "0";
    hora += to_string(ltm->tm_hour) + ":";
    if (ltm->tm_min < 10) hora += "0";
    hora += to_string(ltm->tm_min) + ":";
    if (ltm->tm_sec < 10) hora += "0";
    hora += to_string(ltm->tm_sec);
    return hora;
}

void registrarLog(string evento) {
    if (totalLog >= 500) {
        for (int i = 0; i < 499; i++) {
            logEventos[i] = logEventos[i + 1];
        }
        totalLog = 499;
    }
    logEventos[totalLog] = "[" + obtenerFechaActual() + " " + obtenerHoraActual() + "] " + evento;
    totalLog++;
}

string aMayusculas(string texto) {
    string resultado = "";
    for (int i = 0; i < (int)texto.length(); i++) {
        char c = texto[i];
        if (c >= 'a' && c <= 'z') {
            resultado += (char)(c - 32);
        } else {
            resultado += c;
        }
    }
    return resultado;
}

string aMinusculas(string texto) {
    string resultado = "";
    for (int i = 0; i < (int)texto.length(); i++) {
        char c = texto[i];
        if (c >= 'A' && c <= 'Z') {
            resultado += (char)(c + 32);
        } else {
            resultado += c;
        }
    }
    return resultado;
}

string recortarEspacios(string texto) {
    int inicio = 0, fin = texto.length() - 1;
    while (inicio <= fin && texto[inicio] == ' ') inicio++;
    while (fin >= inicio && texto[fin] == ' ') fin--;
    string resultado = "";
    for (int i = inicio; i <= fin; i++) {
        resultado += texto[i];
    }
    return resultado;
}

string rellenarDerecha(string texto, int largo, char relleno = ' ') {
    string resultado = texto;
    while ((int)resultado.length() < largo) {
        resultado += relleno;
    }
    return resultado;
}

string rellenarIzquierda(string texto, int largo, char relleno = ' ') {
    string resultado = texto;
    while ((int)resultado.length() < largo) {
        resultado = relleno + resultado;
    }
    return resultado;
}

double redondear(double valor, int decimales) {
    double factor = pow(10, decimales);
    return round(valor * factor) / factor;
}

string lineaSeparadora(int largo, char caracter = '-') {
    string linea = "";
    for (int i = 0; i < largo; i++) linea += caracter;
    return linea;
}

bool compararIgnorandoCase(string a, string b) {
    if (a.length() != b.length()) return false;
    for (int i = 0; i < (int)a.length(); i++) {
        char ca = a[i], cb = b[i];
        if (ca >= 'A' && ca <= 'Z') ca += 32;
        if (cb >= 'A' && cb <= 'Z') cb += 32;
        if (ca != cb) return false;
    }
    return true;
}

bool contieneTexto(string texto, string busqueda) {
    string textoMin = aMinusculas(texto);
    string busqMin = aMinusculas(busqueda);
    for (int i = 0; i <= (int)textoMin.length() - (int)busqMin.length(); i++) {
        bool encontrado = true;
        for (int j = 0; j < (int)busqMin.length(); j++) {
            if (textoMin[i + j] != busqMin[j]) {
                encontrado = false;
                break;
            }
        }
        if (encontrado) return true;
    }
    return false;
}

// formatea un numero con decimales fijos
string formatoNota(double nota) {
    string resultado = to_string(redondear(nota, 2));
    // recortar a 2 decimales
    int puntoPos = -1;
    for (int i = 0; i < (int)resultado.length(); i++) {
        if (resultado[i] == '.') { puntoPos = i; break; }
    }
    if (puntoPos >= 0 && (int)resultado.length() > puntoPos + 3) {
        resultado = resultado.substr(0, puntoPos + 3);
    }
    return resultado;
}

// genera una barra visual de progreso con caracteres
string barraProgreso(double porcentaje, int largo = 20) {
    int llenos = (int)(porcentaje / 100.0 * largo);
    if (llenos > largo) llenos = largo;
    if (llenos < 0) llenos = 0;
    string barra = "[";
    for (int i = 0; i < largo; i++) {
        if (i < llenos) barra += "#";
        else barra += ".";
    }
    barra += "] " + formatoNota(porcentaje) + "%";
    return barra;
}

// ============ FUNCIONES DE VALIDACION ============

bool validarMatricula(string mat) {
    // formato: AAAA-NNNN (4 letras, guion, 4 numeros)
    if (mat.length() != 9) return false;
    if (mat[4] != '-') return false;
    for (int i = 0; i < 4; i++) {
        if (!((mat[i] >= 'A' && mat[i] <= 'Z') || (mat[i] >= 'a' && mat[i] <= 'z'))) return false;
    }
    for (int i = 5; i < 9; i++) {
        if (mat[i] < '0' || mat[i] > '9') return false;
    }
    return true;
}

bool validarMatriculaUnica(string mat, int excluirIdx = -1) {
    string matUpper = aMayusculas(mat);
    for (int i = 0; i < cantAlumnos; i++) {
        if (i == excluirIdx) continue;
        if (aMayusculas(matricula[i]) == matUpper) return false;
    }
    return true;
}

bool validarNota(double nota) {
    if (nota < 0.0) return false;
    if (nota > 100.0) return false;
    return true;
}

bool validarNombre(string nombre) {
    if (nombre.length() < 3) return false;
    if (nombre.length() > 60) return false;
    for (int i = 0; i < (int)nombre.length(); i++) {
        char c = nombre[i];
        bool valido = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == ' '
                    || c == '\'' || c == '-';
        if ((unsigned char)c > 127) valido = true;
        if (!valido) return false;
    }
    return true;
}

bool validarCarrera(string carr) {
    for (int i = 0; i < totalCarreras; i++) {
        if (compararIgnorandoCase(carr, carrerasValidas[i])) return true;
    }
    return false;
}

bool validarSemestre(int sem) {
    return sem >= 1 && sem <= 12;
}

bool validarAsistencia(double pct) {
    return pct >= 0.0 && pct <= 100.0;
}

int validarEntradaEntera(string mensaje, int minimo, int maximo) {
    int valor;
    bool valido = false;
    do {
        cout << mensaje;
        cin >> valor;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Error: Ingrese un numero valido." << endl;
        } else if (valor < minimo || valor > maximo) {
            cout << "Error: El valor debe estar entre " << minimo << " y " << maximo << "." << endl;
        } else {
            valido = true;
        }
    } while (!valido);
    return valor;
}

double validarEntradaDecimal(string mensaje, double minimo, double maximo) {
    double valor;
    bool valido = false;
    do {
        cout << mensaje;
        cin >> valor;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Error: Ingrese un numero valido." << endl;
        } else if (valor < minimo || valor > maximo) {
            cout << "Error: El valor debe estar entre " << minimo << " y " << maximo << "." << endl;
        } else {
            valido = true;
        }
    } while (!valido);
    return valor;
}

// ============ MENU ============

void menu() {
    cout << endl;
    cout << lineaSeparadora(55, '*') << endl;
    cout << "*   SISTEMA DE GESTION DE NOTAS                     *" << endl;
    cout << "*   Instituto Tecnologico ABC                        *" << endl;
    cout << "*   " << obtenerFechaActual() << " | " << obtenerHoraActual() << "                           *" << endl;
    cout << lineaSeparadora(55, '*') << endl;
    cout << "  1. Agregar estudiante" << endl;
    cout << "  2. Ver todos los estudiantes" << endl;
    cout << "  3. Calcular nota final de un estudiante" << endl;
    cout << "  4. Calcular notas de todos" << endl;
    cout << "  5. Buscar estudiante" << endl;
    cout << "  6. Estadisticas del curso" << endl;
    cout << "  7. Modificar pesos de evaluacion" << endl;
    cout << "  8. Tabla de honor" << endl;
    cout << "  9. Editar estudiante" << endl;
    cout << " 10. Retirar/Reinscribir estudiante" << endl;
    cout << " 11. Reporte por carrera" << endl;
    cout << " 12. Exportar notas a archivo" << endl;
    cout << " 13. Estudiantes en riesgo academico" << endl;
    cout << " 14. Ver historial de boletas" << endl;
    cout << " 15. Ver log del sistema" << endl;
    cout << "  0. Salir" << endl;
    cout << lineaSeparadora(55, '-') << endl;
    int activos = 0;
    for (int i = 0; i < cantAlumnos; i++) {
        if (inscrito[i]) activos++;
    }
    cout << "  Estudiantes inscritos: " << activos << "/" << cantAlumnos << endl;
    cout << "Seleccione: ";
}

// ============ FUNCIONES PRINCIPALES ============

void agregarEstudiante() {
    if (cantAlumnos >= 80) {
        cout << "ERROR: Capacidad maxima alcanzada (80 estudiantes)." << endl;
        registrarLog("ERROR: Intento de registro con capacidad maxima");
        return;
    }

    cout << "\n--- REGISTRO DE NUEVO ESTUDIANTE ---" << endl;

    // nombre
    string nombre;
    bool nombreValido = false;
    do {
        cout << "Nombre completo: ";
        cin.ignore();
        getline(cin, nombre);
        nombre = recortarEspacios(nombre);
        if (!validarNombre(nombre)) {
            cout << "Error: Nombre invalido (3-60 caracteres, solo letras y espacios)." << endl;
        } else {
            nombreValido = true;
        }
    } while (!nombreValido);

    // matricula
    string mat;
    bool matValida = false;
    do {
        cout << "Matricula (formato AAAA-NNNN): ";
        getline(cin, mat);
        mat = recortarEspacios(mat);
        if (!validarMatricula(mat)) {
            cout << "Error: Formato invalido. Use 4 letras, guion, 4 numeros." << endl;
        } else if (!validarMatriculaUnica(mat)) {
            cout << "Error: Ya existe un estudiante con esa matricula." << endl;
        } else {
            mat = aMayusculas(mat);
            matValida = true;
        }
    } while (!matValida);

    // carrera
    string carr;
    bool carrValida = false;
    cout << "Carreras disponibles: ";
    for (int i = 0; i < totalCarreras; i++) {
        cout << carrerasValidas[i];
        if (i < totalCarreras - 1) cout << ", ";
    }
    cout << endl;
    do {
        cout << "Carrera: ";
        getline(cin, carr);
        carr = recortarEspacios(carr);
        if (!validarCarrera(carr)) {
            cout << "Error: Carrera no valida." << endl;
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

    cout << "\nEstudiante '" << nombre << "' registrado con matricula " << mat << "." << endl;
    registrarLog("Estudiante registrado: " + nombre + " (" + mat + ", " + carr + ", Sem " + to_string(sem) + ")");
}

void verEstudiantes() {
    if (cantAlumnos == 0) {
        cout << "No hay estudiantes registrados." << endl;
        return;
    }

    cout << "\n" << lineaSeparadora(120, '=') << endl;
    cout << rellenarDerecha("ID", 5) << rellenarDerecha("NOMBRE", 25) << rellenarDerecha("MATRICULA", 12)
         << rellenarDerecha("CARRERA", 16) << rellenarDerecha("SEM", 5) << rellenarDerecha("P1", 7)
         << rellenarDerecha("P2", 7) << rellenarDerecha("P3", 7) << rellenarDerecha("FINAL", 8)
         << rellenarDerecha("ASIST", 8) << rellenarDerecha("ESTADO", 12) << endl;
    cout << lineaSeparadora(120, '-') << endl;

    for (int i = 0; i < cantAlumnos; i++) {
        string estado = inscrito[i] ? "Inscrito" : "Retirado";
        cout << rellenarDerecha(to_string(i + 1), 5)
             << rellenarDerecha(alumno[i], 25)
             << rellenarDerecha(matricula[i], 12)
             << rellenarDerecha(carrera[i], 16)
             << rellenarDerecha(to_string(semestre[i]), 5)
             << rellenarDerecha(formatoNota(nota1[i]), 7)
             << rellenarDerecha(formatoNota(nota2[i]), 7)
             << rellenarDerecha(formatoNota(nota3[i]), 7)
             << rellenarDerecha(formatoNota(notaFinal[i]), 8)
             << rellenarDerecha(formatoNota(asistencia[i]) + "%", 8)
             << rellenarDerecha(estado, 12) << endl;
    }
    cout << lineaSeparadora(120, '=') << endl;
    cout << "Total: " << cantAlumnos << " estudiantes" << endl;
    registrarLog("Consulta de lista de estudiantes");
}

// ============ CALCULOS ACADEMICOS ============

double calcularFinal(int idx) {
    double final_ = nota1[idx] * pesoP1 + nota2[idx] * pesoP2 + nota3[idx] * pesoP3;
    final_ = redondear(final_, 2);
    notaFinal[idx] = final_;
    return final_;
}

string obtenerLetra(double nota) {
    if (nota >= 90) return "A";
    if (nota >= 80) return "B";
    if (nota >= 70) return "C";
    if (nota >= 60) return "D";
    return "F";
}

string obtenerEstado(double nota) {
    if (nota >= notaMinima) return "APROBADO";
    return "REPROBADO";
}

string obtenerEstadoAsistencia(double pctAsist) {
    if (pctAsist >= asistenciaMinima) return "Cumple";
    return "NO CUMPLE";
}

// determina si un estudiante esta en riesgo academico
bool estaEnRiesgo(int idx) {
    double final_ = calcularFinal(idx);
    // en riesgo si: nota entre 55-65, o asistencia baja, o nota < notaMinima con asistencia baja
    if (final_ >= 55 && final_ <= 65) return true;
    if (asistencia[idx] < asistenciaMinima && final_ < 70) return true;
    if (final_ < notaMinima) return true;
    return false;
}

void calcularNotaIndividual() {
    if (cantAlumnos == 0) {
        cout << "No hay estudiantes." << endl;
        return;
    }
    int num = validarEntradaEntera("Numero de estudiante (1-" + to_string(cantAlumnos) + "): ", 1, cantAlumnos);
    num--;
    
    if (!inscrito[num]) {
        cout << "ADVERTENCIA: Este estudiante esta RETIRADO." << endl;
        cout << "Desea continuar? (s/n): ";
        char resp;
        cin >> resp;
        if (resp != 's' && resp != 'S') return;
    }

    double final_ = calcularFinal(num);
    string letra = obtenerLetra(final_);
    string estado = obtenerEstado(final_);
    string estadoAsist = obtenerEstadoAsistencia(asistencia[num]);

    cout << "\n" << lineaSeparadora(55, '=') << endl;
    cout << "         BOLETA DE CALIFICACIONES" << endl;
    cout << "         " << obtenerFechaActual() << endl;
    cout << lineaSeparadora(55, '=') << endl;
    cout << "Estudiante:  " << alumno[num] << endl;
    cout << "Matricula:   " << matricula[num] << endl;
    cout << "Carrera:     " << carrera[num] << endl;
    cout << "Semestre:    " << semestre[num] << endl;
    cout << lineaSeparadora(55, '-') << endl;
    cout << "CALIFICACIONES:" << endl;
    cout << "  Parcial 1 (" << pesoP1 * 100 << "%): " << formatoNota(nota1[num])
         << "  ->  " << formatoNota(nota1[num] * pesoP1) << " pts" << endl;
    cout << "  Parcial 2 (" << pesoP2 * 100 << "%): " << formatoNota(nota2[num])
         << "  ->  " << formatoNota(nota2[num] * pesoP2) << " pts" << endl;
    cout << "  Parcial 3 (" << pesoP3 * 100 << "%): " << formatoNota(nota3[num])
         << "  ->  " << formatoNota(nota3[num] * pesoP3) << " pts" << endl;
    cout << lineaSeparadora(55, '-') << endl;
    cout << "  NOTA FINAL: " << formatoNota(final_) << " (" << letra << ") - " << estado << endl;
    cout << lineaSeparadora(55, '-') << endl;
    cout << "ASISTENCIA:" << endl;
    cout << "  " << barraProgreso(asistencia[num]) << endl;
    cout << "  Estado: " << estadoAsist << " (minimo: " << asistenciaMinima << "%)" << endl;

    if (asistencia[num] < asistenciaMinima && final_ >= notaMinima) {
        cout << "\n  *** ADVERTENCIA: Aprobado por nota pero NO cumple asistencia ***" << endl;
    }

    if (estaEnRiesgo(num)) {
        cout << "\n  >>> ALERTA: ESTUDIANTE EN RIESGO ACADEMICO <<<" << endl;
    }

    cout << lineaSeparadora(55, '=') << endl;

    // guardar en historial
    if (totalHistorial < 300) {
        historialAlumno[totalHistorial] = alumno[num];
        historialFecha[totalHistorial] = obtenerFechaActual();
        historialNota[totalHistorial] = final_;
        totalHistorial++;
    }
    registrarLog("Boleta generada para: " + alumno[num] + " - Final: " + formatoNota(final_) + " (" + letra + ")");
}

void calcularTodas() {
    if (cantAlumnos == 0) {
        cout << "No hay estudiantes." << endl;
        return;
    }
    aprobados = 0;
    reprobados = 0;

    cout << "\n" << lineaSeparadora(80, '=') << endl;
    cout << "                NOTAS FINALES - " << obtenerFechaActual() << endl;
    cout << lineaSeparadora(80, '=') << endl;
    cout << rellenarDerecha("NOMBRE", 25) << rellenarDerecha("CARRERA", 16)
         << rellenarDerecha("FINAL", 8) << rellenarDerecha("LETRA", 7)
         << rellenarDerecha("ESTADO", 12) << rellenarDerecha("ASIST", 10) << endl;
    cout << lineaSeparadora(80, '-') << endl;

    for (int i = 0; i < cantAlumnos; i++) {
        if (!inscrito[i]) continue;
        double f = calcularFinal(i);
        string estado = obtenerEstado(f);
        string letra = obtenerLetra(f);
        string estadoAsist = obtenerEstadoAsistencia(asistencia[i]);

        cout << rellenarDerecha(alumno[i], 25)
             << rellenarDerecha(carrera[i], 16)
             << rellenarDerecha(formatoNota(f), 8)
             << rellenarDerecha(letra, 7)
             << rellenarDerecha(estado, 12)
             << rellenarDerecha(estadoAsist, 10) << endl;

        if (f >= notaMinima) aprobados++;
        else reprobados++;
    }

    cout << lineaSeparadora(80, '-') << endl;
    cout << "Aprobados: " << aprobados << " | Reprobados: " << reprobados << endl;
    cout << lineaSeparadora(80, '=') << endl;
    registrarLog("Calculo masivo de notas realizado");
}

void buscarEstudiante() {
    if (cantAlumnos == 0) {
        cout << "No hay estudiantes." << endl;
        return;
    }
    cout << "Buscar por: 1) Nombre  2) Matricula  3) Carrera" << endl;
    int tipo = validarEntradaEntera("Opcion: ", 1, 3);

    string busq;
    cout << "Texto a buscar: ";
    cin.ignore();
    getline(cin, busq);
    busq = recortarEspacios(busq);

    bool found = false;
    int resultados = 0;

    cout << "\n--- RESULTADOS ---" << endl;
    for (int i = 0; i < cantAlumnos; i++) {
        bool match = false;
        switch (tipo) {
            case 1: match = contieneTexto(alumno[i], busq); break;
            case 2: match = contieneTexto(matricula[i], busq); break;
            case 3: match = contieneTexto(carrera[i], busq); break;
        }
        if (match) {
            string estado = inscrito[i] ? "Inscrito" : "Retirado";
            cout << "#" << i + 1 << " | " << alumno[i] << " | " << matricula[i]
                 << " | " << carrera[i] << " | Sem " << semestre[i]
                 << " | Final: " << formatoNota(notaFinal[i])
                 << " | " << estado << endl;
            found = true;
            resultados++;
        }
    }
    if (!found) {
        cout << "No se encontraron resultados." << endl;
    } else {
        cout << "Total encontrados: " << resultados << endl;
    }
    registrarLog("Busqueda: '" + busq + "' - " + to_string(resultados) + " resultados");
}

void estadisticas() {
    if (cantAlumnos == 0) {
        cout << "No hay datos." << endl;
        return;
    }

    double suma = 0, maxNota = 0, minNota = 100;
    string mejorAlumno = "", peorAlumno = "";
    aprobados = 0; reprobados = 0;
    int cantActivos = 0;
    double sumaAsistencia = 0;
    int bajosAsistencia = 0;

    for (int i = 0; i < cantAlumnos; i++) {
        if (!inscrito[i]) continue;
        cantActivos++;
        double f = calcularFinal(i);
        suma += f;
        sumaAsistencia += asistencia[i];
        if (asistencia[i] < asistenciaMinima) bajosAsistencia++;
        if (f > maxNota) { maxNota = f; mejorAlumno = alumno[i]; }
        if (f < minNota) { minNota = f; peorAlumno = alumno[i]; }
        if (f >= notaMinima) aprobados++;
        else reprobados++;
    }

    if (cantActivos == 0) {
        cout << "No hay estudiantes inscritos." << endl;
        return;
    }

    double promedio = suma / cantActivos;
    double promedioAsist = sumaAsistencia / cantActivos;

    // desviacion estandar
    double sumaDif = 0;
    for (int i = 0; i < cantAlumnos; i++) {
        if (!inscrito[i]) continue;
        sumaDif += pow(notaFinal[i] - promedio, 2);
    }
    double desviacion = sqrt(sumaDif / cantActivos);

    // mediana
    double notasOrdenadas[80];
    int k = 0;
    for (int i = 0; i < cantAlumnos; i++) {
        if (!inscrito[i]) continue;
        notasOrdenadas[k++] = notaFinal[i];
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
        if (!inscrito[i]) continue;
        string letra = obtenerLetra(notaFinal[i]);
        if (letra == "A") contA++;
        else if (letra == "B") contB++;
        else if (letra == "C") contC++;
        else if (letra == "D") contD++;
        else contF++;
    }

    double pctAprobados = (aprobados * 100.0) / cantActivos;

    cout << "\n" << lineaSeparadora(60, '=') << endl;
    cout << "         ESTADISTICAS DEL CURSO" << endl;
    cout << "         " << obtenerFechaActual() << endl;
    cout << lineaSeparadora(60, '=') << endl;
    cout << "POBLACION:" << endl;
    cout << "  Total registrados:   " << cantAlumnos << endl;
    cout << "  Inscritos activos:   " << cantActivos << endl;
    cout << "  Retirados:           " << cantAlumnos - cantActivos << endl;
    cout << lineaSeparadora(60, '-') << endl;
    cout << "NOTAS:" << endl;
    cout << "  Promedio general:    " << formatoNota(promedio) << endl;
    cout << "  Mediana:             " << formatoNota(mediana) << endl;
    cout << "  Desviacion estandar: " << formatoNota(desviacion) << endl;
    cout << "  Nota mas alta:       " << mejorAlumno << " (" << formatoNota(maxNota) << ")" << endl;
    cout << "  Nota mas baja:       " << peorAlumno << " (" << formatoNota(minNota) << ")" << endl;
    cout << lineaSeparadora(60, '-') << endl;
    cout << "APROBACION:" << endl;
    cout << "  Aprobados:           " << aprobados << " (" << formatoNota(pctAprobados) << "%)" << endl;
    cout << "  Aprobacion: " << barraProgreso(pctAprobados, 30) << endl;
    cout << "  Reprobados:          " << reprobados << endl;
    cout << lineaSeparadora(60, '-') << endl;
    cout << "ASISTENCIA:" << endl;
    cout << "  Promedio asistencia: " << formatoNota(promedioAsist) << "%" << endl;
    cout << "  Debajo del minimo:   " << bajosAsistencia << " estudiantes" << endl;
    cout << lineaSeparadora(60, '-') << endl;
    cout << "DISTRIBUCION POR LETRA:" << endl;
    cout << "  A (90-100): " << contA << "  " << barraProgreso(contA * 100.0 / cantActivos, 15) << endl;
    cout << "  B (80-89):  " << contB << "  " << barraProgreso(contB * 100.0 / cantActivos, 15) << endl;
    cout << "  C (70-79):  " << contC << "  " << barraProgreso(contC * 100.0 / cantActivos, 15) << endl;
    cout << "  D (60-69):  " << contD << "  " << barraProgreso(contD * 100.0 / cantActivos, 15) << endl;
    cout << "  F (0-59):   " << contF << "  " << barraProgreso(contF * 100.0 / cantActivos, 15) << endl;
    cout << lineaSeparadora(60, '=') << endl;
    registrarLog("Estadisticas del curso generadas");
}

void modificarPesos() {
    cout << "\n--- CONFIGURACION ACADEMICA ---" << endl;
    cout << "Pesos actuales -> P1: " << pesoP1 * 100 << "%, P2: " << pesoP2 * 100
         << "%, P3: " << pesoP3 * 100 << "%" << endl;
    cout << "Nuevo peso P1 (ej: 0.25 para 25%): ";
    cin >> pesoP1;
    cout << "Nuevo peso P2: ";
    cin >> pesoP2;
    cout << "Nuevo peso P3: ";
    cin >> pesoP3;

    double total = pesoP1 + pesoP2 + pesoP3;
    if (abs(total - 1.0) > 0.01) {
        cout << "ADVERTENCIA: Los pesos suman " << total * 100 << "%, no 100%." << endl;
        cout << "Se normalizaran automaticamente." << endl;
        pesoP1 /= total;
        pesoP2 /= total;
        pesoP3 /= total;
    }
    cout << "Pesos actualizados: P1=" << pesoP1 * 100 << "%, P2=" << pesoP2 * 100
         << "%, P3=" << pesoP3 * 100 << "%" << endl;

    cout << "\nNota minima aprobatoria actual: " << notaMinima << endl;
    notaMinima = validarEntradaDecimal("Nueva nota minima: ", 0, 100);

    cout << "Nota minima para tabla de honor actual: " << notaMinimaHonor << endl;
    notaMinimaHonor = validarEntradaDecimal("Nueva nota minima honor: ", 0, 100);

    cout << "Asistencia minima actual: " << asistenciaMinima << "%" << endl;
    asistenciaMinima = validarEntradaDecimal("Nueva asistencia minima (%): ", 0, 100);

    cout << "Configuracion guardada." << endl;
    registrarLog("Configuracion academica modificada");
}

void tablaHonor() {
    if (cantAlumnos == 0) {
        cout << "No hay estudiantes." << endl;
        return;
    }

    // calcular todas primero
    for (int i = 0; i < cantAlumnos; i++) {
        if (inscrito[i]) calcularFinal(i);
    }

    // copiar activos con nota >= notaMinimaHonor y asistencia >= asistenciaMinima
    string nombresHonor[80];
    string matHonor[80];
    string carrHonor[80];
    double notasHonor[80];
    double asistHonor[80];
    int cantHonor = 0;

    for (int i = 0; i < cantAlumnos; i++) {
        if (!inscrito[i]) continue;
        if (notaFinal[i] >= notaMinimaHonor && asistencia[i] >= asistenciaMinima) {
            nombresHonor[cantHonor] = alumno[i];
            matHonor[cantHonor] = matricula[i];
            carrHonor[cantHonor] = carrera[i];
            notasHonor[cantHonor] = notaFinal[i];
            asistHonor[cantHonor] = asistencia[i];
            cantHonor++;
        }
    }

    if (cantHonor == 0) {
        cout << "No hay estudiantes que cumplan los requisitos para la tabla de honor." << endl;
        cout << "(Nota >= " << notaMinimaHonor << " y Asistencia >= " << asistenciaMinima << "%)" << endl;
        return;
    }

    // ordenar por nota (burbuja)
    for (int i = 0; i < cantHonor - 1; i++) {
        for (int j = 0; j < cantHonor - i - 1; j++) {
            if (notasHonor[j] < notasHonor[j + 1]) {
                double tempN = notasHonor[j]; notasHonor[j] = notasHonor[j + 1]; notasHonor[j + 1] = tempN;
                double tempA = asistHonor[j]; asistHonor[j] = asistHonor[j + 1]; asistHonor[j + 1] = tempA;
                string tempS = nombresHonor[j]; nombresHonor[j] = nombresHonor[j + 1]; nombresHonor[j + 1] = tempS;
                string tempM = matHonor[j]; matHonor[j] = matHonor[j + 1]; matHonor[j + 1] = tempM;
                string tempC = carrHonor[j]; carrHonor[j] = carrHonor[j + 1]; carrHonor[j + 1] = tempC;
            }
        }
    }

    cout << "\n" << lineaSeparadora(75, '=') << endl;
    cout << "         TABLA DE HONOR" << endl;
    cout << "         (Nota >= " << notaMinimaHonor << " | Asistencia >= " << asistenciaMinima << "%)" << endl;
    cout << lineaSeparadora(75, '=') << endl;
    cout << rellenarDerecha("POS", 5) << rellenarDerecha("NOMBRE", 25) << rellenarDerecha("MATRICULA", 12)
         << rellenarDerecha("CARRERA", 16) << rellenarDerecha("NOTA", 8) << rellenarDerecha("ASIST", 8) << endl;
    cout << lineaSeparadora(75, '-') << endl;

    for (int i = 0; i < cantHonor; i++) {
        string medalla = "";
        if (i == 0) medalla = " ***";
        else if (i == 1) medalla = " **";
        else if (i == 2) medalla = " *";

        cout << rellenarDerecha(to_string(i + 1), 5)
             << rellenarDerecha(nombresHonor[i] + medalla, 25)
             << rellenarDerecha(matHonor[i], 12)
             << rellenarDerecha(carrHonor[i], 16)
             << rellenarDerecha(formatoNota(notasHonor[i]), 8)
             << rellenarDerecha(formatoNota(asistHonor[i]) + "%", 8) << endl;
    }
    cout << lineaSeparadora(75, '=') << endl;
    registrarLog("Tabla de honor generada con " + to_string(cantHonor) + " estudiantes");
}

void editarEstudiante() {
    if (cantAlumnos == 0) {
        cout << "No hay estudiantes." << endl;
        return;
    }
    int num = validarEntradaEntera("Numero de estudiante (1-" + to_string(cantAlumnos) + "): ", 1, cantAlumnos);
    num--;

    cout << "\nEditando: " << alumno[num] << " (" << matricula[num] << ")" << endl;
    cout << "1. Nombre (" << alumno[num] << ")" << endl;
    cout << "2. Carrera (" << carrera[num] << ")" << endl;
    cout << "3. Semestre (" << semestre[num] << ")" << endl;
    cout << "4. Nota P1 (" << formatoNota(nota1[num]) << ")" << endl;
    cout << "5. Nota P2 (" << formatoNota(nota2[num]) << ")" << endl;
    cout << "6. Nota P3 (" << formatoNota(nota3[num]) << ")" << endl;
    cout << "7. Asistencia (" << formatoNota(asistencia[num]) << "%)" << endl;
    cout << "8. Cancelar" << endl;

    int campo = validarEntradaEntera("Campo a editar: ", 1, 8);

    switch (campo) {
        case 1: {
            string nuevoNombre;
            cout << "Nuevo nombre: ";
            cin.ignore();
            getline(cin, nuevoNombre);
            nuevoNombre = recortarEspacios(nuevoNombre);
            if (validarNombre(nuevoNombre)) {
                registrarLog("Nombre cambiado: " + alumno[num] + " -> " + nuevoNombre);
                alumno[num] = nuevoNombre;
                cout << "Nombre actualizado." << endl;
            } else {
                cout << "Nombre invalido." << endl;
            }
            break;
        }
        case 2: {
            string nuevaCarrera;
            cout << "Carreras: ";
            for (int i = 0; i < totalCarreras; i++) cout << carrerasValidas[i] << " ";
            cout << "\nNueva carrera: ";
            cin.ignore();
            getline(cin, nuevaCarrera);
            nuevaCarrera = recortarEspacios(nuevaCarrera);
            if (validarCarrera(nuevaCarrera)) {
                for (int i = 0; i < totalCarreras; i++) {
                    if (compararIgnorandoCase(nuevaCarrera, carrerasValidas[i])) {
                        nuevaCarrera = carrerasValidas[i];
                        break;
                    }
                }
                registrarLog("Carrera cambiada para " + alumno[num]);
                carrera[num] = nuevaCarrera;
                cout << "Carrera actualizada." << endl;
            } else {
                cout << "Carrera no valida." << endl;
            }
            break;
        }
        case 3:
            semestre[num] = validarEntradaEntera("Nuevo semestre (1-12): ", 1, 12);
            cout << "Semestre actualizado." << endl;
            break;
        case 4:
            nota1[num] = validarEntradaDecimal("Nueva nota P1 (0-100): ", 0, 100);
            cout << "Nota P1 actualizada." << endl;
            registrarLog("Nota P1 actualizada para " + alumno[num]);
            break;
        case 5:
            nota2[num] = validarEntradaDecimal("Nueva nota P2 (0-100): ", 0, 100);
            cout << "Nota P2 actualizada." << endl;
            registrarLog("Nota P2 actualizada para " + alumno[num]);
            break;
        case 6:
            nota3[num] = validarEntradaDecimal("Nueva nota P3 (0-100): ", 0, 100);
            cout << "Nota P3 actualizada." << endl;
            registrarLog("Nota P3 actualizada para " + alumno[num]);
            break;
        case 7:
            asistencia[num] = validarEntradaDecimal("Nueva asistencia (0-100%): ", 0, 100);
            cout << "Asistencia actualizada." << endl;
            break;
        case 8:
            cout << "Edicion cancelada." << endl;
            return;
    }
}

void toggleInscripcion() {
    if (cantAlumnos == 0) {
        cout << "No hay estudiantes." << endl;
        return;
    }
    int num = validarEntradaEntera("Numero de estudiante (1-" + to_string(cantAlumnos) + "): ", 1, cantAlumnos);
    num--;

    cout << alumno[num] << " esta actualmente: " << (inscrito[num] ? "INSCRITO" : "RETIRADO") << endl;
    cout << "Desea " << (inscrito[num] ? "RETIRAR" : "REINSCRIBIR") << "? (s/n): ";
    char resp;
    cin >> resp;
    if (resp == 's' || resp == 'S') {
        inscrito[num] = !inscrito[num];
        string nuevoEstado = inscrito[num] ? "reinscrito" : "retirado";
        cout << "Estudiante " << nuevoEstado << "." << endl;
        registrarLog("Estudiante " + nuevoEstado + ": " + alumno[num]);
    }
}

void reportePorCarrera() {
    if (cantAlumnos == 0) {
        cout << "No hay estudiantes." << endl;
        return;
    }

    cout << "\n" << lineaSeparadora(65, '=') << endl;
    cout << "         REPORTE POR CARRERA" << endl;
    cout << lineaSeparadora(65, '=') << endl;

    for (int c = 0; c < totalCarreras; c++) {
        int cantEnCarrera = 0;
        double sumaNotas = 0;
        double sumaAsist = 0;
        int aprobCarrera = 0;

        for (int i = 0; i < cantAlumnos; i++) {
            if (!inscrito[i]) continue;
            if (compararIgnorandoCase(carrera[i], carrerasValidas[c])) {
                cantEnCarrera++;
            }
        }

        if (cantEnCarrera == 0) continue;

        cout << "\n>> " << aMayusculas(carrerasValidas[c]) << " (" << cantEnCarrera << " estudiantes)" << endl;
        cout << lineaSeparadora(60, '-') << endl;

        for (int i = 0; i < cantAlumnos; i++) {
            if (!inscrito[i]) continue;
            if (compararIgnorandoCase(carrera[i], carrerasValidas[c])) {
                double f = calcularFinal(i);
                sumaNotas += f;
                sumaAsist += asistencia[i];
                if (f >= notaMinima) aprobCarrera++;
                cout << "  " << rellenarDerecha(alumno[i], 25)
                     << " | Sem " << semestre[i]
                     << " | Final: " << rellenarDerecha(formatoNota(f), 7)
                     << " (" << obtenerLetra(f) << ")"
                     << " | Asist: " << formatoNota(asistencia[i]) << "%" << endl;
            }
        }

        cout << "  " << lineaSeparadora(55, '.') << endl;
        cout << "  Promedio: " << formatoNota(sumaNotas / cantEnCarrera)
             << " | Asist. promedio: " << formatoNota(sumaAsist / cantEnCarrera) << "%"
             << " | Aprobacion: " << formatoNota(aprobCarrera * 100.0 / cantEnCarrera) << "%" << endl;
    }

    cout << "\n" << lineaSeparadora(65, '=') << endl;
    registrarLog("Reporte por carrera generado");
}

void exportarNotas() { // exporta el archivo de texto con los datos de prueba, va en el gestor de archivos 
    if (cantAlumnos == 0) {
        cout << "No hay datos para exportar." << endl;
        return;
    }

    string nombreArchivo = "notas_" + obtenerFechaActual() + ".txt"; 
    for (int i = 0; i < (int)nombreArchivo.length(); i++) {
        if (nombreArchivo[i] == '/') nombreArchivo[i] = '-';
    }

    ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "Error: No se pudo crear el archivo." << endl;
        return;
    }

    archivo << "REPORTE DE NOTAS - INSTITUTO TECNOLOGICO ABC" << endl;
    archivo << "Fecha: " << obtenerFechaActual() << " " << obtenerHoraActual() << endl;
    archivo << lineaSeparadora(90, '=') << endl;
    archivo << rellenarDerecha("NOMBRE", 25) << rellenarDerecha("MATRICULA", 12)
            << rellenarDerecha("CARRERA", 16) << rellenarDerecha("P1", 7)
            << rellenarDerecha("P2", 7) << rellenarDerecha("P3", 7)
            << rellenarDerecha("FINAL", 8) << rellenarDerecha("ESTADO", 12) << endl;
    archivo << lineaSeparadora(90, '-') << endl;

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

    archivo << lineaSeparadora(90, '=') << endl;
    archivo << "Aprobados: " << aprobados << " | Reprobados: " << reprobados << endl;
    archivo.close();

    cout << "Notas exportadas a: " << nombreArchivo << endl;
    registrarLog("Notas exportadas a: " + nombreArchivo);
}

void estudiantesEnRiesgo() {
    if (cantAlumnos == 0) {
        cout << "No hay estudiantes." << endl;
        return;
    }

    cout << "\n" << lineaSeparadora(75, '=') << endl;
    cout << "         ESTUDIANTES EN RIESGO ACADEMICO" << endl;
    cout << lineaSeparadora(75, '=') << endl;

    int enRiesgo = 0;
    for (int i = 0; i < cantAlumnos; i++) {
        if (!inscrito[i]) continue;
        if (estaEnRiesgo(i)) {
            double f = notaFinal[i];
            string razones = "";
            if (f < notaMinima) razones += "Nota baja";
            if (asistencia[i] < asistenciaMinima) {
                if (razones.length() > 0) razones += ", ";
                razones += "Baja asistencia";
            }
            if (f >= 55 && f <= 65) {
                if (razones.length() > 0 && razones.find("Nota baja") == string::npos) razones += ", ";
                if (razones.find("Nota baja") == string::npos) razones += "Zona critica (55-65)";
            }

            cout << "  " << rellenarDerecha(alumno[i], 25)
                 << " | " << matricula[i]
                 << " | Final: " << formatoNota(f)
                 << " | Asist: " << formatoNota(asistencia[i]) << "%"
                 << " | Razon: " << razones << endl;
            enRiesgo++;
        }
    }

    if (enRiesgo == 0) {
        cout << "  No hay estudiantes en riesgo academico." << endl;
    } else {
        cout << lineaSeparadora(75, '-') << endl;
        cout << "  Total en riesgo: " << enRiesgo << " estudiantes" << endl;
    }
    cout << lineaSeparadora(75, '=') << endl;
    registrarLog("Consulta de estudiantes en riesgo: " + to_string(enRiesgo) + " encontrados");
}

void verHistorialBoletas() {
    if (totalHistorial == 0) {
        cout << "No hay historial de boletas." << endl;
        return;
    }

    cout << "\n--- HISTORIAL DE BOLETAS GENERADAS ---" << endl;
    cout << rellenarDerecha("FECHA", 15) << rellenarDerecha("ESTUDIANTE", 25)
         << rellenarDerecha("NOTA", 10) << endl;
    cout << lineaSeparadora(50, '-') << endl;

    int inicio = totalHistorial > 20 ? totalHistorial - 20 : 0;
    for (int i = inicio; i < totalHistorial; i++) {
        cout << rellenarDerecha(historialFecha[i], 15)
             << rellenarDerecha(historialAlumno[i], 25)
             << rellenarDerecha(formatoNota(historialNota[i]), 10) << endl;
    }
    cout << lineaSeparadora(50, '-') << endl;
    cout << "Mostrando ultimos " << totalHistorial - inicio << " de " << totalHistorial << endl;
}

void verLog() {
    if (totalLog == 0) {
        cout << "No hay eventos." << endl;
        return;
    }

    cout << "\n--- LOG DEL SISTEMA ---" << endl;
    int inicio = totalLog > 30 ? totalLog - 30 : 0;
    for (int i = inicio; i < totalLog; i++) {
        cout << logEventos[i] << endl;
    }
    cout << lineaSeparadora(50, '-') << endl;
    cout << "Mostrando ultimos " << totalLog - inicio << " de " << totalLog << " eventos." << endl;
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

// ============ MAIN ============

int main() {
    cargarDatosPrueba();
    registrarLog("Sistema de notas iniciado");

    int op;
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
            case 1: agregarEstudiante(); break;
            case 2: verEstudiantes(); break;
            case 3: calcularNotaIndividual(); break;
            case 4: calcularTodas(); break;
            case 5: buscarEstudiante(); break;
            case 6: estadisticas(); break;
            case 7: modificarPesos(); break;
            case 8: tablaHonor(); break;
            case 9: editarEstudiante(); break;
            case 10: toggleInscripcion(); break;
            case 11: reportePorCarrera(); break;
            case 12: exportarNotas(); break;
            case 13: estudiantesEnRiesgo(); break;
            case 14: verHistorialBoletas(); break;
            case 15: verLog(); break;
            case 0:
                registrarLog("Sistema de notas cerrado");
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
