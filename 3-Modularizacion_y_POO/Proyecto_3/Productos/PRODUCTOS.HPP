#ifndef PRODUCTOS_HPP
#define PRODUCTOS_HPP

#include <ctime>

class Producto {
private:
    int id;                    
    char codigo[20];           
    char nombre[100];          
    char descripcion[200];     
    int idProveedor;           
    float precio;              
    int existencia;           
    int stockminimo;           
    int totalVendido;          
    int stockInicial;           
    int totalComprado;
    char fechaRegistro[11];    
    time_t fechaCreacion;      
    time_t fechaUltimaModificacion; 
    bool activo;               

public:
    // Constructores
    Producto();
    // (Puedes agregar más constructores según necesites)

    // Getters
    int getId() const;
    const char* getCodigo() const;
    const char* getNombre() const;
    const char* getDescripcion() const;
    int getIdProveedor() const;
    float getPrecio() const;
    int getExistencia() const;
    int getStockminimo() const;
    int getTotalVendido() const;
    int getStockInicial() const;
    int getTotalComprado() const;
    const char* getFechaRegistro() const;
    time_t getFechaCreacion() const;
    time_t getFechaUltimaModificacion() const;
    bool getActivo() const;

    // Setters
    void setId(int nuevoId);
    void setCodigo(const char* nuevoCodigo);
    void setNombre(const char* nuevoNombre);
    void setDescripcion(const char* nuevaDescripcion);
    void setIdProveedor(int nuevoIdProveedor);
    void setPrecio(float nuevoPrecio);
    void setExistencia(int nuevaExistencia);
    void setStockminimo(int nuevoStockminimo);
    void setTotalVendido(int nuevoTotalVendido);
    void setStockInicial(int nuevoStockInicial);
    void setTotalComprado(int nuevoTotalComprado);
    void setFechaRegistro(const char* nuevaFechaRegistro);
    void setFechaCreacion(time_t nuevaFechaCreacion);
    void setFechaUltimaModificacion(time_t nuevaFechaUltimaModificacion);
    void setActivo(bool nuevoActivo);
    
    // Métodos útiles
    static int obtenerTamano();
};

#endif
