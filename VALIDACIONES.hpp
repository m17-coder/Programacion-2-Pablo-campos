#ifndef VALIDACIONES_HPP
#define VALIDACIONES_HPP

#include <string>

bool validarMatricula(std::string mat);
bool validarMatriculaUnica(std::string mat, int excluirIdx = -1);
bool validarNota(double nota);
bool validarNombre(std::string nombre);
bool validarCarrera(std::string carr);
bool validarSemestre(int sem);
bool validarAsistencia(double pct);
int validarEntradaEntera(std::string mensaje, int minimo, int maximo);
double validarEntradaDecimal(std::string mensaje, double minimo, double maximo);

#endif // VALIDACIONES_HPP
