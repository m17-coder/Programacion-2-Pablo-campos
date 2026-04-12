#ifndef ESTUDIANTES_HPP
#define ESTUDIANTES_HPP
#include <string>
#include <iostream>

class Colegio;

    class Estudiantes{
        private:
            std::string alumno;
            std::string matricula;
            std::string carrera;
            int semestre;
            double nota1, nota2, nota3, notaFinal;
            double asistencia; // porcentaje de asistencia 0-100
            bool inscrito; // si el alumno esta activo o se retiro
        public:
            void setNombre(std::string nombre);
            void setMatricula(std::string mat);
            void setCarrera(std::string carr);
            void setSemestre(int sem);
            void setNota(double n1,double n2,double n3);
            void setAsistencia(double asist);
            void setInscrito(bool estado);
            double calcularFinal(Colegio& uru);
            std::string getNombre();
            std::string getMatricula();
            std::string getCarrera();
            int getSemestre();
            double getNota1();
            double getNota2();
            double getNota3();
            double getAsistencia();
            bool getInscrito();
            double getNotaFinal();
            bool estaEnRiesgo(Colegio& uru);
            void agregarEstudiante(Colegio& uru);
            void calcularNotaIndividual(Colegio& uru);

// configuracion academica

};

void cargarDatosPrueba();

#endif // ESTUDIANTES_HPP