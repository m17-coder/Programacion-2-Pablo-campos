#include <iostream>
#include "persistencia/GestorArchivos.hpp"
#include "interfaz/Interfaz.hpp"
#include "Tienda/OPERACIONESTIENDA.HPP"

using namespace std;

int main() {
    if (!GestorArchivos::inicializarSistemaArchivos()) {
        std::cerr << "Error al inicializar archivos" << std::endl;
        return 1;
    }
    
    inicializarTiendaPorDefecto();
    
    Interfaz::ejecutar();

    return 0;
}