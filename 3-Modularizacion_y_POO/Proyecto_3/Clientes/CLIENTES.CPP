#include "CLIENTES.HPP"
#include <cstring>
#include <iostream>

Cliente::Cliente() {
    id = 0;
    nombre[0] = '\0';
    cedula[0] = '\0';
    telefono[0] = '\0';
    email[0] = '\0';
    direccion[0] = '\0';
    totalComprasRealizadas = 0;
    montoTotalGastado = 0.0f;
    activo = true;
    fechaRegistro[0] = '\0';
    fechaCreacion = time(nullptr);
    fechaUltimaModificacion = fechaCreacion;
}

int Cliente::getId() const { return id; }
void Cliente::setId(int nuevoId) { id = nuevoId; }

const char* Cliente::getNombre() const { return nombre; }
void Cliente::setNombre(const char* nuevoNombre) {
    std::strncpy(nombre, nuevoNombre, sizeof(nombre) - 1);
    nombre[sizeof(nombre) - 1] = '\0';
}

const char* Cliente::getCedula() const { return cedula; }
void Cliente::setCedula(const char* nuevaCedula) {
    std::strncpy(cedula, nuevaCedula, sizeof(cedula) - 1);
    cedula[sizeof(cedula) - 1] = '\0';
}

const char* Cliente::getTelefono() const { return telefono; }
void Cliente::setTelefono(const char* nuevoTelefono) {
    std::strncpy(telefono, nuevoTelefono, sizeof(telefono) - 1);
    telefono[sizeof(telefono) - 1] = '\0';
}

const char* Cliente::getEmail() const { return email; }
void Cliente::setEmail(const char* nuevoEmail) {
    std::strncpy(email, nuevoEmail, sizeof(email) - 1);
    email[sizeof(email) - 1] = '\0';
}

const char* Cliente::getDireccion() const { return direccion; }
void Cliente::setDireccion(const char* nuevaDireccion) {
    std::strncpy(direccion, nuevaDireccion, sizeof(direccion) - 1);
    direccion[sizeof(direccion) - 1] = '\0';
}

int Cliente::getTotalComprasRealizadas() const { return totalComprasRealizadas; }
void Cliente::setTotalComprasRealizadas(int nuevoTotal) { totalComprasRealizadas = nuevoTotal; }

float Cliente::getMontoTotalGastado() const { return montoTotalGastado; }
void Cliente::setMontoTotalGastado(float nuevoTotal) { montoTotalGastado = nuevoTotal; }

bool Cliente::getActivo() const { return activo; }
void Cliente::setActivo(bool estado) { activo = estado; }

const char* Cliente::getFechaRegistro() const { return fechaRegistro; }
void Cliente::setFechaRegistro(const char* nuevaFecha) {
    std::strncpy(fechaRegistro, nuevaFecha, sizeof(fechaRegistro) - 1);
    fechaRegistro[sizeof(fechaRegistro) - 1] = '\0';
}

time_t Cliente::getFechaCreacion() const { return fechaCreacion; }
void Cliente::setFechaCreacion(time_t fecha) { fechaCreacion = fecha; }

time_t Cliente::getFechaUltimaModificacion() const { return fechaUltimaModificacion; }
void Cliente::setFechaUltimaModificacion(time_t fecha) { fechaUltimaModificacion = fecha; }

void Cliente::actualizarUltimaModificacion() {
    fechaUltimaModificacion = time(nullptr);
}
