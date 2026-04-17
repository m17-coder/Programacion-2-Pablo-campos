#include "FORMATOS.HPP"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>
#include <cstring>
#include <cctype>
#include <windows.h>

using namespace std;

std::string Formatos::formatearFecha(time_t fecha) {
    if (fecha <= 0) return "N/A";
    struct tm* tm_info = localtime(&fecha);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y", tm_info);
    return std::string(buffer);
}

std::string Formatos::formatearMoneda(float monto) {
    stringstream ss;
    ss << "$ " << fixed << setprecision(2) << monto;
    return ss.str();
}

void Formatos::convertirAMayusculas(char* texto) {
    if (texto == nullptr) return;
    int len = strlen(texto);
    for (int i = 0; i < len; i++) {
        texto[i] = toupper(texto[i]);
    }
}

void Formatos::limpiarBuffer() {
    if (cin.fail()) {
        cin.clear(); 
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void Formatos::pausar() {
    cout << "\nPresione ENTER para continuar...";
    if (cin.peek() == '\n') cin.get();
    else limpiarBuffer();
}

void Formatos::pausarYlimpiarpantalla() {
    pausar();
    system("cls");
}
