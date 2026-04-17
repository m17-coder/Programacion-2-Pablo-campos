#include "PROVEEDORES.HPP"

// Constructor
Proveedor::Proveedor() {
    id = 0;
    nombre[0] = '\0';
    rif[0] = '\0';
    telefono[0] = '\0';
    email[0] = '\0';
    direccion[0] = '\0';
    fechaRegistro[0] = '\0';
    totalSuministros = 0;
    totalPagado = 0.0f;
    activo = true;
    fechaCreacion = time(nullptr);
    fechaUltimaModificacion = fechaCreacion;
}

// Getters y Setters
int Proveedor::getId() const { return id; }
void Proveedor::setId(int nuevoId) { id = nuevoId; }

const char* Proveedor::getNombre() const { return nombre; }
void Proveedor::setNombre(const char* nuevoNombre) {
    std::strncpy(nombre, nuevoNombre, sizeof(nombre) - 1);
    nombre[sizeof(nombre) - 1] = '\0';
}

const char* Proveedor::getRif() const { return rif; }
void Proveedor::setRif(const char* nuevoRif) {
    std::strncpy(rif, nuevoRif, sizeof(rif) - 1);
    rif[sizeof(rif) - 1] = '\0';
}

const char* Proveedor::getTelefono() const { return telefono; }
void Proveedor::setTelefono(const char* nuevoTelefono) {
    std::strncpy(telefono, nuevoTelefono, sizeof(telefono) - 1);
    telefono[sizeof(telefono) - 1] = '\0';
}

const char* Proveedor::getEmail() const { return email; }
void Proveedor::setEmail(const char* nuevoEmail) {
    std::strncpy(email, nuevoEmail, sizeof(email) - 1);
    email[sizeof(email) - 1] = '\0';
}

const char* Proveedor::getDireccion() const { return direccion; }
void Proveedor::setDireccion(const char* nuevaDireccion) {
    std::strncpy(direccion, nuevaDireccion, sizeof(direccion) - 1);
    direccion[sizeof(direccion) - 1] = '\0';
}

const char* Proveedor::getFechaRegistro() const { return fechaRegistro; }
void Proveedor::setFechaRegistro(const char* nuevaFecha) {
    std::strncpy(fechaRegistro, nuevaFecha, sizeof(fechaRegistro) - 1);
    fechaRegistro[sizeof(fechaRegistro) - 1] = '\0';
}

int Proveedor::getTotalSuministros() const { return totalSuministros; }
void Proveedor::setTotalSuministros(int nuevoTotal) { totalSuministros = nuevoTotal; }

float Proveedor::getTotalPagado() const { return totalPagado; }
void Proveedor::setTotalPagado(float nuevoTotal) { totalPagado = nuevoTotal; }

bool Proveedor::getActivo() const { return activo; }
void Proveedor::setActivo(bool estado) { activo = estado; }

time_t Proveedor::getFechaCreacion() const { return fechaCreacion; }
void Proveedor::setFechaCreacion(time_t fecha) { fechaCreacion = fecha; }

time_t Proveedor::getFechaUltimaModificacion() const { return fechaUltimaModificacion; }
void Proveedor::setFechaUltimaModificacion(time_t fecha) { fechaUltimaModificacion = fecha; }

// Métodos utilitarios
void Proveedor::actualizarUltimaModificacion() {
    fechaUltimaModificacion = time(nullptr);
}
