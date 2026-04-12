#ifndef UTILIDADES_HPP
#define UTILIDADES_HPP
#include <string>

std::string obtenerFechaActual();
std::string obtenerHoraActual();
std::string aMayusculas(std::string texto);
std::string aMinusculas(std::string texto);
double redondear(double valor, int decimales);
std::string recortarEspacios(std::string texto);
std::string rellenarDerecha(std::string texto, int largo, char relleno = ' ');
std::string rellenarIzquierda(std::string texto, int largo, char relleno = ' ');
std::string lineaSeparadora(int largo, char caracter = '-');
bool compararIgnorandoCase(std::string a, std::string b);
bool contieneTexto(std::string texto, std::string busqueda);
std::string formatoNota(double nota);
std::string barraProgreso(double porcentaje, int largo = 20);

#endif // UTILIDADES_HPP