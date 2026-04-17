#include "TRANSACCIONES.HPP"

DetalleVenta::DetalleVenta() : idproducto(0), cantidad(0), precioUnitario(0.0f) {}

int DetalleVenta::getIdProducto() const { return idproducto; }
void DetalleVenta::setIdProducto(int id) { idproducto = id; }

int DetalleVenta::getCantidad() const { return cantidad; }
void DetalleVenta::setCantidad(int cant) { cantidad = cant; }

float DetalleVenta::getPrecioUnitario() const { return precioUnitario; }
void DetalleVenta::setPrecioUnitario(float precio) { precioUnitario = precio; }

// -------------------------------------------------------------

Transaccion::Transaccion() {
    id = 0;
    idRelacionado = 0;
    total = 0.0f;
    fecha[0] = '\0';
    descripcion[0] = '\0';
    cantidaditems = 0;
    tipo = ' ';
    activo = true;
    fechaCreacion = time(nullptr);
    fechaUltimaModificacion = fechaCreacion;
}

int Transaccion::getId() const { return id; }
void Transaccion::setId(int nuevoId) { id = nuevoId; }

int Transaccion::getIdRelacionado() const { return idRelacionado; }
void Transaccion::setIdRelacionado(int nuevoIdRel) { idRelacionado = nuevoIdRel; }

float Transaccion::getTotal() const { return total; }
void Transaccion::setTotal(float nuevoTotal) { total = nuevoTotal; }

const char* Transaccion::getFecha() const { return fecha; }
void Transaccion::setFecha(const char* nuevaFecha) {
    std::strncpy(fecha, nuevaFecha, sizeof(fecha) - 1);
    fecha[sizeof(fecha) - 1] = '\0';
}

const char* Transaccion::getDescripcion() const { return descripcion; }
void Transaccion::setDescripcion(const char* nuevaDesc) {
    std::strncpy(descripcion, nuevaDesc, sizeof(descripcion) - 1);
    descripcion[sizeof(descripcion) - 1] = '\0';
}

int Transaccion::getCantidadItems() const { return cantidaditems; }

bool Transaccion::agregarDetalle(const DetalleVenta& detalle) {
    if (cantidaditems < 30) {
        detalles[cantidaditems] = detalle;
        cantidaditems++;
        return true;
    }
    return false; // Lleno
}

const DetalleVenta* Transaccion::getDetalles() const { return detalles; }

char Transaccion::getTipo() const { return tipo; }
void Transaccion::setTipo(char nuevoTipo) { tipo = nuevoTipo; }

bool Transaccion::getActivo() const { return activo; }
void Transaccion::setActivo(bool estado) { activo = estado; }

time_t Transaccion::getFechaCreacion() const { return fechaCreacion; }
void Transaccion::setFechaCreacion(time_t fecha) { fechaCreacion = fecha; }

time_t Transaccion::getFechaUltimaModificacion() const { return fechaUltimaModificacion; }
void Transaccion::setFechaUltimaModificacion(time_t fecha) { fechaUltimaModificacion = fecha; }

void Transaccion::calcularTotal() {
    float nuevoTotal = 0;
    for (int i = 0; i < cantidaditems; ++i) {
        nuevoTotal += (detalles[i].getCantidad() * detalles[i].getPrecioUnitario());
    }
    total = nuevoTotal;
}

void Transaccion::actualizarUltimaModificacion() {
    fechaUltimaModificacion = time(nullptr);
}
