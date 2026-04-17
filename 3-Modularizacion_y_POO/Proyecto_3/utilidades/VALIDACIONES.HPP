#pragma once
#include <string>

class Validaciones {
public:
    static bool validarEmail(const char* email);
    static bool validarRIF(const char* rif);
    static bool validarTelefono(const char* telefono);
    static bool validarRango(int valor, int min, int max);
    
    static bool validarNombre(const std::string& nombre);
    static int validarEntradaEntera(const std::string& mensaje, int minimo, int maximo);
    static float validarEntradaFlotante(const std::string& mensaje, float minimo, float maximo);
};
