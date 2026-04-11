#ifndef COLEGIO_HPP
#define COLEGIO_HPP
#include "ESTUDIANTES.hpp"
#include <string>

    class Colegio{
        private:
            // carreras validas
            static const int totalCarreras = 6;
            


            // log del sistema
            std::string logEventos[500];
            int totalLog = 0;
            int cantAlumnos = 0;

            // configuracion academica
            double pesoP1 = 0.30, pesoP2 = 0.30, pesoP3 = 0.40;
            double notaMinima = 60.0;
            double notaMinimaHonor = 90.0;
            double asistenciaMinima = 75.0; // porcentaje minimo
            int aprobados = 0, reprobados = 0;

            // historial de consultas de boletas
            
            

        public:
        std::string carrerasValidas[totalCarreras]= {"Ingenieria", "Medicina", "Derecho", "Administracion", "Psicologia", "Arquitectura"};
        int totalHistorial = 0;   
        std::string historialAlumno[300];
            std::string historialFecha[300];
            double historialNota[300]; 
        void setAprovados(int ap);
            void setReprobados(int rep);
            void setCantAlumnos(int cant);
            // metodos para validar carreras
            bool validarCarrera(std::string carr);
            // metodos para registrar eventos en el log
            void registrarLog(std::string evento);
            void verLog() ;
            // metodos para configurar parametros academicos
            void modificarPesos();
            // metodos para calcular resultados academicos
            void calcularTodas(Estudiantes est[80]);
            std::string obtenerLetra(double nota);
            std::string obtenerEstado(double nota);
            void verEstudiantes(Estudiantes est[80]);
            void estudiantesEnRiesgo(Estudiantes est[80]);
            void verHistorialBoletas();
            void tablaHonor(Estudiantes est[80]);
            std::string obtenerEstadoAsistencia(double pctAsist) ;
            double getPesoP1();
            double getPesoP2();
            double getPesoP3();
            double getNotaMinima();
            double getNotaMinimaHonor();
            double getAsistenciaMinima();
            int getAprobados();
            int getReprobados();
            int getCantAlumnos();
            void toggleInscripcion(Estudiantes est);
            int getcantidadcarreras();
            void buscarEstudiante(Estudiantes est[80]);
            void estadisticas(Estudiantes est[80]);
            void editarEstudiante(Estudiantes est[80]);
            void reportePorCarrera(Estudiantes est[80]);
            void exportarNotas(Estudiantes est[80]);
        };
    #endif //colegio.hpp