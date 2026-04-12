#include "VALIDACIONES.hpp"
#include "UTILIDADES.hpp"
#include "ESTUDIANTES.hpp"
#include "COLEGIO.hpp"
#include <iostream>

using namespace std;

extern Estudiantes est[80];
extern Colegio colegio;

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

bool validarMatriculaUnica(string mat, int excluirIdx) {
    string matUpper = aMayusculas(mat);
    for (int i = 0; i < colegio.getCantAlumnos(); i++) {
        if (i == excluirIdx) continue;
        if (aMayusculas(est[i].getMatricula()) == matUpper) return false;
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
    return colegio.validarCarrera(carr);
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