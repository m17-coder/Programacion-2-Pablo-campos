#ifndef CLIENTES_HPP
#define CLIENTES_HPP

#include <ctime>
#include <cstring>

class Cliente {
private:
    int id;                      // Identificador único (autoincremental)
    char nombre[100];            // Nombre completo del cliente
    char cedula[20];             // Cédula o RIF
    char telefono[20];           // Teléfono de contacto
    char email[100];             // Correo electrónico
    char direccion[200];         // Dirección física
    int totalComprasRealizadas;  // Cuántas veces ha venido
    float montoTotalGastado;     // Total gastado en la tienda
    bool activo;                 // Indica si el cliente está activo o inactivo
    char fechaRegistro[11];      // Formato: YYYY-MM-DD
    time_t fechaCreacion;        // Timestamp de creación
    time_t fechaUltimaModificacion; // Timestamp de última modificación

public:
    Cliente();

    // Getters y Setters
    int getId() const;
    void setId(int nuevoId);

    const char* getNombre() const;
    void setNombre(const char* nuevoNombre);

    const char* getCedula() const;
    void setCedula(const char* nuevaCedula);

    const char* getTelefono() const;
    void setTelefono(const char* nuevoTelefono);

    const char* getEmail() const;
    void setEmail(const char* nuevoEmail);

    const char* getDireccion() const;
    void setDireccion(const char* nuevaDireccion);

    int getTotalComprasRealizadas() const;
    void setTotalComprasRealizadas(int nuevoTotal);

    float getMontoTotalGastado() const;
    void setMontoTotalGastado(float nuevoTotal);

    bool getActivo() const;
    void setActivo(bool estado);

    const char* getFechaRegistro() const;
    void setFechaRegistro(const char* nuevaFecha);

    time_t getFechaCreacion() const;
    void setFechaCreacion(time_t fecha);

    time_t getFechaUltimaModificacion() const;
    void setFechaUltimaModificacion(time_t fecha);

    // Métodos utilitarios
    void actualizarUltimaModificacion();
};

#endif // CLIENTES_HPP
