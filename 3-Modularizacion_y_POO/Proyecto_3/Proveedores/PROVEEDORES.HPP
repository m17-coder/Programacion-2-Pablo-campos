#ifndef PROVEEDORES_HPP
#define PROVEEDORES_HPP

#include <ctime>
#include <cstring>
#include <iostream>

class Proveedor {
private:
    int id;                    // Identificador único (autoincremental)
    char nombre[100];          // Nombre del proveedor
    char rif[20];              // RIF o identificación fiscal
    char telefono[20];         // Teléfono de contacto
    char email[100];           // Correo electrónico
    char direccion[200];       // Dirección física
    char fechaRegistro[11];    // Formato: YYYY-MM-DD
    int totalSuministros;      // Cuántas veces le hemos comprado
    float totalPagado;
    bool activo;               // Indica si el proveedor está activo o inactivo
    time_t fechaCreacion;      // Timestamp de creación
    time_t fechaUltimaModificacion; // Timestamp de última modificación

public:
    // Constructores
    Proveedor();

    // Getters y Setters
    int getId() const;
    void setId(int nuevoId);

    const char* getNombre() const;
    void setNombre(const char* nuevoNombre);

    const char* getRif() const;
    void setRif(const char* nuevoRif);

    const char* getTelefono() const;
    void setTelefono(const char* nuevoTelefono);

    const char* getEmail() const;
    void setEmail(const char* nuevoEmail);

    const char* getDireccion() const;
    void setDireccion(const char* nuevaDireccion);

    const char* getFechaRegistro() const;
    void setFechaRegistro(const char* nuevaFecha);

    int getTotalSuministros() const;
    void setTotalSuministros(int nuevoTotal);

    float getTotalPagado() const;
    void setTotalPagado(float nuevoTotal);

    bool getActivo() const;
    void setActivo(bool estado);

    time_t getFechaCreacion() const;
    void setFechaCreacion(time_t fecha);

    time_t getFechaUltimaModificacion() const;
    void setFechaUltimaModificacion(time_t fecha);

    // Métodos utilitarios
    void actualizarUltimaModificacion();
};

#endif // PROVEEDORES_HPP
