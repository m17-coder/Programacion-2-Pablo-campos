#include "TIENDA.HPP"
#include <cstring>

Tienda::Tienda() {
    id = 1;
    nombre[0] = '\0';
    rif[0] = '\0';
    caja = 0.0;
    ingresosTotales = 0.0;
    egresosTotales = 0.0;
    contadorVentas = 0;
    contadorCompras = 0;
    activo = true;
    fechaUltimaTransaccion = time(nullptr);
    versionSistema = 1;
}

int Tienda::getId() const { return id; }
void Tienda::setId(int nuevoId) { id = nuevoId; }

const char* Tienda::getNombre() const { return nombre; }
void Tienda::setNombre(const char* nuevoNombre) {
    std::strncpy(nombre, nuevoNombre, sizeof(nombre) - 1);
    nombre[sizeof(nombre) - 1] = '\0';
}

const char* Tienda::getRif() const { return rif; }
void Tienda::setRif(const char* nuevoRif) {
    std::strncpy(rif, nuevoRif, sizeof(rif) - 1);
    rif[sizeof(rif) - 1] = '\0';
}

double Tienda::getCaja() const { return caja; }
void Tienda::setCaja(double nuevaCaja) { caja = nuevaCaja; }
void Tienda::agregarCaja(double monto) { caja += monto; }
void Tienda::restarCaja(double monto) { caja -= monto; }

double Tienda::getIngresosTotales() const { return ingresosTotales; }
void Tienda::setIngresosTotales(double nuevosIngresos) { ingresosTotales = nuevosIngresos; }
void Tienda::agregarIngreso(double monto) { ingresosTotales += monto; }

double Tienda::getEgresosTotales() const { return egresosTotales; }
void Tienda::setEgresosTotales(double nuevosEgresos) { egresosTotales = nuevosEgresos; }
void Tienda::agregarEgreso(double monto) { egresosTotales += monto; }

int Tienda::getContadorVentas() const { return contadorVentas; }
void Tienda::setContadorVentas(int ventas) { contadorVentas = ventas; }
void Tienda::incrementarVentas() { contadorVentas++; }

int Tienda::getContadorCompras() const { return contadorCompras; }
void Tienda::setContadorCompras(int compras) { contadorCompras = compras; }
void Tienda::incrementarCompras() { contadorCompras++; }

bool Tienda::getActivo() const { return activo; }
void Tienda::setActivo(bool estado) { activo = estado; }

time_t Tienda::getFechaUltimaTransaccion() const { return fechaUltimaTransaccion; }
void Tienda::setFechaUltimaTransaccion(time_t fecha) { fechaUltimaTransaccion = fecha; }
void Tienda::actualizarUltimaTransaccion() { fechaUltimaTransaccion = time(nullptr); }

int Tienda::getVersionSistema() const { return versionSistema; }
void Tienda::setVersionSistema(int version) { versionSistema = version; }
