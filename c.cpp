#include <iostream>
 // Necesaria para SetConsoleOutputCP

int main() {
    // Forzamos a la consola a interpretar la salida como UTF-8
    SetConsoleOutputCP(CP_UTF8); 

    std::cout << "Configuración con UTF-8: Acción, ñandú y cigüeña." << std::endl;
    
    return 0;
}