#include "VALIDACIONES.HPP"
#include "FORMATOS.HPP"
#include <iostream>
#include <limits>
#include <cstring>
#include <cctype>

using namespace std;

bool Validaciones::validarEmail(const char* email) {
    if(email == nullptr) return false;
    const char* atPos = strchr(email, '@');
    if (!atPos) return false; // No contiene '@'
    
    const char* dotPos = strchr(atPos, '.');
    if (!dotPos) return false; // No contiene '.' después del '@'
    
    if (atPos == email || dotPos == atPos + 1 || dotPos == email + strlen(email) - 1) {
        return false; // '@' no puede ser el primer carácter, '.' no puede estar justo después de '@' ni al final
    }
    
    return true;
}

bool Validaciones::validarRango(int valor, int min, int max) {
    return (valor >= min && valor <= max);
}

bool Validaciones::validarRIF(const char* rif) {
    // Validacion simple: al menos 8 caracteres y suele empezar por V, J, E, G
    if(rif == nullptr) return false;
    int len = strlen(rif);
    if(len < 8 || len > 15) return false;
    char primera = toupper(rif[0]);
    if(primera != 'V' && primera != 'J' && primera != 'E' && primera != 'G') return false;
    return true;
}

bool Validaciones::validarTelefono(const char* telefono) {
    // Validacion simple: que posea numeros, quiza algun '+' o '-' pero una log. basica
    if(telefono == nullptr) return false;
    int len = strlen(telefono);
    if(len < 10) return false;
    for(int i = 0; i < len; i++) {
        if(!isdigit(telefono[i]) && telefono[i] != '+' && telefono[i] != '-' && telefono[i] != ' ') {
            return false;
        }
    }
    return true;
}

bool Validaciones::validarNombre(const std::string& nombre) {
    if (nombre.length() < 3 || nombre.length() > 60) return false;
    for (int i = 0; i < (int)nombre.length(); i++) {
        char c = nombre[i];
        bool valido = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == ' '
                    || c == '\'' || c == '-';
        if ((unsigned char)c > 127) valido = true;
        if (!valido) return false;
    }
    return true;
}

int Validaciones::validarEntradaEntera(const string& mensaje, int minimo, int maximo) {
    int valor;
    bool valido = false;
    do {
        cout << mensaje;
        cin >> valor;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Error: Ingrese un numero valido." << endl;
        } else if (!validarRango(valor, minimo, maximo)) {
            cout << "Error: El valor debe estar entre " << minimo << " y " << maximo << "." << endl;
        } else {
            valido = true;
        }
    } while (!valido);
    Formatos::limpiarBuffer();
    return valor;
}

float Validaciones::validarEntradaFlotante(const string& mensaje, float minimo, float maximo) {
    float valor;
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
    Formatos::limpiarBuffer();
    return valor;
}
