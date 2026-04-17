#ifndef TRANSACCIONES_HPP
#define TRANSACCIONES_HPP

#include <ctime>
#include <cstring>

class DetalleVenta {
private:
    int idproducto;
    int cantidad;
    float precioUnitario;

public:
    DetalleVenta();
    
    int getIdProducto() const;
    void setIdProducto(int id);

    int getCantidad() const;
    void setCantidad(int cant);

    float getPrecioUnitario() const;
    void setPrecioUnitario(float precio);
};

class Transaccion {
private:
    int id;                      // Identificador único (autoincremental)
    int idRelacionado;           // ID del proveedor (compra) o cliente (venta)
    float total;                 // suma de (cantidad * precioUnitario)
    char fecha[11];              // Formato: YYYY-MM-DD
    char descripcion[200];
    DetalleVenta detalles[30];   // Hasta 30 detalles
    int cantidaditems;
    char tipo;                   // 'C' para compra, 'V' para venta
    bool activo;                 // Indica si la transacción ha sido cancelada
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;

public:
    Transaccion();

    int getId() const;
    void setId(int nuevoId);

    int getIdRelacionado() const;
    void setIdRelacionado(int nuevoIdRel);

    float getTotal() const;
    void setTotal(float nuevoTotal);

    const char* getFecha() const;
    void setFecha(const char* nuevaFecha);

    const char* getDescripcion() const;
    void setDescripcion(const char* nuevaDesc);

    // Métodos para manejar los detalles de la transacción
    int getCantidadItems() const;
    bool agregarDetalle(const DetalleVenta& detalle);
    const DetalleVenta* getDetalles() const;

    char getTipo() const;
    void setTipo(char nuevoTipo);

    bool getActivo() const;
    void setActivo(bool estado);

    time_t getFechaCreacion() const;
    void setFechaCreacion(time_t fecha);

    time_t getFechaUltimaModificacion() const;
    void setFechaUltimaModificacion(time_t fecha);

    // Re-calcula el total sumando los detalles
    void calcularTotal();
    void actualizarUltimaModificacion();
};

#endif // TRANSACCIONES_HPP
