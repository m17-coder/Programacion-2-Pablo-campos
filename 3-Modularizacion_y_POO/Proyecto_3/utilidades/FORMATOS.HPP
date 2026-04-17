#pragma once
#include <string>
#include <ctime>

class Formatos {
public:
    static std::string formatearFecha(time_t fecha);
    static std::string formatearMoneda(float monto);
    static void convertirAMayusculas(char* texto);
    static void limpiarBuffer();
    static void pausar();
    static void pausarYlimpiarpantalla();
};
