#ifndef TIENDA_HPP
#define TIENDA_HPP

#include <ctime>
#include <cstring>

class Tienda {
private:
    int id;                    // Necesario para GestorArchivos (generalmente siempre ID = 1)
    char nombre[100];          // Nombre de la tienda
    char rif[20];              // RIF de la tienda
    // Acumuladores de Negocio (Dinámicos)
    double caja;               // Dinero actual disponible para dar vuelto o gastos
    double ingresosTotales;    // Todo lo recaudado por ventas
    double egresosTotales;     // Todo lo pagado a proveedores
    
    // Contadores de Actividad
    int contadorVentas;        // Cuántas facturas de venta se han emitido
    int contadorCompras;       // Cuántas facturas de compra se han emitido
    
    // Auditoría
    bool activo;               // Requerido por compatibilidad de GestorArchivos
    time_t fechaUltimaTransaccion; 
    int versionSistema;

public:
    Tienda();

    int getId() const;
    void setId(int nuevoId);

    const char* getNombre() const;
    void setNombre(const char* nuevoNombre);

    const char* getRif() const;
    void setRif(const char* nuevoRif);

    double getCaja() const;
    void setCaja(double nuevaCaja);
    void agregarCaja(double monto);
    void restarCaja(double monto);

    double getIngresosTotales() const;
    void setIngresosTotales(double nuevosIngresos);
    void agregarIngreso(double monto);

    double getEgresosTotales() const;
    void setEgresosTotales(double nuevosEgresos);
    void agregarEgreso(double monto);

    int getContadorVentas() const;
    void setContadorVentas(int ventas);
    void incrementarVentas();

    int getContadorCompras() const;
    void setContadorCompras(int compras);
    void incrementarCompras();

    bool getActivo() const;
    void setActivo(bool estado);

    time_t getFechaUltimaTransaccion() const;
    void setFechaUltimaTransaccion(time_t fecha);
    void actualizarUltimaTransaccion();

    int getVersionSistema() const;
    void setVersionSistema(int version);
};

#endif // TIENDA_HPP
