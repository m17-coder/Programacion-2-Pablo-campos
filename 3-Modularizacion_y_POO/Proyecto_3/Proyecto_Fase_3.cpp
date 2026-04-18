#include <iostream>
#include <iomanip>
#include <limits>
#include <string.h>
#include <windows.h>
#include <cstring>
#include <fstream>
#include <conio.h>
using namespace std;   

/*REPÚBLICA BOLIVARIANA DE VENEZUELA
UNIVERSIDAD RAFAEL URDANETA
FACULTAD DE INGENIERÍA
ESCUELA DE INGENIERÍA DE COMPUTACIÓN
ASIGNATURA: PROGRAMACIÓN 2
PROFESOR: ING. VICTOR KNEIDER
TRIMESTRE: 2026-A


________________


PROYECTO 3: POO Y MODULARIZACIÓN DEL SISTEMA DE INVENTARIO
OBJETIVOS DE APRENDIZAJE
* Aplicar principios de Programación Orientada a Objetos (POO) en C++
* Convertir estructuras (struct) a clases (class) con encapsulamiento
* Separar declaraciones (.hpp) de implementaciones (.cpp)
* Organizar el proyecto en múltiples archivos por dominio/servicio
* Dominar constructores, destructores y métodos de acceso
* Comprender el flujo de compilación multi-archivo


________________


FILOSOFÍA DE ESTA ETAPA
¿Qué cambia y qué se mantiene?
✅ SE MANTIENE:


* Toda la funcionalidad de archivos binarios del Proyecto 2
* Acceso aleatorio con seekg() y seekp()
* Headers en archivos binarios
* Persistencia de datos y operaciones CRUD completas


🔄 EVOLUCIONA:


* struct → class (con encapsulamiento)
* Código monolítico → Módulos organizados por dominio
* Funciones sueltas → Métodos de clase
* Acceso directo a atributos → Getters y Setters
* Todo en main.cpp → Arquitectura modular


________________


1. ESTRUCTURA DEL PROYECTO
1.1 Organización por Dominios
ProyectoTienda_v3/


│


├── tienda/


│   ├── Tienda.hpp


│   └── Tienda.cpp


│


├── productos/


│   ├── Producto.hpp


│   ├── Producto.cpp


│   ├── operacionesProductos.hpp


│   └── operacionesProductos.cpp


│


├── proveedores/


│   ├── Proveedor.hpp


│   ├── Proveedor.cpp


│   ├── operacionesProveedores.hpp


│   └── operacionesProveedores.cpp


│


├── clientes/


│   ├── Cliente.hpp


│   ├── Cliente.cpp


│   ├── operacionesClientes.hpp


│   └── operacionesClientes.cpp


│


├── transacciones/


│   ├── Transaccion.hpp


│   ├── Transaccion.cpp


│   ├── operacionesTransacciones.hpp


│   └── operacionesTransacciones.cpp


│


├── interfaz/


│   ├── Interfaz.hpp


│   └── Interfaz.cpp


│


├── persistencia/


│   ├── GestorArchivos.hpp


│   ├── GestorArchivos.cpp


│   └── Constantes.hpp


│


├── utilidades/


│   ├── Validaciones.hpp


│   ├── Validaciones.cpp


│   ├── Formatos.hpp


│   └── Formatos.cpp


│


├── main.cpp


├── Makefile


│


└── datos/                  # Archivos binarios (generados)


    ├── tienda.bin


    ├── productos.bin


    ├── proveedores.bin


    ├── clientes.bin


    └── transacciones.bin
1.2 Separación de Responsabilidades
Cada módulo se compone de:


* Entidad (Producto.hpp/cpp): Representa los datos con getters/setters y validaciones propias.
* Operaciones (operacionesProductos.hpp/cpp): Contiene la lógica de negocio y flujos (registrar, buscar, listar, etc.).
* Persistencia (GestorArchivos): Centraliza todas las operaciones de archivos binarios.
* Interfaz (Interfaz): Encapsula todos los menús y la navegación del sistema.
* main.cpp: Solo inicializa el sistema y llama a Interfaz::ejecutar(). Debe ser un archivo mínimo.


________________


2. CONVERSIÓN DE ESTRUCTURAS A CLASES
2.1 Reglas Generales
Cada clase de entidad debe tener:


* Atributos privados: Todos los campos del struct del Proyecto 2.
* Constructores: Por defecto, con parámetros principales, y de copia.
* Destructor.
* Getters (const) y Setters (con validación) para cada atributo.
* Métodos de validación propios de la entidad.
* Métodos de presentación (mostrarInformacionBasica(), mostrarInformacionCompleta()).
* Métodos de gestión de relaciones (ej. agregarTransaccionID(), eliminarProductoID()).
* Método estático obtenerTamano() que retorna sizeof de la clase.


La clase NO debe conocer operaciones de archivos. Eso es responsabilidad exclusiva de GestorArchivos.


Aplica estos principios a todas las entidades: Producto, Proveedor, Cliente, Transaccion y Tienda.


________________


3. PRINCIPIOS DE POO A APLICAR
3.1 Encapsulamiento
Los datos son privados, solo accesibles mediante métodos públicos.


Antes (Proyecto 2):


Producto p;


p.precio = -50;  // ❌ Sin validación


Ahora (Proyecto 3):


Producto p;


p.setPrecio(-50);  // ✅ El setter rechaza valores inválidos
3.2 Abstracción
El usuario de la clase llama producto.agregarTransaccionID(15) y no necesita saber que internamente se verifica espacio, se valida el ID, se actualiza un contador y se marca la fecha de modificación.
3.3 Responsabilidad Única
Clase
	Responsabilidad
	Producto
	Representar y gestionar datos de un producto
	Proveedor
	Representar y gestionar datos de un proveedor
	Cliente
	Representar y gestionar datos de un cliente
	Transaccion
	Representar y gestionar datos de una transacción
	Tienda
	Información general y contadores del sistema
	GestorArchivos
	Operaciones de persistencia en archivos binarios
	Interfaz
	Menús, navegación y flujo de interacción con el usuario
	Validaciones
	Validar formatos de datos generales
	

⚠️ ANTIPATRÓN — NO HACER:


class Producto {


    void guardarEnArchivo();  // ❌ Responsabilidad de GestorArchivos


    void mostrarMenu();       // ❌ Responsabilidad de main.cpp


};


________________


4. MÓDULO DE PERSISTENCIA
4.1 Clase GestorArchivos
Centraliza todas las operaciones de archivos binarios. Mantiene la misma estructura de ArchivoHeader del Proyecto 2.
4.2 Uso Obligatorio de Templates
Si observas la lógica de GestorArchivos, las operaciones CRUD a nivel de archivo son prácticamente idénticas para todas las entidades: abrir archivo, calcular offset, leer/escribir sizeof(T) bytes, cerrar archivo. La única diferencia es el tipo de dato y la ruta del archivo.


En lugar de escribir guardarProducto(), guardarProveedor(), guardarCliente(), guardarTransaccion() con código casi idéntico, debes utilizar templates (plantillas de C++) para crear métodos genéricos que funcionen con cualquier entidad:


template <typename T>


bool guardarRegistro(const char* archivo, T& registro);


template <typename T>


bool leerRegistroPorIndice(const char* archivo, int indice, T& registro);


template <typename T>


bool actualizarRegistro(const char* archivo, int indice, T& registro);


Con templates, escribes la lógica una sola vez y el compilador genera la versión específica para cada tipo cuando la invocas. Esto reduce drásticamente la cantidad de código repetido y facilita el mantenimiento: si corriges un bug en el método genérico, se corrige para todas las entidades.


Nota: Los métodos template deben implementarse en el archivo .hpp (no en .cpp), ya que el compilador necesita ver la implementación completa al momento de instanciar la plantilla.


________________


5. ARCHIVOS DE OPERACIONES
Cada módulo tiene su archivo de operaciones con la lógica de negocio. Estas funciones son las que main.cpp invoca.


Ejemplo: productos/operacionesProductos.hpp


#ifndef OPERACIONES_PRODUCTOS_HPP


#define OPERACIONES_PRODUCTOS_HPP


#include "../tienda/Tienda.hpp"


void registrarProducto(Tienda& tienda);


void buscarProducto(Tienda& tienda);


void actualizarProducto(Tienda& tienda);


void listarProductos(Tienda& tienda);


void eliminarProducto(Tienda& tienda);


void productosStockCritico(Tienda& tienda);


#endif


Las implementaciones en operacionesProductos.cpp contienen toda la lógica: solicitar datos al usuario, validar, llamar a GestorArchivos para persistir, y mostrar resultados. Replicar este patrón para proveedores, clientes y transacciones.


________________


6. MÓDULO DE UTILIDADES
Clase Validaciones
Funciones estáticas para validaciones generales de formato:


* validarEmail(const char*) → Contiene '@' y '.' después del @
* validarRIF(const char*) → Formato válido
* validarTelefono(const char*) → Formato válido
* validarRango(int valor, int min, int max) → Dentro de rango


Diferencia clave:


* Validaciones (clase estática): Reglas generales de formato. ¿Es un email válido?
* Métodos de entidad: Reglas de negocio específicas. ¿Este producto tiene datos completos?
Clase Formatos
Funciones estáticas para formateo de salida:


* formatearFecha(time_t) → "DD/MM/AAAA"
* formatearMoneda(float) → Formato con separador de miles
* convertirAMayusculas(char*)
* limpiarBuffer()
* pausar() → Espera Enter del usuario


________________


7. CLASE INTERFAZ Y main.cpp
7.1 Clase Interfaz
Todos los menús del sistema deben estar encapsulados en una clase Interfaz dentro de su propio módulo:


├── interfaz/


│   ├── Interfaz.hpp


│   └── Interfaz.cpp


Esta clase centraliza la presentación de menús y la lectura de opciones del usuario. Internamente, cada submenú delega la acción correspondiente a las funciones de operacionesXXX. De esta forma, ni main.cpp ni las clases de entidad conocen la lógica de navegación.


Responsabilidades de Interfaz:


* Mostrar el menú principal y los submenús de cada módulo (productos, proveedores, clientes, transacciones, mantenimiento).
* Leer y validar la opción seleccionada por el usuario.
* Invocar las funciones de operaciones correspondientes según la selección.
* Contener el loop principal del programa (ejecutar()).
7.2 main.cpp
Con la clase Interfaz, main.cpp queda reducido a su mínima expresión: inicializar el sistema y delegar todo el flujo.


#include "persistencia/GestorArchivos.hpp"


#include "interfaz/Interfaz.hpp"


#include <iostream>


int main() {


    if (!GestorArchivos::inicializarSistemaArchivos()) {


        cerr << "Error al inicializar archivos" << endl;


        return 1;


    }


    Interfaz::ejecutar();


    return 0;


}


main.cpp no debe contener lógica de negocio, menús, ni switches. Todo eso vive en Interfaz.


________________


8. GUARDAS DE INCLUSIÓN Y COMPILACIÓN
8.1 Guardas de Inclusión (OBLIGATORIO en todo .hpp)
#ifndef PRODUCTO_HPP


#define PRODUCTO_HPP


// ... contenido ...


#endif
8.2 Inclusiones en .cpp
Solo incluir lo que realmente se necesita:


// productos/Producto.cpp


#include "Producto.hpp"     // ✅ Siempre su propio .hpp primero


#include <iostream>         // ✅ Solo si usas cout/cin


#include <cstring>          // ✅ Solo si usas strcpy/strlen
8.3 Compilación Multi-Archivo
Implementar un Makefile que compile todos los módulos y genere el ejecutable final.


________________


9. REQUERIMIENTOS TÉCNICOS
9.1 Todos los Módulos Deben Tener
* Guardas de inclusión en .hpp
* Separación .hpp / .cpp
* Atributos privados, métodos públicos
* Constructores (por defecto + parametrizados) y destructor
* Getters const y Setters con validaciones
* Comentarios explicativos
9.2 Convenciones de Nombres
* Clases: PascalCase → GestorArchivos, Producto
* Métodos: camelCase → registrarProducto(), buscarPorID()
* Atributos privados: camelCase → nombreCompleto, fechaCreacion
* Constantes: UPPER_SNAKE_CASE → MAX_PRODUCTOS, RUTA_ARCHIVO
9.3 Uso de const
int getId() const;                              // Getters siempre const


const char* getNombre() const;                  // Retorno const para char[]


void setNombre(const char* nombre);             // Parámetros que no se modifican


bool buscarPorID(int id, Producto& resultado) const;


________________


10. DIFERENCIAS CLAVE CON PROYECTO 2
Aspecto
	Proyecto 2
	Proyecto 3
	Estructura de datos
	struct pública
	class encapsulada
	Organización
	1-2 archivos grandes
	Módulos por dominio
	Acceso a datos
	p.precio = 10
	p.setPrecio(10)
	Lógica de negocio
	En main o funciones sueltas
	En operacionesXXX.cpp
	Persistencia
	Funciones sueltas
	Clase GestorArchivos
	main.cpp
	500+ líneas
	50-100 líneas
	Validaciones
	Manuales antes de usar
	Dentro de setters
	Compilación
	g++ archivo.cpp
	Multi-archivo con Makefile
	

________________


11. ENTREGABLES
11.1 Código Fuente Organizado
Estructura de carpetas completa según la sección 1.1. Todos los módulos completamente implementados. La organización propuesta es una guía; pueden adaptarla a su criterio siempre que mantengan la separación por dominios.
11.2 Archivos de Prueba
Incluir en carpeta datos/ archivos .bin con al menos: 15 productos, 5 proveedores, 8 clientes y 25 transacciones.
11.3 Documentación
* Makefile funcional
* README_V3.md con instrucciones de compilación, estructura del proyecto y manual de usuario


________________


12. ESTRATEGIA DE IMPLEMENTACIÓN SUGERIDA
1. Fase 1: Toma struct Producto del Proyecto 2, conviértelo a class, crea operacionesProductos con registrarProducto(), compila y prueba.
2. Fase 2: Implementa GestorArchivos con métodos básicos (guardar, leer, listar). Prueba con Producto.
3. Fase 3: Completa el módulo de Productos con todas las operaciones.
4. Fase 4: Replica el patrón para Proveedor, Cliente y Transacción.
5. Fase 5: Crea main.cpp limpio con menús, conecta todo.
6. Fase 6: Extrae validaciones y formatos a las clases utilitarias.


________________




IMPORTANTE: No reinicies desde cero. Evoluciona tu Proyecto 2 aplicando estos conceptos gradualmente.


________________




Ing. Victor Kneider
Profesor de Programación 2
Universidad Rafael Urdaneta
*/
struct Producto {
    int id;                    // Identificador único (autoincremental)
    char codigo[20];           // Código del producto (ej: "PROD-001")
    char nombre[100];          // Nombre del producto
    char descripcion[200];     // Descripción del producto
    int idProveedor;           // ID del proveedor asociado
    float precio;              // Precio unitario
    int existencia;           // Cantidad actual en stock (calculada dinámicamente)
    int stockminimo;           // Cantidad mínima en inventario
    int totalVendido;          // Cantidad total vendida (para estadísticas)
    int stockInicial;           // Cuánto había al registrarlo
    int totalComprado;
    char fechaRegistro[11];    // Formato: YYYY-MM-DD
    time_t fechaCreacion;      // Timestamp de creación
    time_t fechaUltimaModificacion; // Timestamp de última modificación
    bool activo;               // Indica si el producto está activo o inactivo (no eliminado físicamente)
};

struct Proveedor {
    int id;                    // Identificador único (autoincremental)
    char nombre[100];          // Nombre del proveedor
    char rif[20];              // RIF o identificación fiscal
    char telefono[20];         // Teléfono de contacto
    char email[100];           // Correo electrónico
    char direccion[200];       // Dirección física
    char fechaRegistro[11];    // Formato: YYYY-MM-DD
    int totalSuministros;        // Cuántas veces le hemos comprado
    float totalPagado;
    bool activo;               // Indica si el proveedor está activo o inactivo
    time_t fechaCreacion;      // Timestamp de creación
    time_t fechaUltimaModificacion; // Timestamp de última modificación
};

struct Cliente {
    int id;                    // Identificador único (autoincremental)
    char nombre[100];          // Nombre completo del cliente
    char cedula[20];           // Cédula o RIF
    char telefono[20];         // Teléfono de contacto
    char email[100];           // Correo electrónico
    char direccion[200];       // Dirección física
    int totalComprasRealizadas;  // Cuántas veces ha venido
    float montoTotalGastado;
    bool activo;               // Indica si el cliente está activo o inactivo
    char fechaRegistro[11];    // Formato: YYYY-MM-DD
    time_t fechaCreacion;      // Timestamp de creación
    time_t fechaUltimaModificacion; // Timestamp de última modificación
};

struct DetalleVenta {
    int idProducto;
    int cantidad;
    float precioUnitario;
};

struct Transaccion {
    int id;                    // Identificador único (autoincremental)           // "COMPRA" o "VENTA"      
    int idRelacionado;         // ID del proveedor (compra) o cliente (venta)
    float total;               // cantidad * precioUnitario
    char fecha[11];            // Formato: YYYY-MM-DD
    char descripcion[200];
    DetalleVenta detalles[30]; // Permite hasta
    int cantidaditems;
    char tipo;                 // 'C' para compra, 'V' para venta
    bool activo;            // Indica si la transacción ha sido cancelada
    time_t fechaCreacion;      
    time_t fechaUltimaModificacion;
};

struct Tienda {
    char nombre[100];          // Nombre de la tienda
    char rif[20];              // RIF de la tienda
    // Acumuladores de Negocio (Dinámicos)
    double caja;          // Dinero actual disponible para dar vuelto o gastos
    double ingresosTotales;    // Todo lo recaudado por ventas
    double egresosTotales;     // Todo lo pagado a proveedores
    
    // Contadores de Actividad
    int contadorVentas;        // Cuántas facturas de venta se han emitido
    int contadorCompras;       // Cuántas facturas de compra se han emitido
    
    // Auditoría
    time_t fechaUltimaTransaccion; 
    int versionSistema;
};

struct Header{
    int cantidadRegistros;
    int ProximoId;
    int registrosActivos;
    int version;
};



void vaciarBuffer();
void pausarYlimpiarpantalla();
bool contieneSubstring(const char* texto, const char* busqueda);
bool pedirEnteroCancelable(const char* etiqueta, int &valor);
bool pedirTextoCancelable(const char* mensaje, char* destino, int tamano);
bool confirmarAccion(const char* mensaje);
//-------------------------------------//
//Funciones para inicializar archivos, leer y actualizar headers, calcular posiciones y calcular estadisticas
void inicializartienda(){
    ifstream archivotienda("tienda.bin", ios::binary);
    if(!archivotienda){
        Tienda tienda;
        strcpy(tienda.nombre, "TECNOSTORE");
        strcpy(tienda.rif, "J-12345678-9");
        
        // ¡AQUÍ FALTA INICIALIZAR EL COBRE!
        tienda.caja = 100000.0; 
        tienda.ingresosTotales = 0.0;
        tienda.egresosTotales = 0.0;
        tienda.contadorVentas = 0;
        tienda.contadorCompras = 0;

        ofstream archivoNuevo("tienda.bin", ios::binary);
        archivoNuevo.write(reinterpret_cast<char*>(&tienda), sizeof(Tienda));
        archivoNuevo.close();
        cout << "[SISTEMA]: Archivo tienda.bin creado con valores predeterminados." << endl;
    } else {
        archivotienda.close(); // Si existe, solo cerramos el archivo
    }
}

void crearArchivoBinario(const char* nombreArchivo) {
    ifstream archivotienda(nombreArchivo, ios::binary);
    if (!archivotienda) {
        // Si el archivo no existe, lo creamos con un Header vacío
        Header nuevoHeader = {0, 1, 0, 1}; // 0 registros, próximo ID es 1
        ofstream archivoNuevo(nombreArchivo, ios::binary);
        archivoNuevo.write(reinterpret_cast<char*>(&nuevoHeader), sizeof(Header));
        archivoNuevo.close();
        cout << "[SISTEMA]: Inicializado " << nombreArchivo << " con Header limpio." << endl;
    } else {
        archivotienda.close();
    }
}

// Lee el encabezado de cualquier archivo para obtener contadores y el próximo ID
Header leerHeader(const char* nombreArchivo) {
    Header h = {0, 1, 0, 1}; // Valores por defecto por seguridad
    ifstream archivo(nombreArchivo, ios::binary);
    if (archivo) {
        archivo.read(reinterpret_cast<char*>(&h), sizeof(Header));
        archivo.close();
    }
    return h;
}

//----------------------------------------------------------------//

//templates a usar//

template <typename T>
bool validarExistencia(const char* nombreArchivo, int idBuscar = -1) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo) return false;

    Header h;
    archivo.read(reinterpret_cast<char*>(&h), sizeof(Header));

    // CASO A: Solo queremos saber si el archivo tiene datos (como tieneDatos)
    if (idBuscar == -1) {
        archivo.close();
        return h.registrosActivos > 0;
    }

    // CASO B: Queremos validar si un ID específico existe
    T reg;
    for (int i = 0; i < h.cantidadRegistros; i++) {
        archivo.read(reinterpret_cast<char*>(&reg), sizeof(T));
        if (reg.id == idBuscar && reg.activo) {
            archivo.close();
            return true; // El ID existe
        }
    }

    archivo.close()
    return false; // El ID no existe
}

template <typename T>
int buscarPosicion(const char* nombreArchivo, int idBuscar) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo) return -1;

    Header h;
    archivo.read(reinterpret_cast<char*>(&h), sizeof(Header));

    T reg;
    for (int i = 0; i < h.cantidadRegistros; i++) {
        archivo.read(reinterpret_cast<char*>(&reg), sizeof(T));
        if (reg.id == idBuscar && reg.activo) {
            archivo.close();
            return i; 
        }
    }

    archivo.close();
    return -1; // No se encontró
}

template <typename T>
T leerRegistroPorIndice(const char* nombreArchivo, int indice) {
    T reg;
    ifstream archivo(nombreArchivo, ios::binary);
    
    if (!archivo) {
        // Si el archivo no abre, devolvemos un objeto con ID -1 para avisar
        reg.id = -1; 
        return reg;
    }

    // Calculamos posición: Saltamos el Header + (los registros anteriores)
    long posicion = sizeof(Header) + (indice * sizeof(T));
    
    archivo.seekg(posicion, ios::beg);
    archivo.read(reinterpret_cast<char*>(&reg), sizeof(T));
    
    archivo.close();
    return reg;
}

template <typename T>
int buscarRegistrosPorNombre(const char* nombreArchivo, const char* textoBusqueda, int indicesEncontrados[100]) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo) return 0;

    Header h;
    archivo.read(reinterpret_cast<char*>(&h), sizeof(Header));

    T reg;
    int contador = 0;

    for (int i = 0; i < h.cantidadRegistros; i++) {
        archivo.read(reinterpret_cast<char*>(&reg), sizeof(T));

        // Validamos que esté activo y que el nombre contenga lo que buscamos
        if (reg.activo && contieneSubstring(reg.nombre, textoBusqueda)) {
            indicesEncontrados[contador] = i; // Guardamos el ÍNDICE para acceso directo
            contador++;

            if (contador >= 100) break; // Seguridad
        }
    }

    archivo.close();
    return contador;
}

template <typename T>
void obtenerNombrePorID(const char* nombreArchivo, int idBuscar, char nombreDestino[100]) {
    strcpy(nombreDestino, "No encontrado");

    // Reutilizamos la lógica de buscar el ID único
    int idx = buscarPosicion<T>(nombreArchivo, idBuscar);

    if (idx != -1) {
        T reg = leerRegistroPorIndice<T>(nombreArchivo, idx);
        // Solo devolvemos el nombre si el registro está activo
        if (reg.activo) {
            strcpy(nombreDestino, reg.nombre);
        }
    }
}


template <typename T>
T leerDato(const char* mensaje) {
    T valor;
    while (true) {
        cout << mensaje;
        cin >> valor;
        if (cin.fail()) { // Si el usuario metió algo que no corresponde al tipo T
            cout << "[!] Error: Entrada no valida. Intente de nuevo.\n";
            cin.clear(); // Limpiamos el estado de error
            cin.ignore(1000, '\n'); // Descartamos la "basura" que quedó en el buffer
        } else {
            cin.ignore(1000, '\n'); // Limpiamos el buffer por seguridad
            return valor;
        }
    }
}

template <typename T, typename Func>
bool procesarRegistro(const char* nombreArchivo, int idBuscar, Func accion) {
    fstream archivo(nombreArchivo, ios::binary | ios::in | ios::out);
    if (!archivo) return false;

    Header h;
    archivo.read(reinterpret_cast<char*>(&h), sizeof(Header));

    T reg;
    bool encontrado = false;
    long posicion = -1;

    // Lógica para encontrar o preparar el registro
    if (idBuscar == -1) { // NUEVO REGISTRO
        posicion = sizeof(Header) + (h.cantidadRegistros * sizeof(T));
        reg.id = h.ProximoId;
        reg.activo = true;
        encontrado = true;
    } else { // BUSCAR EXISTENTE
        archivo.seekg(sizeof(Header), ios::beg); 

        for (int i = 0; i < h.cantidadRegistros; i++) {
            // Guardamos la posición exacta donde EMPIEZA este registro
            posicion = archivo.tellg(); 

            if (!archivo.read(reinterpret_cast<char*>(&reg), sizeof(T))) break;

            if (reg.id == idBuscar) {
                encontrado = true;
                break; // Salimos m
            }
        }
    }

    // --- EL FILTRO DE SEGURIDAD ---
    // Ejecutamos la lambda (accion) y vemos qué nos responde (true/false)
    if (encontrado && accion(reg)) { 
        // Si la lambda retornó TRUE, procedemos al guardado físico
        archivo.seekp(posicion, ios::beg);
        archivo.write(reinterpret_cast<const char*>(&reg), sizeof(T));

        if (idBuscar == -1) {
            h.cantidadRegistros++;
            h.registrosActivos++;
            h.ProximoId++;
        }

        archivo.seekp(0, ios::beg);
        archivo.write(reinterpret_cast<const char*>(&h), sizeof(Header));
        cout << "\n[Datos guardados exitosamente en el archivo." << endl;
    } 
    else {
        // Si la lambda retornó FALSE, el f.write NUNCA ocurre
        cout << "\nOperacion cancelada." << endl;
    }

    archivo.close();
    return encontrado;
}
//----------------------------------------------------//

// Funciones para Complementarias para recorrer los headers y encontrar valores

int calcularStockActual(Producto p) {
    int stock = (p.stockInicial + p.totalComprado) - p.totalVendido;
    return stock;
}


bool hayDineroEnCaja(float montoRequerido) {
    ifstream archivo("tienda.bin", ios::binary);
    if (!archivo) {
        cout << "[!] Error: No se pudo abrir tienda.bin para consultar el saldo." << endl;
        return false;
    }

    Tienda miTienda;
    archivo.read(reinterpret_cast<char*>(&miTienda), sizeof(Tienda));
    archivo.close();

    return (miTienda.caja >= montoRequerido);
}

// Devuelve el saldo actual para poder mostrarlo en mensajes de error
void actualizarFinanzasTienda(float monto, char tipo) {
    fstream archivo("tienda.bin", ios::in | ios::out | ios::binary);
    if (!archivo) return;

    Tienda miTienda;
    archivo.read(reinterpret_cast<char*>(&miTienda), sizeof(Tienda));

    if (tipo == 'V') { // Venta: entra dinero
        miTienda.caja += monto;
        miTienda.ingresosTotales += monto;
        miTienda.contadorVentas++;
    } else if (tipo == 'C') { // Compra: sale dinero
        miTienda.caja -= monto;
        miTienda.egresosTotales += monto;
        miTienda.contadorCompras++;
    }
    miTienda.fechaUltimaTransaccion = time(0);

    archivo.seekp(0, ios::beg);
    archivo.write(reinterpret_cast<const char*>(&miTienda), sizeof(Tienda));
    archivo.close();
}

// Actualiza los acumuladores del producto para que tu calculo sea exacto
void actualizarAcumuladoresProducto(int id, int cant, char tipoTransaccion) {
    int pos = buscarPosicion<Producto>("productos.bin", id);
    if (pos == -1) return;

    procesarRegistro<Producto>("productos.bin", id, [&](Producto& p) -> bool {
        if (tipoTransaccion == 'V') {
            p.totalVendido += cant;
            p.existencia -= cant; // Mantenemos existencia sincronizada
        } else if (tipoTransaccion == 'C') {
            p.totalComprado += cant;
            p.existencia += cant;
        }
        p.fechaUltimaModificacion = time(0);
        return true; 
    });
}
//---------------------------------------------------------------------//

//Funciones para la UX del usuario, como mostrar menús, validar entradas, etc.
void vaciarBuffer() {
    if (cin.fail()) {
        cin.clear(); // Limpia los flags de error de cin
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}
void pausarYlimpiarpantalla() {
    cout << "Presione Enter para continuar...";
    vaciarBuffer(); // Limpia el buffer de entrada
    system("cls"); // Limpia la pantalla
}
int submenu_producto(){
    int opcion;
    cout << "╔═══════════════════════════════════════════╗" << endl;
    cout << "║        GESTIÓN DE PRODUCTOS              ║" << endl;
    cout << "╚═══════════════════════════════════════════╝" << endl;
    cout << "1. Registrar nuevo producto" << endl;
    cout << "2. Buscar producto" << endl;
    cout << "3. Actualizar producto" << endl;
    cout << "4. Listar todos los productos" << endl;
    cout << "5. Eliminar producto" << endl;
    cout << "6. Restaurar producto eliminado" << endl;
    cout << "0. Volver al menú principal" << endl;
    opcion = leerDato<int>("Seleccione una opción: ");
    pausarYlimpiarpantalla();
    return opcion;
}
int submenu_proveedor(){
    int opcion;
    cout << "╔═══════════════════════════════════════════╗" << endl;
    cout << "║       GESTIÓN DE PROVEEDORES              ║" << endl;
    cout << "╚═══════════════════════════════════════════╝" << endl;
    cout << "1. Registrar proveedor" << endl;
    cout << "2. Buscar proveedor" << endl;
    cout << "3. Actualizar proveedor" << endl;
    cout << "4. Listar proveedores" << endl;
    cout << "5. Eliminar proveedor" << endl;
    cout << "6. Restaurar proveedor eliminado" << endl;
    cout << "0. Volver al menú principal" << endl;
    opcion = leerDato<int>("Seleccione una opción: ");
    pausarYlimpiarpantalla();
    return opcion;
}
int submenu_cliente(){
    int opcion;
    cout << "╔═══════════════════════════════════════════╗" << endl;
    cout << "║         GESTIÓN DE CLIENTES               ║" << endl;
    cout << "╚═══════════════════════════════════════════╝" << endl;
    cout << "1. Registrar cliente" << endl;
    cout << "2. Buscar cliente" << endl;
    cout << "3. Actualizar cliente" << endl;
    cout << "4. Listar clientes" << endl;
    cout << "5. Eliminar cliente" << endl;;
    cout << "6. Restaurar cliente eliminado" << endl;
    cout << "0. Volver al menú principal" << endl;
    opcion = leerDato<int>("Seleccione una opción: ");
    
    pausarYlimpiarpantalla();
    return opcion;
}
int submenu_transaccion(){
    int opcion;
    cout << "╔═══════════════════════════════════════════╗" << endl;
    cout << "║       GESTIÓN DE TRANSACCIONES            ║" << endl;
    cout << "╚═══════════════════════════════════════════╝" << endl;
    cout << "1. Registrar compra (a proveedor)" << endl;
    cout << "2. Registrar venta (a cliente)" << endl;
    cout << "3. Buscar transacciones" << endl;
    cout << "4. Listar todas las transacciones" << endl;
    cout << "5. Cancelar/Anular transacción" << endl;
    cout << "6. Restaurar transacción anulada" << endl;
    cout << "0. Volver al menú principal" << endl;
    opcion = leerDato<int>("Seleccione una opción: ");

    pausarYlimpiarpantalla();
    return opcion;
}

// Retorna false si el usuario presionó la tecla ESC
bool pedirTextoCancelable(const char* mensaje, char* destino, int tamano) {
    cout << mensaje << ": ";
    
    int i = 0;
    char tecla;

    while (true) {
        tecla = _getch(); // Captura la tecla al instante sin esperar el Enter

        if (tecla == 27) { // 27 es el código ASCII de 'Escape'
            cout << "\n[!] Operación cancelada por el usuario." << endl;
            pausarYlimpiarpantalla();
            return false; 
        }
        else if (tecla == '\r') { // '\r' es el código ASCII de 'Enter'
            destino[i] = '\0'; // Cerramos la cadena de texto correctamente
            cout << endl; // Bajamos de línea visualmente
            return true;
        }
        else if (tecla == '\b' && i > 0) { // Manejo de la tecla 'Retroceso' (Borrar)
            i--;
            cout << "\b \b"; // Borra el carácter de la pantalla usando secuencias de escape
        }
        else if (i < tamano - 1 && tecla >= 32 && tecla <= 126) { 
            // Solo aceptamos caracteres imprimibles (letras, números, espacios, símbolos)
            destino[i] = tecla;
            i++;
            cout << tecla; // Imprimimos en pantalla la letra que acaba de tocar
        }
    }
}

void mostrarInstruccionESC() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 8); // 8 = Gris oscuro (Color tenue)
    cout << " [i] Presione ESC en cualquier campo para cancelar y volver al menu principal." << endl;
    SetConsoleTextAttribute(hConsole, 7); // 7 = Restaura el color original (Blanco/Gris)
    cout << endl; // Un salto de linea para separar de los inputs
}
void mostrarProveedor(int Id_a_buscar) {
    int idx = buscarPosicion<Proveedor>("proveedores.bin", Id_a_buscar);
    if (idx <= 0) {
        cout << "[!] Proveedor no encontrado o inactivo." << endl;
        return;
    }
    // 2. Leemos el registro
    Proveedor temp = leerRegistroPorIndice<Proveedor>("proveedores.bin", idx);
    // 3. Mostramos la interfaz
    cout << "\n========================================" << endl;
    cout << "          DATOS DEL PROVEEDOR           " << endl;
    cout << "========================================" << endl;
    cout << " ID:         " << temp.id << endl;
    cout << " Nombre:     " << temp.nombre << endl;
    cout << " RIF:        " << temp.rif << endl;
    cout << " Telefono:   " << temp.telefono << endl;
    cout << " Email:      " << temp.email << endl;
    cout << " Direccion:  " << temp.direccion << endl;
    cout << " Registro:   " << temp.fechaRegistro << endl;
    cout << "========================================" << endl;
}

void mostrarcliente(int Id_a_buscar) {
    int idx = buscarPosicion<Cliente>("clientes.bin", Id_a_buscar);
    if (idx == -1) {
        cout << "\n[!] Error: Cliente no encontrado." << endl;
        return;
    }
    Cliente temp = leerRegistroPorIndice<Cliente>("clientes.bin", idx);
    cout << "\n--- DETALLE DEL CLIENTE ---" << endl;
    cout << " ID: " << temp.id << " | Nombre: " << temp.nombre << " | Cédula/RIF: " << temp.cedula << endl;
}

void mostrarTransaccion(int idBuscar) {
    int idx = buscarPosicion<Transaccion>("transacciones.bin", idBuscar);

    if (idx == -1) {
        cout << "[!] Transaccion no hallada." << endl;
        return;
    }

    Transaccion t = leerRegistroPorIndice<Transaccion>("transacciones.bin", idx);
    char nombreRelacionado[100];

    cout << "\n====================================================" << endl;
    cout << "             DETALLE DE TRANSACCION #" << t.id << endl;
    cout << "====================================================" << endl;
    
    if (t.tipo == 'V') {
        cout << " TIPO: VENTA";
        obtenerNombrePorID<Cliente>("clientes.bin", t.idRelacionado, nombreRelacionado);
    } else {
        cout << " TIPO: COMPRA";
        obtenerNombrePorID<Proveedor>("proveedores.bin", t.idRelacionado, nombreRelacionado);
    }
    
    cout << " | ENTIDAD: " << nombreRelacionado << endl;
    cout << " FECHA: " << t.fecha << " | DESC: " << t.descripcion << endl;
    cout << "----------------------------------------------------" << endl;
    cout << left << setw(8) << "ID" << setw(22) << "PRODUCTO" << setw(8) << "CANT" << setw(12) << "P. UNIT" << endl;
    cout << "----------------------------------------------------" << endl;

    // Recorrido de los detalles
    for (int j = 0; j < t.cantidaditems; j++) {
        char nombreP[100];
        obtenerNombrePorID<Producto>("productos.bin", t.detalles[j].idProducto, nombreP);

        cout << left << setw(8) << t.detalles[j].idProducto 
             << setw(22) << nombreP
             << setw(8) << t.detalles[j].cantidad 
             << "$ " << fixed << setprecision(2) << t.detalles[j].precioUnitario << endl;
    }

    cout << "----------------------------------------------------" << endl;
    cout << right << setw(40) << "TOTAL: $ " << t.total << endl;
    
    if (!t.activo) cout << "\n [ESTADO: ANULADA]" << endl;
    cout << "====================================================" << endl;
}

// El = -1 hace que el parámetro sea opcional
void mostrarProductos(int id_Para_flitrar_Proveedor = -1) {
    Header h = leerHeader("productos.bin");
    bool hayProductos = false;

    cout << "\n================================================================================" << endl;
    if (id_Para_flitrar_Proveedor == -1) {
        cout << "                         CATALOGO GENERAL DE PRODUCTOS" << endl;
    } else {
        cout << "                 PRODUCTOS ASOCIADOS AL PROVEEDOR #" << id_Para_flitrar_Proveedor << endl;
    }
    cout << "================================================================================" << endl;
    cout << left << setw(6) << "ID" << setw(25) << "NOMBRE" << setw(12) << "PRECIO" << setw(10) << "STOCK" << setw(10) << "PROV" << endl;
    cout << "--------------------------------------------------------------------------------" << endl;

    for (int i = 0; i < h.cantidadRegistros; i++) {
        Producto p = leerRegistroPorIndice<Producto>("productos.bin", i);

        if (p.activo) {
            // Lógica de filtrado:
            // Si el filtro es -1 (mostrar todos) O si el idProveedor coincide con el filtro
            bool mostrar = false;
            if (id_Para_flitrar_Proveedor == -1) {
                mostrar = true;
            } else {
                if (p.idProveedor == id_Para_flitrar_Proveedor) {
                    mostrar = true;
                }
            }

            if (mostrar) {
                hayProductos = true;
                cout << left << setw(6) << p.id 
                     << setw(25) << p.nombre 
                     << "$ " << setw(10) << fixed << setprecision(2) << p.precio
                     << setw(10) << calcularStockActual(p)
                     << setw(10) << p.idProveedor << endl;
            }
        }
    }

    if (!hayProductos) {
        cout << "[!] No se encontraron productos para mostrar." << endl;
    }
    cout << "================================================================================" << endl;
}

void buscarProductoPorNombre(const char* textoBusqueda) {
    int indices[100];
    int encontrados = buscarRegistrosPorNombre<Producto>("productos.bin", textoBusqueda, indices);

    if (encontrados == 0) {
        cout << "[!] No se encontraron productos que coincidan con '" << textoBusqueda << "'." << endl;
        return;
    }

    cout << "\n--- RESULTADOS DE LA BÚSQUEDA ---" << endl;
    cout << left << setw(5) << "ID" << setw(30) << "NOMBRE" << setw(15) << "PROVEEDOR" << endl;
    cout << "--------------------------------------------------------" << endl;

    for (int i = 0; i < encontrados; i++) {
        Producto p = leerRegistroPorIndice<Producto>("productos.bin", indices[i]);
        char nombreProv[100];
        obtenerNombrePorID<Proveedor>("proveedores.bin", p.idProveedor, nombreProv);

        cout << left << setw(5) << p.id 
             << setw(30) << p.nombre 
             << setw(15) << nombreProv 
             << endl;
    }
}

void buscarProveedorPorNombre(const char* textoBusqueda) {
    int indices[100];
    int encontrados = buscarRegistrosPorNombre<Proveedor>("proveedores.bin", textoBusqueda, indices);

    if (encontrados == 0) {
        cout << "[!] No se encontraron proveedores que coincidan con '" << textoBusqueda << "'." << endl;
        return;
    }

    cout << "\n--- RESULTADOS DE LA BÚSQUEDA ---" << endl;
    cout << left << setw(5) << "ID" << setw(30) << "NOMBRE" << setw(15) << "RIF" << endl;
    cout << "--------------------------------------------------------" << endl;

    for (int i = 0; i < encontrados; i++) {
        Proveedor p = leerRegistroPorIndice<Proveedor>("proveedores.bin", indices[i]);
        cout << left << setw(5) << p.id 
             << setw(30) << p.nombre 
             << setw(15) << p.rif 
             << endl;
    }
}

void buscarClientePorNombre(const char* textoBusqueda) {
    int indices[100];
    int encontrados = buscarRegistrosPorNombre<Cliente>("clientes.bin", textoBusqueda, indices);

    if (encontrados == 0) {
        cout << "[!] No se encontraron clientes que coincidan con '" << textoBusqueda << "'." << endl;
        return;
    }

    cout << "\n--- RESULTADOS DE LA BÚSQUEDA ---" << endl;
    cout << left << setw(5) << "ID" << setw(30) << "NOMBRE" << setw(15) << "CEDULA" << endl;
    cout << "--------------------------------------------------------" << endl;

    for (int i = 0; i < encontrados; i++) {
        Cliente c = leerRegistroPorIndice<Cliente>("clientes.bin", indices[i]);
        cout << left << setw(5) << c.id 
             << setw(30) << c.nombre 
             << setw(15) << c.cedula 
             << endl;
    }
}

void listarProveedoresEliminados() {
    ifstream archivo("proveedores.bin", ios::binary);
    if (!archivo) return;

    Header h;
    archivo.read(reinterpret_cast<char*>(&h), sizeof(Header));
    Proveedor temp;
    bool hayEliminados = false;

    cout << "\n--- PROVEEDORES EN PAPELERA (INACTIVOS) ---" << endl;
    cout << left << setw(5) << "ID" << setw(30) << "NOMBRE" << setw(15) << "RIF" << endl;
    cout << "--------------------------------------------------------" << endl;

    for (int i = 0; i < h.cantidadRegistros; i++) {
        archivo.read(reinterpret_cast<char*>(&temp), sizeof(Proveedor));
        if (!temp.activo) { // Solo mostramos los que tienen activo == false
            hayEliminados = true;
            cout << left << setw(5) << temp.id << setw(30) << temp.nombre << setw(15) << temp.rif << endl;
        }
    }

    if (!hayEliminados) cout << "[!] No hay proveedores para restaurar." << endl;
    archivo.close();
}

void listarClientesEliminados() {
    ifstream archivo("clientes.bin", ios::binary);
    if (!archivo) return;

    Header h;
    archivo.read(reinterpret_cast<char*>(&h), sizeof(Header));
    
    Cliente temp;
    bool hayEliminados = false;

    // Encabezado con el mismo estilo de proveedores
    cout << "\n--- CLIENTES EN PAPELERA (INACTIVOS) ---" << endl;
    cout << left << setw(5) << "ID" << setw(30) << "NOMBRE" << setw(15) << "CEDULA" << endl;
    cout << "--------------------------------------------------------" << endl;

    for (int i = 0; i < h.cantidadRegistros; i++) {
        archivo.read(reinterpret_cast<char*>(&temp), sizeof(Cliente));
        
        // Filtro: Solo mostramos los que tienen activo == false
        if (!temp.activo) { 
            hayEliminados = true;
            cout << left << setw(5) << temp.id 
                 << setw(30) << temp.nombre 
                 << setw(15) << temp.cedula << endl;
        }
    }

    if (!hayEliminados) {
        cout << "[!] No hay clientes para restaurar." << endl;
    }
    
    archivo.close();
}

void listarProductosEliminados() {
    system("cls");
    cout << "==================================================" << endl;
    cout << "          LISTA DE PRODUCTOS ELIMINADOS           " << endl;
    cout << "==================================================" << endl;
    cout << left << setw(10) << "ID" << setw(25) << "Nombre" << "Stock" << endl;
    cout << "--------------------------------------------------" << endl;

    Header h = leerHeader("productos.bin");
    bool encontrados = false;

    for (int i = 0; i < h.cantidadRegistros; i++) {
        Producto p = leerRegistroPorIndice<Producto>("productos.bin", i);
        if (!p.activo) {
            cout << left << setw(10) << p.id 
                 << setw(25) << p.nombre 
                 << p.existencia << endl;
            encontrados = true;
        }
    }

    if (!encontrados) {
        cout << "\n[i] No hay productos eliminados para mostrar." << endl;
    }
    
    cout << "==================================================" << endl;
    pausarYlimpiarpantalla();
}

//-----------------------------------------------------------------------------------------//

// Validaciones específicas para cada campo, como verificar que un ID exista o que un código de producto sea único, etc. Estas funciones se pueden llamar dentro de los procesos de creación o actualización para asegurar la integridad de los datos.

bool validarEmail(const char* email){
    if(email == nullptr) return false;
    const char* atPos = strchr(email, '@');
    if (!atPos) return false; // No contiene '@'
    
    const char* dotPos = strchr(atPos, '.');
    if (!dotPos) return false; // No contiene '.' después del '@'
    
    if (atPos == email || dotPos == atPos + 1 || dotPos == email + strlen(email) - 1) {
        return false; // '@' no puede ser el primer carácter, '.' no puede estar justo después de '@' ni al final
    }
    
    return true;
    
}

bool obtenerFechaActual(char* buffer) {
    if (buffer == nullptr) return false;
    
    SYSTEMTIME st;
    GetLocalTime(&st);

    // Validación de seguridad: El sistema no debería operar con fechas anteriores a su creación
    if (st.wYear < 2026) {
        cout << "\n[!] ERROR DE SISTEMA: La fecha detectada es " << st.wYear << "." << endl;
        cout << "[!] El reloj de su PC parece estar desconfigurado." << endl;
        cout << "[!] Por seguridad, no se pueden registrar datos con fechas antiguas." << endl;
        return false;
    }

    sprintf(buffer, "%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay);
    return true;
}

bool rifDuplicado(const char* rifABuscar) {
    ifstream archivo("proveedores.bin", ios::binary);
    if (!archivo) return false;

    Header h;
    archivo.read(reinterpret_cast<char*>(&h), sizeof(Header));
    Proveedor temp;

    for (int i = 0; i < h.cantidadRegistros; i++) {
        archivo.read(reinterpret_cast<char*>(&temp), sizeof(Proveedor));
        
        // Solo nos importa si el RIF coincide y el proveedor está ACTIVO
        if (temp.activo && strcmp(temp.rif, rifABuscar) == 0) {
            archivo.close();
            return true; // Encontramos un conflicto activo
        }
    }
    archivo.close();
    return false; // No hay conflictos con proveedores activos
}

bool codigoProductoDuplicado(const char* codigoABuscar) {
    ifstream archivo("productos.bin", ios::binary);
    if (!archivo) return false;

    Header h;
    archivo.read(reinterpret_cast<char*>(&h), sizeof(Header));
    Producto temp;

    for (int i = 0; i < h.cantidadRegistros; i++) {
        archivo.read(reinterpret_cast<char*>(&temp), sizeof(Producto));
        
        // Comparamos ignorando mayúsculas/minúsculas para mayor seguridad
        if (temp.activo && strcasecmp(temp.codigo, codigoABuscar) == 0) { // Usa _stricmp en Windows si strcasecmp falla
            archivo.close();
            return true; 
        }
    }
    archivo.close();
    return false;
}

bool cedulaDuplicada(const char* cedulaABuscar) {
    ifstream archivo("clientes.bin", ios::binary);
    if (!archivo) return false;

    Header h;
    archivo.read(reinterpret_cast<char*>(&h), sizeof(Header));
    Cliente temp;

    for (int i = 0; i < h.cantidadRegistros; i++) {
        archivo.read(reinterpret_cast<char*>(&temp), sizeof(Cliente));
        if (temp.activo && strcmp(temp.cedula, cedulaABuscar) == 0) {
            archivo.close();
            return true; 
        }
    }
    archivo.close();
    return false;
}

bool confirmarAccion(const char* mensaje) {
    char respuesta;
    cout << "\n[?] " << mensaje << " (S/N): ";
    cin >> respuesta;
    vaciarBuffer(); // Limpia el enter para que la siguiente lectura no falle
    return (toupper(respuesta) == 'S');
}

bool pedirEnteroCancelable(const char* etiqueta, int &valor) {
    cout << etiqueta << ": ";
    string entrada = "";
    char c;
    while (true) {
        c = getch();
        if (c == 27) { // ESC
            cout << " [CANCELADO]" << endl;
            return false; 
        }
        if (c == 13) { // ENTER
            cout << endl;
            break;
        }
        if (isdigit(c)) {
            cout << c;
            entrada += c;
        }
        if (c == 8 && entrada.length() > 0) { // Backspace
            cout << "\b \b";
            entrada.pop_back();
        }
    }
    if (entrada == "") valor = 0;
    else valor = stoi(entrada);
    return true;
}

bool validarEnteroPositivo(const char* etiqueta, int &destino) {
    int valor;
    while (true) {
        if (!pedirEnteroCancelable(etiqueta, valor)) return false; // Salir si presiona ESC
        
        if (valor >= 0) {
            destino = valor;
            return true;
        }
        cout << "  [!] Error: El valor no puede ser negativo. Reintente." << endl;
    }
}

void actualizarStockProducto(int id, int cantidad, bool esSuma) {
    int pos = buscarPosicion<Producto>("productos.bin", id);
    if (pos == -1) return;

    procesarRegistro<Producto>("productos.bin", pos, [&](Producto& p) -> bool {
        if (esSuma) {
            p.existencia += cantidad;
        } else {
            // Regla de oro: No permitir menos de 0
            if (p.existencia < cantidad) {
                p.existencia = 0; 
            } else {
                p.existencia -= cantidad;
            }
        }
        p.fechaUltimaModificacion = time(0);
        return true; // Forzar escritura en el archivo binario
    });
}

bool validarFloatPositivo(const char* etiqueta, float &destino) {
    float valor;
    while (true) {
        cout << etiqueta << ": ";
        if (!(cin >> valor)) {
            vaciarBuffer();
            cout << "  [!] Entrada invalida." << endl;
            continue;
        }
        
        if (valor >= 0) {
            destino = valor;
            return true;
        }
        cout << "  [!] Error: El precio/monto no puede ser negativo." << endl;
    }
}

bool contieneSubstring(const char* texto, const char* busqueda){
    int lenTexto = strlen(texto);
    int lenBusqueda = strlen(busqueda);
    if (lenBusqueda > lenTexto) return false;
    for (int i = 0; i <= lenTexto - lenBusqueda; i++) {
        bool encontrado = true;
        for (int j = 0; j < lenBusqueda; j++) {
            if (tolower(texto[i + j]) != tolower(busqueda[j])) {
                encontrado = false;
                break;
            }
        }
        if (encontrado) return true;
    }
    return false;
}

//------------------------------------------------//

//CRUDS//
//CRUD PRODUCTO//
void crearProducto() {
    cout << "\n==================================================" << endl;
    cout << "             NUEVO PRODUCTO (REGISTRO)             " << endl;
    cout << "==================================================" << endl;
    mostrarInstruccionESC();

    // Llamamos al motor Maestro. -1 indica que es un registro NUEVO.
    procesarRegistro<Producto>("productos.bin", -1, [](Producto& p) -> bool {
        
        cout << "Registrando Producto ID: " << p.id << " (Automático)" << endl;
        cout << "--------------------------------------------------" << endl;

        // 1. CAPTURA DE DATOS BÁSICOS
        if (!pedirTextoCancelable("Nombre del Producto", p.nombre, 100)) return false;
        
        // Validación: Código Único
        do {
            if (!pedirTextoCancelable("Codigo Interno (Ej: P-001)", p.codigo, 20)) return false;
            if (codigoProductoDuplicado(p.codigo)) {
                cout << "  [!] Error: El codigo '" << p.codigo << "' ya esta en uso por otro producto activo.\n";
            } else {
                break; // El código es válido y único
            }
        } while (true);

        if (!pedirTextoCancelable("Breve Descripcion", p.descripcion, 200)) return false;

        // 2. VALIDACIÓN DE LLAVE FORÁNEA (PROVEEDOR)
        char nombreProv[100];
        do {
            if (!pedirEnteroCancelable("ID del Proveedor suministrador", p.idProveedor)) return false;
        
            if (validarExistencia<Proveedor>("proveedores.bin", p.idProveedor)) {
                obtenerNombrePorID<Proveedor>("proveedores.bin", p.idProveedor, nombreProv);
                cout << "  [OK] Proveedor verificado: " << nombreProv << endl;
                break;
            } else {
                cout << "  [!] Error: El proveedor con ID " << p.idProveedor << " no existe o esta inactivo.\n";
            }
        } while (true);

        // 3. CAPTURA Y VALIDACIÓN DE NÚMEROS (Evitar negativos)
        if (!validarFloatPositivo("Precio de Venta ($)", p.precio)) return false;

        if (!validarEnteroPositivo("Stock Mínimo", p.stockminimo)) return false;

        if (!validarEnteroPositivo("Stock Inicial", p.stockInicial)) return false;


        // 4. VALORES DEL SISTEMA (Automáticos)
        p.existencia = p.stockInicial; // Al crear, la existencia es igual al stock inicial
        p.totalVendido = 0;
        p.totalComprado = 0;
        p.fechaCreacion = time(0);
        p.fechaUltimaModificacion = time(0);
        
        // Obtener fecha en formato string (YYYY-MM-DD)
        time_t t = time(0);
        struct tm * now = localtime(&t);
        sprintf(p.fechaRegistro, "%04d-%02d-%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);

        // 5. RESUMEN VISUAL ANTES DE GUARDAR
        cout << "\n--------------------------------------------------" << endl;
        cout << "          REVISIÓN DE DATOS PRE-GUARDADO          " << endl;
        cout << "--------------------------------------------------" << endl;
        cout << left << setw(15) << " > ID:" << p.id << " (Asignado automáticamente)" << endl;
        cout << left << setw(15) << " > PRODUCTO:" << p.nombre << " [" << p.codigo << "]" << endl;
        cout << left << setw(15) << " > PROVEEDOR:" << nombreProv << " (ID: " << p.idProveedor << ")" << endl;
        cout << left << setw(15) << " > PRECIO:" << "$ " << fixed << setprecision(2) << p.precio << endl;
        cout << left << setw(15) << " > STOCK INI:" << p.stockInicial << endl;
        cout << left << setw(15) << " > S. MINIMO:" << p.stockminimo << endl;
        cout << "--------------------------------------------------" << endl;

        // 6. EL FILTRO DE SEGURIDAD (Confirmación)
        return confirmarAccion("¿Desea confirmar el guardado de este producto?");
    });
}
void buscarProducto() {
    cout << "ingrese 1 para buscar por ID o 2 para buscar por nombre: ";
    int opcion;
    if(!(cin >> opcion) || (opcion != 1 && opcion != 2)) {
        cout << "[!] Opcion invalida. Volviendo al menu principal." << endl;
        pausarYlimpiarpantalla();
        return;
    }
    vaciarBuffer();
    if(opcion == 1) {
        int idBuscar;
        if (pedirEnteroCancelable("Ingrese el ID del producto a buscar", idBuscar)) {
            mostrarProductos(idBuscar);
        }
    } else if(opcion==2) {
        char nombreBusqueda[100];
        if (pedirTextoCancelable("Ingrese el nombre del producto a buscar", nombreBusqueda, 100)) {
            buscarProductoPorNombre(nombreBusqueda);
        }
    }
};
void actualizarProducto() {
    int idBuscar;

    cout << "\n--- ACTUALIZAR PRODUCTO ---" << endl;
    mostrarInstruccionESC(); 

    // 1. Buscamos el ID con opción de cancelar
    if (!pedirEnteroCancelable("Ingrese el ID del producto a actualizar", idBuscar)) return;

    // 2. Usamos el Template para que nos traiga el producto y gestione el archivo
    procesarRegistro<Producto>("productos.bin", idBuscar, [](Producto& p) -> bool {
        
        // Verificamos si existe y está activo
        if (!p.activo) {
            cout << "\n[!] El producto con ID " << p.id << " no existe o esta inactivo." << endl;
            return false; // El template no guarda nada
        }

        cout << "\n--- EDITANDO: " << p.nombre << " ---" << endl;
        cout << "1. Nombre\n2. Descripcion\n3. Precio\n4. Stock Minimo\n0. Cancelar\nSeleccione: ";
        int opcion;
        cin >> opcion;
        vaciarBuffer();

        char tempStr[200];
        int tempVal;
        bool modificado = true;

        // Mostramos la instrucción de nuevo por si el usuario la olvidó al entrar al submenú
        mostrarInstruccionESC();

        switch(opcion) {
            case 1: 
                if (pedirTextoCancelable("Nuevo Nombre", tempStr, 100)) 
                    strcpy(p.nombre, tempStr);
                else 
                    modificado = false;
                break;

            case 2: 
                if (pedirTextoCancelable("Nueva Descripcion", tempStr, 200)) 
                    strcpy(p.descripcion, tempStr);
                else 
                    modificado = false;
                break;

            case 3: 
                do {
                    if (!pedirEnteroCancelable("Nuevo Precio ($)", tempVal)) {
                        modificado = false;
                        break;
                    }
                    if (tempVal < 0) cout << "[!] El precio no puede ser negativo." << endl;
                } while (tempVal < 0);
                
                if (modificado) p.precio = (float)tempVal;
                break;

            case 4: 
                do {
                    if (!pedirEnteroCancelable("Nuevo Stock Minimo", tempVal)) {
                        modificado = false;
                        break;
                    }
                    if (tempVal < 0) cout << "[!] El stock no puede ser negativo." << endl;
                } while (tempVal < 0);
                
                if (modificado) p.stockminimo = tempVal;
                break;

            case 0: 
                modificado = false;
                break;

            default: 
                cout << "[!] Opcion invalida." << endl; 
                modificado = false;
        }

        // 3. DECISIÓN FINAL PARA EL TEMPLATE
        if (modificado) {
            p.fechaUltimaModificacion = time(0);
            return true;  // <--- El template hace el f.write y actualiza
        } else {
            return false; // <--- El template aborta y no toca el archivo
        }
    });

    // Nota: El template procesarRegistro ya incluye los mensajes de "[OK] Datos persistidos..."
    // y "[i] Operacion cancelada...", además del pausarYlimpiarpantalla() al final (si lo configuraste así).
}
void listarProductos() {
    cout << "\n==================================================" << endl;
    cout << "             LISTA DE PRODUCTOS ACTIVOS           " << endl;
    cout << "==================================================" << endl;
    cout << left << setw(10) << "ID" << setw(25) << "Nombre" << "Stock" << endl;
    cout << "--------------------------------------------------" << endl;

    Header h = leerHeader("productos.bin");
    bool encontrados = false;

    for (int i = 0; i < h.cantidadRegistros; i++) {
        Producto p = leerRegistroPorIndice<Producto>("productos.bin", i);
        if (p.activo) {
            int stockActual = calcularStockActual(p);
            cout << left << setw(10) << p.id 
                 << setw(25) << p.nombre << "|"
                 << stockActual << endl;
            encontrados = true;
        }
    }

    if (!encontrados) {
        cout << "\n[i] No hay productos activos para mostrar." << endl;
    }
    
    cout << "==================================================" << endl;
};
void eliminarProducto() {
    listarProductos(); // Mostramos los productos activos para que el usuario sepa qué ID puede eliminar
    int idBuscar;
    cout << "\n==================================================" << endl;
    cout << "             ELIMINAR PRODUCTO (INACTIVAR)        " << endl;
    cout << "==================================================" << endl;
    if (pedirEnteroCancelable("Ingrese el ID del producto a eliminar", idBuscar)) {
        // Usamos el template para marcar como inactivo
        procesarRegistro<Producto>("productos.bin", idBuscar, [](Producto& p) -> bool {
            if (confirmarAccion("¿Confirma que desea eliminar este producto?")) {
                p.activo = false; // <--- ¡Vital! Lo volvemos a la vida
                p.fechaUltimaModificacion = time(0); 
                return true; // El template guarda el cambio
            }
            return false;
        });
    }
}
void restaurarProducto() {
    listarProductosEliminados(); // Mostramos los productos inactivos para que el usuario sepa qué ID puede restaurar
    int idBuscar;
    cout << "\n==================================================" << endl;
    cout << "             RESTAURAR PRODUCTO INACTIVO          " << endl;
    cout << "==================================================" << endl;
    if (pedirEnteroCancelable("Ingrese el ID del producto a restaurar", idBuscar)) {
        // Usamos el template para marcar como activo
        procesarRegistro<Producto>("productos.bin", idBuscar, [](Producto& p) -> bool {
            if (confirmarAccion("¿Confirma que desea restaurar este producto?")) {
                p.activo = true; 
                p.fechaUltimaModificacion = time(0); 
                return true; // El template guarda el cambio
            }
            return false;
        });
    }
}
//------------------------------------------//

//CRUD PROVEEDOR//

void crearProveedor() {
    cout << "\n==================================================" << endl;
    cout << "             NUEVO PROVEEDOR (REGISTRO)           " << endl;
    cout << "==================================================" << endl;
    mostrarInstruccionESC();

    // Llamamos al motor Maestro con ID -1 para indicar un registro NUEVO
    procesarRegistro<Proveedor>("proveedores.bin", -1, [](Proveedor& p) -> bool {
        
        // 1. Inicialización de fecha y ID (El ID ya viene asignado por el template)
        if (!obtenerFechaActual(p.fechaRegistro)) {
            cout << "\n[i] Sugerencia: Verifique la configuracion de fecha y hora en Windows." << endl;
        }

        cout << "Registrando Proveedor ID: " << p.id << endl;
        cout << "--------------------------------------------------" << endl;

        // 2. Captura de Nombre
        if (!pedirTextoCancelable("Nombre del Proveedor", p.nombre, 100)) return false;

        // 3. RIF con validación de duplicados (Usando tu función rifDuplicado)
        do {
            if (!pedirTextoCancelable("RIF (Ej: J-12345678-9)", p.rif, 20)) return false;
            
            if (rifDuplicado(p.rif)) {
                cout << "  [!] Error: Este RIF ya esta registrado por otro proveedor activo.\n";
            } else {
                break; 
            }
        } while (true);

        // 4. Teléfono
        if (!pedirTextoCancelable("Telefono de Contacto", p.telefono, 20)) return false;

        // 5. Email con validación de formato (Usando tu función validarEmail)
        do {
            if (!pedirTextoCancelable("Correo Electronico", p.email, 100)) return false;
            
            if (!validarEmail(p.email)) {
                cout << "  [!] Error: Formato de email invalido (ejemplo@dominio.com).\n";
            } else {
                break;
            }
        } while (true);

        // 6. Dirección
        if (!pedirTextoCancelable("Direccion Fisica", p.direccion, 200)) return false;

        // 7. Inicialización de Auditoría y Metadatos
        p.totalSuministros = 0;
        p.totalPagado = 0;
        p.activo = true;
        p.fechaCreacion = time(0);
        p.fechaUltimaModificacion = time(0);

        // 8. Resumen Visual antes de confirmar
        system("cls");
        cout << "==================================================" << endl;
        cout << "          REVISIÓN DE DATOS DEL PROVEEDOR         " << endl;
        cout << "==================================================" << endl;
        cout << " ID        : " << p.id << endl;
        cout << " NOMBRE    : " << p.nombre << endl;
        cout << " RIF       : " << p.rif << endl;
        cout << " EMAIL     : " << p.email << endl;
        cout << " REGISTRO  : " << p.fechaRegistro << endl;
        cout << "--------------------------------------------------" << endl;

        // 9. Confirmación final: Si retorna true, el Template guarda físicamente
        return confirmarAccion("¿Desea guardar este proveedor en la base de datos?");
    });

    // Nota: No hace falta pausarYlimpiarpantalla() aquí porque el Template 
    // procesarRegistro ya lo hace al finalizar su ejecución.
}
void buscarProveedor() {
    cout << "ingrese 1 para buscar por ID o 2 para buscar por nombre: ";
    int opcion;
    if(!(cin >> opcion) || (opcion != 1 && opcion != 2)) {
        vaciarBuffer();
        cout << "[!] Opcion invalida. Volviendo al menu principal." << endl;
        pausarYlimpiarpantalla();
        return;
    }
    vaciarBuffer();

    if(opcion == 1) {
        int idBuscar;
        if (pedirEnteroCancelable("Ingrese el ID del proveedor a buscar", idBuscar)) {
            mostrarProveedor(idBuscar);
        }
    } else {
        char nombreBusqueda[100];
        if (pedirTextoCancelable("Ingrese el nombre del proveedor a buscar", nombreBusqueda, 100)) {
            buscarProveedorPorNombre(nombreBusqueda);
        }
    }
};
void actualizarProveedor() {
    int idBuscar;
    cout << "\n--- ACTUALIZAR PROVEEDOR ---" << endl;
    mostrarInstruccionESC(); 

    // 1. Buscamos el ID con opción de cancelar (igual que en productos)
    if (!pedirEnteroCancelable("Ingrese el ID del proveedor a actualizar", idBuscar)) return;

    // 2. Usamos el Template para gestionar el archivo de proveedores
    procesarRegistro<Proveedor>("proveedores.bin", idBuscar, [](Proveedor& p) -> bool {
        
        // Verificamos si existe y está activo
        if (!p.activo) {
            cout << "\n[!] El proveedor con ID " << p.id << " no existe o esta inactivo." << endl;
            return false; 
        }

        cout << "\n--- EDITANDO: " << p.nombre << " ---" << endl;
        cout << "1. Nombre\n2. RIF\n3. Telefono\n4. Email\n5. Direccion\n0. Cancelar\nSeleccione: ";
        int opcion;
        cin >> opcion;
        vaciarBuffer();

        char tempStr[200];
        bool modificado = true;

        mostrarInstruccionESC();

        switch(opcion) {
            case 1: 
                if (pedirTextoCancelable("Nuevo Nombre", tempStr, 100)) 
                    strcpy(p.nombre, tempStr);
                else 
                    modificado = false;
                break;

            case 2: 
                // Validamos que el nuevo RIF no sea de otro proveedor
                if (pedirTextoCancelable("Nuevo RIF", tempStr, 20)) {
                    if (!rifDuplicado(tempStr)) {
                        strcpy(p.rif, tempStr);
                    } else {
                        cout << "[!] Error: RIF ya registrado." << endl;
                        modificado = false;
                    }
                } else modificado = false;
                break;

            case 3: 
                if (pedirTextoCancelable("Nuevo Telefono", tempStr, 20)) 
                    strcpy(p.telefono, tempStr);
                else 
                    modificado = false;
                break;

            case 4: 
                if (pedirTextoCancelable("Nuevo Email", tempStr, 100)) {
                    if (validarEmail(tempStr)) {
                        strcpy(p.email, tempStr);
                    } else {
                        cout << "[!] Formato de email invalido." << endl;
                        modificado = false;
                    }
                } else modificado = false;
                break;

            case 5: 
                if (pedirTextoCancelable("Nueva Direccion", tempStr, 200)) 
                    strcpy(p.direccion, tempStr);
                else 
                    modificado = false;
                break;

            case 0: 
                modificado = false;
                break;

            default: 
                cout << "[!] Opcion invalida." << endl; 
                modificado = false;
        }

        // 3. DECISIÓN FINAL
        if (modificado) {
            p.fechaUltimaModificacion = time(0);
            return true;  // El template guarda los cambios
        } else {
            return false; // El template cancela la operación
        }
    });
}
void listarProveedores() {
        cout << "\n==================================================" << endl;
    cout << "             LISTA DE PROVEEDORES ACTIVOS         " << endl;
    cout << "==================================================" << endl;
    cout << left << setw(5) << "ID" << setw(30) << "NOMBRE" << setw(15) << "RIF" << endl;
    cout << "--------------------------------------------------------" << endl;

    Header h = leerHeader("proveedores.bin");
    bool encontrados = false;   
    for (int i = 0; i < h.cantidadRegistros; i++) {
        Proveedor p = leerRegistroPorIndice<Proveedor>("proveedores.bin", i);
        if (p.activo) {
            cout << left << setw(5) << p.id << setw(30) << p.nombre << setw(15) << p.rif << endl;
            encontrados = true;
        }
    }
    if (!encontrados) {
        cout << "No hay proveedores activos." << endl;
    }
        cout << "==================================================" << endl;
    // Lógica para listar todos los proveedores activos, mostrando información relevante como el nombre y el RIF.
};
void eliminarProveedor() {
    listarProveedores(); // Mostramos los activos
    int idBuscar;
    
    cout << "\n==================================================" << endl;
    cout << "             ELIMINAR PROVEEDOR (INACTIVAR)       " << endl;
    cout << "==================================================" << endl;
    
    if (pedirEnteroCancelable("Ingrese el ID del proveedor a eliminar", idBuscar)) {
        
        procesarRegistro<Proveedor>("proveedores.bin", idBuscar, [](Proveedor& p) -> bool {
            if (!p.activo) {
                cout << "\n[!] El proveedor con ID " << p.id << " ya esta inactivo o no existe." << endl;
                return false; 
            }
            
            // LA CORRECCIÓN ESTÁ AQUÍ:
            if (confirmarAccion("¿Confirma que desea eliminar este proveedor?")) {
                p.activo = false; // <--- ¡Vital! Cambiamos el estado a inactivo
                p.fechaUltimaModificacion = time(0); // Actualizamos la fecha
                return true; // Ahora el template guarda el cambio
            }
            return false; // Si el usuario dice "No" en confirmarAccion
        });
    }
}
void restaurarProveedor() {
    listarProveedoresEliminados(); // Asumiendo que tienes esta función creada
    int idBuscar;
    
    cout << "\n==================================================" << endl;
    cout << "             RESTAURAR PROVEEDOR INACTIVO         " << endl;
    cout << "==================================================" << endl;
    
    if (pedirEnteroCancelable("Ingrese el ID del proveedor a restaurar", idBuscar)) {
        
        procesarRegistro<Proveedor>("proveedores.bin", idBuscar, [](Proveedor& p) -> bool {
            if (p.activo) {
                cout << "\n[!] El proveedor con ID " << p.id << " ya esta activo o no existe." << endl;
                return false; 
            }
            
            // LA CORRECCIÓN ESTÁ AQUÍ:
            if (confirmarAccion("¿Confirma que desea restaurar este proveedor?")) {
                p.activo = true; // <--- ¡Vital! Lo volvemos a la vida
                p.fechaUltimaModificacion = time(0); 
                return true; // El template guarda el cambio
            }
            return false;
        });
    }
}
//------------------------------------------//

//CRUD CLIENTE//
void crearCliente() {
    cout << "\n==================================================" << endl;
    cout << "             NUEVO CLIENTE (REGISTRO)             " << endl;
    cout << "==================================================" << endl;
    mostrarInstruccionESC();

    procesarRegistro<Cliente>("clientes.bin", -1, [](Cliente& c) -> bool {
        if (!obtenerFechaActual(c.fechaRegistro)) {
            cout << "\n[i] Sugerencia: Verifique la configuracion de fecha y hora en Windows." << endl;
        }

        cout << "Registrando Cliente ID: " << c.id << endl;
        cout << "--------------------------------------------------" << endl;

        if (!pedirTextoCancelable("Nombre del Cliente", c.nombre, 100)) return false;

        do {
            if (!pedirTextoCancelable("Cedula/RIF (Ej: V-12345678)", c.cedula, 20)) return false;
            
            if (cedulaDuplicada(c.cedula)) {
                cout << "  [!] Error: Esta cedula/RIF ya esta registrada por otro cliente activo.\n";
            } else {
                break; 
            }
        } while (true);

        if (!pedirTextoCancelable("Telefono de Contacto", c.telefono, 20)) return false;

        do{
        if (!pedirTextoCancelable("Correo Electronico", c.email, 100)) return false;
        if (!validarEmail(c.email)) {
            cout << "[!] Formato de email invalido (ejemplo@dominio.com)." << endl;
        }
        } while (!validarEmail(c.email));

        if (!pedirTextoCancelable("Direccion", c.direccion, 200)) return false;

        c.totalComprasRealizadas = 0;
        c.montoTotalGastado = 0;
        c.activo = true;
        c.fechaCreacion = time(0);
        c.fechaUltimaModificacion = time(0);

        system("cls");
        cout << "==================================================" << endl;
        cout << "          REVISIÓN DE DATOS DEL CLIENTE           " << endl;
        cout << "==================================================" << endl;
        cout << " ID        : " << c.id << endl;
        cout << " NOMBRE    : " << c.nombre << endl;
        cout << " CEDULA/RIF: " << c.cedula << endl;
        cout << " EMAIL     : " << c.email << endl;
        cout << " REGISTRO  : " << c.fechaRegistro << endl;
        cout << "--------------------------------------------------" << endl;

        return confirmarAccion("¿Desea guardar este cliente en la base de datos?");
    });
};
void buscarCliente() {
    cout << "ingrese 1 para buscar por ID o 2 para buscar por nombre: ";
    int opcion;
    if(!(cin >> opcion) || (opcion != 1 && opcion != 2)) {
        vaciarBuffer();
        cout << "[!] Opcion invalida. Volviendo al menu principal." << endl;
        pausarYlimpiarpantalla();
        return;
    }
    vaciarBuffer();
    if(opcion == 1) {
        int idBuscar;
        if (pedirEnteroCancelable("Ingrese el ID del cliente a buscar", idBuscar)) {
            mostrarcliente(idBuscar);
        }
    } else {
        char nombreBusqueda[100];
        if (pedirTextoCancelable("Ingrese el nombre del cliente a buscar", nombreBusqueda, 100)) {
            buscarClientePorNombre(nombreBusqueda);
        }
    }
};
void actualizarCliente() {
    int idBuscar;
    cout << "\n--- ACTUALIZAR CLIENTE ---" << endl;
    mostrarInstruccionESC(); 

    // 1. Buscamos el ID con opción de cancelar
    if (!pedirEnteroCancelable("Ingrese el ID del cliente a actualizar", idBuscar)) return;

    // 2. Usamos el Template para gestionar el archivo de clientes
    procesarRegistro<Cliente>("clientes.bin", idBuscar, [](Cliente& c) -> bool {
        
        if (!c.activo) {
            cout << "\n[!] El cliente con ID " << c.id << " no existe o esta inactivo." << endl;
            return false; 
        }

        cout << "\n--- EDITANDO: " << c.nombre << " ---" << endl;
        cout << "1. Nombre\n2. Cedula/RIF\n3. Telefono\n4. Email\n5. Direccion\n0. Cancelar\nSeleccione: ";
        int opcion;
        cin >> opcion;
        vaciarBuffer();

        char tempStr[200];
        mostrarInstruccionESC();

        // Usamos el switch para decidir qué campo modificar
        switch(opcion) {
            case 1: // Nombre
                if (pedirTextoCancelable("Nuevo Nombre", tempStr, 100)) {
                    strcpy(c.nombre, tempStr);
                    break; 
                }
                return false;

            case 2: // Cédula/RIF con validación de duplicados
                if (pedirTextoCancelable("Nueva Cedula/RIF", tempStr, 20)) {
                    if (!cedulaDuplicada(tempStr)) {
                        strcpy(c.cedula, tempStr);
                        break;
                    } else {
                        cout << "[!] Error: Cedula/RIF ya registrado." << endl;
                    }
                }
                return false;

            case 3: // Teléfono
                if (pedirTextoCancelable("Nuevo Telefono", tempStr, 20)) {
                    strcpy(c.telefono, tempStr);
                    break;
                }
                return false;

            case 4: // Email con validación de formato
                if (pedirTextoCancelable("Nuevo Email", tempStr, 100)) {
                    if (validarEmail(tempStr)) {
                        strcpy(c.email, tempStr);
                        break;
                    } else {
                        cout << "[!] Formato de email invalido." << endl;
                    }
                }
                return false;

            case 5: // Dirección
                if (pedirTextoCancelable("Nueva Direccion", tempStr, 200)) {
                    strcpy(c.direccion, tempStr);
                    break;
                }
                return false;

            case 0: // Cancelar
            default:
                return false;
        }
        // Si el código llegó aquí, es porque entró en un 'break' (modificación exitosa)
        c.fechaUltimaModificacion = time(0);
        return confirmarAccion("¿Desea guardar los cambios en el cliente?");
    });
}
void listarClientes() {
    cout << "\n==================================================" << endl;
    cout << "             LISTA DE CLIENTES ACTIVOS         " << endl;
    cout << "==================================================" << endl;
    cout << left << setw(5) << "ID" << setw(30) << "NOMBRE" << setw(15) << "CEDULA" << endl;
    cout << "--------------------------------------------------------" << endl;

    Header h = leerHeader("clientes.bin");
    bool encontrados = false;   
    for (int i = 0; i < h.cantidadRegistros; i++) {
        Cliente c = leerRegistroPorIndice<Cliente>("clientes.bin", i);
        if (c.activo) {
            cout << left << setw(5) << c.id << setw(30) << c.nombre << setw(15) << c.cedula << endl;
            encontrados = true;
        }
    }
    if (!encontrados) {
        cout << "No hay clientes activos." << endl;
    }
        cout << "==================================================" << endl;
};
void eliminarCliente() {
    listarClientes();
    int idbuscar;
    cout << "\n==================================================" << endl;
    cout << "             ELIMINAR CLIENTE (INACTIVAR)       " << endl;
    cout << "==================================================" << endl;
    if(pedirEnteroCancelable("Ingrese el id del cliente a eliminar", idbuscar)){
        procesarRegistro<Cliente>("clientes.bin", idbuscar, [](Cliente& c) -> bool {
            if (confirmarAccion("¿Confirma que desea restaurar este Cliente?")) {
                c.activo = false; 
                c.fechaUltimaModificacion = time(0); 
                return true; // El template guarda el cambio
            }
            return false;
        });
    }
}
void restaurarCliente() {
    listarClientesEliminados(); // Asumiendo que tienes esta función creada
    int idBuscar;
    
    cout << "\n==================================================" << endl;
    cout << "             RESTAURAR CLIENTE INACTIVO         " << endl;
    cout << "==================================================" << endl;
    
    if (pedirEnteroCancelable("Ingrese el ID del cliente a restaurar", idBuscar)) {
        
        procesarRegistro<Cliente>("clientes.bin", idBuscar, [](Cliente& c) -> bool {
            if (c.activo) {
                cout << "\n[!] El proveedor con ID " << c.id << " ya esta activo o no existe." << endl;
                return false; 
            }
            
            // LA CORRECCIÓN ESTÁ AQUÍ:
            if (confirmarAccion("¿Confirma que desea restaurar este proveedor?")) {
                c.activo = true; // <--- ¡Vital! Lo volvemos a la vida
                c.fechaUltimaModificacion = time(0); 
                return true; // El template guarda el cambio
            }
            return false;
        });
    }
};
//------------------------------------------//
//CRUD TRANSACCION//
void registrarCompra() {
    Transaccion t;
    memset(&t, 0, sizeof(Transaccion));
    t.tipo = 'C';
    t.activo = true;
    obtenerFechaActual(t.fecha);

    mostrarInstruccionESC();
    listarProveedores();
    
    if (!pedirEnteroCancelable("Ingrese ID del Proveedor", t.idRelacionado)) return;
    
    if (buscarPosicion<Proveedor>("proveedores.bin", t.idRelacionado) == -1) {
        cout << "[!] Proveedor no encontrado o inactivo." << endl;
        return;
    }

    // --- MEJORA APLICADA ---
    // Ahora solo mostramos lo que ese proveedor nos puede vender
    mostrarInstruccionESC();
    mostrarProductos(t.idRelacionado); 

    while (t.cantidaditems < 30) {
        int idProd;
        cout << "\n(Comprando al Proveedor #" << t.idRelacionado << ")" << endl;
        if (!pedirEnteroCancelable("ID Producto a comprar (ESC para finalizar)", idProd)) break;

        int pos = buscarPosicion<Producto>("productos.bin", idProd);
        if (pos == -1) { 
            cout << "[!] El producto no existe." << endl; 
            continue; 
        }

        Producto p = leerRegistroPorIndice<Producto>("productos.bin", pos);

        // Validación de seguridad adicional
        if (p.idProveedor != t.idRelacionado) {
            cout << "[!] ERROR: Este producto no pertenece al proveedor seleccionado." << endl;
            continue;
        }

        cout << " -> " << p.nombre << " | Costo Unitario: $" << p.precio << endl;

        int cant;
        if (!pedirEnteroCancelable("Cantidad a comprar", cant) || cant <= 0) continue;

        float costoArticulo = p.precio; 
        float subtotal = cant * costoArticulo;
        if (!hayDineroEnCaja(t.total + subtotal)) {
            cout << "[!] ERROR: Fondos insuficientes en caja." << endl;
            continue;
        }

        t.detalles[t.cantidaditems] = { idProd, cant, costoArticulo };
        t.total += subtotal;
        t.cantidaditems++;
        cout << "[+] Agregado. Total factura: $" << t.total << endl;
    }

    if (t.cantidaditems > 0 && confirmarAccion("¿Confirmar Compra?")) {
        procesarRegistro<Transaccion>("transacciones.bin", -1, [&](Transaccion& reg) -> bool {
            int idGenerado = reg.id;
            memcpy(&reg, &t, sizeof(Transaccion));
            reg.id = idGenerado; 
            reg.fechaCreacion = time(0);
            return true;
        });

        for (int i = 0; i < t.cantidaditems; i++) {
            actualizarAcumuladoresProducto(t.detalles[i].idProducto, t.detalles[i].cantidad, 'C');
        }
        actualizarFinanzasTienda(t.total, 'C');
        cout << "[OK] Compra procesada." << endl;
    }
}

void registrarVenta() {
    Transaccion t;
    memset(&t, 0, sizeof(Transaccion)); 
    t.tipo = 'V';
    t.activo = true;
    obtenerFechaActual(t.fecha);
    mostrarInstruccionESC();
    listarClientes();
    if (!pedirEnteroCancelable("Ingrese ID del Cliente", t.idRelacionado)) return;
    if (buscarPosicion<Cliente>("clientes.bin", t.idRelacionado) == -1) {
        cout << "[!] Cliente no encontrado o inactivo." << endl;
        return;
    }
    mostrarInstruccionESC();
    listarProductos();
    while (t.cantidaditems < 30) {
        int idProd;
        if (!pedirEnteroCancelable("ID Producto (0 para finalizar)", idProd) || idProd == 0) break;

        int pos = buscarPosicion<Producto>("productos.bin", idProd);
        if (pos == -1) { cout << "[!] El producto no existe." << endl; continue; }

        Producto p = leerRegistroPorIndice<Producto>("productos.bin", pos);
        
        // Calculamos stock real considerando lo que ya pusimos en esta misma factura
        int yaVendidoEnEstaFactura = 0;
        for(int i = 0; i < t.cantidaditems; i++) {
            if(t.detalles[i].idProducto == idProd) 
                yaVendidoEnEstaFactura += t.detalles[i].cantidad;
        }

        int stockDisponible = calcularStockActual(p) - yaVendidoEnEstaFactura;
        cout << " -> " << p.nombre << " | Stock disponible: " << stockDisponible << endl;

        if (stockDisponible <= 0) {
            cout << "[!] Sin existencias para este producto." << endl;
            continue;
        }

        int cant;
        if (!pedirEnteroCancelable("Cantidad a vender", cant) || cant <= 0) continue;

        if (cant > stockDisponible) {
            cout << "[!] No puedes vender mas de lo que hay en stock." << endl;
            continue;
        }

        t.detalles[t.cantidaditems] = { idProd, cant, p.precio };
        t.total += (cant * p.precio);
        t.cantidaditems++;
        cout << "[+] Item agregado. Subtotal: $" << t.total << endl;
    }

    if (t.cantidaditems > 0 && confirmarAccion("¿Confirmar y facturar venta?")) {
        procesarRegistro<Transaccion>("transacciones.bin", -1, [&](Transaccion& reg) -> bool {
            int idGenerado = reg.id;
            memcpy(&reg, &t, sizeof(Transaccion));
            reg.id = idGenerado;
            reg.fechaCreacion = time(0);
            return true;
        });

        for (int i = 0; i < t.cantidaditems; i++) {
            actualizarAcumuladoresProducto(t.detalles[i].idProducto, t.detalles[i].cantidad, 'V');
        }
        actualizarFinanzasTienda(t.total, 'V');
        cout << "[EXITO] Venta procesada correctamente." << endl;
    }
}

void buscarTransaccion() {
    int idBuscar;
    cout << "\n--- BUSQUEDA DE TRANSACCION ---" << endl;
    
    if (!pedirEnteroCancelable("Ingrese el ID de la factura a visualizar", idBuscar)) {
        return;
    }

    int idx = buscarPosicion<Transaccion>("transacciones.bin", idBuscar);

    if (idx == -1) {
        cout << "[!] No se encontro ninguna transaccion con el ID: " << idBuscar << endl;
        return;
    }

    // Si existe, llamamos a tu funcion de mostrar detalle
    mostrarTransaccion(idBuscar);
}
void listarTransacciones() {
    Header h = leerHeader("transacciones.bin");
    char nombreEntidad[100];
    bool hayRegistros = false;

    cout << "\n================================================================================" << endl;
    cout << "                         HISTORIAL DE TRANSACCIONES ACTIVAS" << endl;
    cout << "================================================================================" << endl;
    cout << left << setw(6) << "ID" 
         << setw(10) << "TIPO" 
         << setw(25) << "ENTIDAD (CLIENTE/PROV)" 
         << setw(15) << "FECHA" 
         << "TOTAL" << endl;
    cout << "--------------------------------------------------------------------------------" << endl;

    for (int i = 0; i < h.cantidadRegistros; i++) {
        Transaccion t = leerRegistroPorIndice<Transaccion>("transacciones.bin", i);
        
        // Solo listamos las que no han sido anuladas
        if (t.activo) {
            hayRegistros = true;

            // Identificar tipo y obtener nombre de la entidad sin usar ternarios
            if (t.tipo == 'V') {
                obtenerNombrePorID<Cliente>("clientes.bin", t.idRelacionado, nombreEntidad);
                cout << left << setw(6) << t.id << setw(10) << "VENTA";
            } else {
                obtenerNombrePorID<Proveedor>("proveedores.bin", t.idRelacionado, nombreEntidad);
                cout << left << setw(6) << t.id << setw(10) << "COMPRA";
            }

            // Continuar con el resto de la fila
            cout << left << setw(25) << nombreEntidad
                 << setw(15) << t.fecha
                 << "$ " << fixed << setprecision(2) << t.total << endl;
        }
    }

    if (!hayRegistros) {
        cout << "\n[i] No existen transacciones activas en el sistema." << endl;
    }
    cout << "================================================================================" << endl;
}
void cancelarTransacciones() {
    // 1. Mostrar transacciones activas primero
    Header h = leerHeader("transacciones.bin");
    char nombreEntidad[100];
    bool hayActivas = false;

    cout << "\n====================================================" << endl;
    cout << "           LISTADO DE TRANSACCIONES ACTIVAS" << endl;
    cout << "====================================================" << endl;
    cout << left << setw(6) << "ID" << setw(10) << "TIPO" << setw(20) << "ENTIDAD" << "TOTAL" << endl;
    cout << "----------------------------------------------------" << endl;

    for (int i = 0; i < h.cantidadRegistros; i++) {
        Transaccion t = leerRegistroPorIndice<Transaccion>("transacciones.bin", i);
        if (t.activo) {
            hayActivas = true;
            if (t.tipo == 'V') {
                obtenerNombrePorID<Cliente>("clientes.bin", t.idRelacionado, nombreEntidad);
                cout << left << setw(6) << t.id << setw(10) << "VENTA";
            } else {
                obtenerNombrePorID<Proveedor>("proveedores.bin", t.idRelacionado, nombreEntidad);
                cout << left << setw(6) << t.id << setw(10) << "COMPRA";
            }
            cout << left << setw(20) << nombreEntidad << "$ " << t.total << endl;
        }
    }

    if (!hayActivas) {
        cout << "[!] No hay transacciones activas para anular." << endl;
        return;
    }

   int idAnular;
    cout << "\n----------------------------------------------------" << endl;
    if (!pedirEnteroCancelable("Ingrese el ID de la factura a ANULAR", idAnular)) return;

    int pos = buscarPosicion<Transaccion>("transacciones.bin", idAnular);
    if (pos == -1) {
        cout << "[!] Transaccion no encontrada." << endl;
        return;
    }

    Tienda miTienda;
    fstream fTienda("tienda.bin", ios::binary | ios::in | ios::out);
    fTienda.read(reinterpret_cast<char*>(&miTienda), sizeof(Tienda));

    Transaccion t;
    memset(&t, 0, sizeof(Transaccion)); // 1. Limpiamos la estructura de basura
    fstream f("transacciones.bin", ios::binary | ios::in);
    f.seekg(sizeof(Header) + (pos * sizeof(Transaccion)), ios::beg);
    f.read(reinterpret_cast<char*>(&t), sizeof(Transaccion));
    f.close();

    if (!t.activo) {
        cout << "[!] Esta factura ya estaba anulada." << endl;
        return;
    }

    // --- VALIDACIONES DE SEGURIDAD ---
    if (t.tipo == 'C') { 
        // 2. Usamos t.cantidaditems con cuidado
        for (int i = 0; i < t.cantidaditems; i++) {
            int idProd = t.detalles[i].idProducto;
            int cantReq = t.detalles[i].cantidad;
            
            // Si el ID es 0 o inválido, saltamos (evita procesar basura)
            if (idProd <= 0) continue; 

            int posP = buscarPosicion<Producto>("productos.bin", idProd);
            if (posP == -1) continue; // Si no existe el producto, saltar

            Producto p;
            fstream fp("productos.bin", ios::binary | ios::in);
            fp.seekg(sizeof(Header) + (posP * sizeof(Producto)), ios::beg);
            fp.read(reinterpret_cast<char*>(&p), sizeof(Producto));
            fp.close();

            if (p.existencia < cantReq) {
                cout << "[!] ERROR: No hay stock suficiente de '" << p.nombre 
                     << "' (ID: " << idProd << ") para devolver la compra." << endl;
                return;
            }
        }
        // 2. Validar Finanzas
        if (miTienda.egresosTotales < t.total) {
            cout << "[!] ERROR: Balance de egresos insuficiente para revertir." << endl;
            return;
        }
    } else { 
        // Validar Finanzas para Venta
        if (miTienda.ingresosTotales < t.total) {
            cout << "[!] ERROR: Balance de ingresos insuficiente para revertir." << endl;
            return;
        }
    }

    // 3. Proceder a la anulacion
    procesarRegistro<Transaccion>("transacciones.bin", idAnular, [&](Transaccion& reg) -> bool {
        reg.activo = false;
        char tipoInverso = (reg.tipo == 'V') ? 'C' : 'V';

        // Actualizar stock de todos los items
        for (int i = 0; i < reg.cantidaditems; i++) {
            actualizarAcumuladoresProducto(reg.detalles[i].idProducto, reg.detalles[i].cantidad, tipoInverso);
        }

        // Actualizar finanzas de la tienda
        if (reg.tipo == 'V') {
            miTienda.ingresosTotales -= reg.total;
        } else {
            miTienda.egresosTotales -= reg.total;
        }

        cout << "\n[OK] Transaccion " << idAnular << " anulada y stock/saldos actualizados." << endl;
        return true; 
    });
}

void restaurarTransacciones() {
    // 1. Mostrar las anuladas (Tu código de listado está perfecto)
    Header h = leerHeader("transacciones.bin");
    char nombreEntidad[100];
    bool hayAnuladas = false;

    cout << "\n====================================================" << endl;
    cout << "           LISTADO DE TRANSACCIONES ANULADAS" << endl;
    cout << "====================================================" << endl;
    cout << left << setw(6) << "ID" << setw(10) << "TIPO" << setw(20) << "ENTIDAD" << "TOTAL" << endl;
    cout << "----------------------------------------------------" << endl;

    for (int i = 0; i < h.cantidadRegistros; i++) {
        Transaccion t = leerRegistroPorIndice<Transaccion>("transacciones.bin", i);
        if (!t.activo) {
            hayAnuladas = true;
            if (t.tipo == 'V') {
                obtenerNombrePorID<Cliente>("clientes.bin", t.idRelacionado, nombreEntidad);
                cout << left << setw(6) << t.id << setw(10) << "VENTA";
            } else {
                obtenerNombrePorID<Proveedor>("proveedores.bin", t.idRelacionado, nombreEntidad);
                cout << left << setw(6) << t.id << setw(10) << "COMPRA";
            }
            cout << left << setw(20) << nombreEntidad << "$ " << t.total << endl;
        }
    }

    if (!hayAnuladas) {
        cout << "[!] No hay transacciones anuladas para restaurar." << endl;
        return;
    }

    int idRestaurar;
    if (!pedirEnteroCancelable("Ingrese el ID de la factura a RESTAURAR", idRestaurar)) return;
    Transaccion t;
    bool encontrada = false;
    Header hRest = leerHeader("transacciones.bin");
    
    ifstream f("transacciones.bin", ios::binary);
    f.seekg(sizeof(Header), ios::beg); // Saltamos el header

    for (int i = 0; i < hRest.cantidadRegistros; i++) {
        f.read(reinterpret_cast<char*>(&t), sizeof(Transaccion));
        if (t.id == idRestaurar) {
            encontrada = true;
            break;
        }
    }
    f.close();

    if (!encontrada) {
        cout << "[!] Transaccion no encontrada." << endl;
        return;
    }

    if (t.activo) {
        cout << "[!] Esta factura ya esta activa." << endl;
        return;
    }

    // --- VALIDACIÓN DE STOCK (SI ES VENTA) ---
    if (t.tipo == 'V') {
        for (int i = 0; i < t.cantidaditems; i++) {
            int posP = buscarPosicion<Producto>("productos.bin", t.detalles[i].idProducto);
            Producto p;
            fstream fp("productos.bin", ios::binary | ios::in);
            fp.seekg(sizeof(Header) + (posP * sizeof(Producto)), ios::beg);
            fp.read(reinterpret_cast<char*>(&p), sizeof(Producto));
            fp.close();

            if (p.existencia < t.detalles[i].cantidad) {
                cout << "[!] ABORTADO: No hay stock de '" << p.nombre << "' para restaurar." << endl;
                return; 
            }
        }
    }

    procesarRegistro<Transaccion>("transacciones.bin", idRestaurar, [&](Transaccion& reg) -> bool {
        
        // 1. Abrir tienda para actualizar finanzas
        Tienda miTiendaActual;
        fstream fTienda("tienda.bin", ios::binary | ios::in | ios::out);
        fTienda.read(reinterpret_cast<char*>(&miTiendaActual), sizeof(Tienda));

        // 2. Cambiar estado
        reg.activo = true; 

        // 3. Restaurar stock de cada producto
        for (int i = 0; i < reg.cantidaditems; i++) {
            actualizarAcumuladoresProducto(reg.detalles[i].idProducto, reg.detalles[i].cantidad, reg.tipo);
        }

        // 4. Actualizar finanzas
        if (reg.tipo == 'V') {
            miTiendaActual.ingresosTotales += reg.total;
            miTiendaActual.caja += reg.total;
        } else {
            miTiendaActual.egresosTotales += reg.total;
            miTiendaActual.caja -= reg.total;
        }

        // 5. Guardar la tienda y CERRARLA antes de salir de la lambda
        fTienda.seekp(0, ios::beg);
        fTienda.write(reinterpret_cast<char*>(&miTiendaActual), sizeof(Tienda));
        fTienda.close();

        cout << "\n[OK] Factura " << idRestaurar << " restaurada y valores actualizados." << endl;
        return true; // Retorna true para que procesarRegistro guarde la Transaccion
    });
}
//-----------------------------------------------------------------------------------------//

void verificarIntegridadReferencial() {
    cout << "\n==================================================" << endl;
    cout << "     DIAGNOSTICO DE INTEGRIDAD DE DATOS           " << endl;
    cout << "==================================================" << endl;
    int errores = 0;
    // 1. Productos -> Proveedores
    cout << "[i] Verificando: Productos -> Proveedores..." << endl;
    Header hProd = leerHeader("productos.bin");
    for (int i = 0; i < hProd.cantidadRegistros; i++) {
        Producto p = leerRegistroPorIndice<Producto>("productos.bin", i);
        if (p.activo && !validarExistencia<Proveedor>("proveedores.bin", p.idProveedor)) {
            cout << "  [!] Producto ID " << p.id << " apunta a Proveedor inexistente: " << p.idProveedor << endl;
            errores++;
        }
    }
    // 2. Transacciones -> Entidades
    cout << "[i] Verificando: Transacciones -> Entidades..." << endl;
    Header hTrans = leerHeader("transacciones.bin");
    for (int i = 0; i < hTrans.cantidadRegistros; i++) {
        Transaccion t = leerRegistroPorIndice<Transaccion>("transacciones.bin", i);
        if (t.activo) {
            bool entidadOk = (t.tipo == 'V') ? validarExistencia<Cliente>("clientes.bin", t.idRelacionado) 
                                            : validarExistencia<Proveedor>("proveedores.bin", t.idRelacionado);
            if (!entidadOk) {
                cout << "  [!] Factura #" << t.id << " vinculada a Entidad inexistente: " << t.idRelacionado << endl;
                errores++;
            }
        }
    }

    if (errores == 0) cout << "[OK] Integridad al 100%. No hay referencias rotas." << endl;
    else cout << "[!] Se detectaron " << errores << " inconsistencias." << endl;
    
    pausarYlimpiarpantalla();
}
void generarReportes() {
    if(!validarExistencia<Producto>("productos.bin") && !validarExistencia<Transaccion>("transacciones.bin")) {
        cout << "\n[!] No hay datos suficientes para generar reportes analíticos." << endl;
        cout << "[!] Asegurese de tener productos registrados y transacciones realizadas." << endl;
        pausarYlimpiarpantalla();
        return;
    }
    int opcion;
    do {
        system("cls"); // Limpieza de pantalla para el menú de reportes
        cout << "╔═══════════════════════════════════════════╗" << endl;
        cout << "║           REPORTES ANALÍTICOS             ║" << endl;
        cout << "╚═══════════════════════════════════════════╝" << endl;
        cout << "1. Reporte de Productos con Stock Critico" << endl;
        cout << "2. Historial de Compras por Cliente" << endl;
        cout << "0. Volver" << endl;
        opcion = leerDato<int>("Seleccione un reporte: ");

        if (opcion == 1) {
            // --- REPORTE 1: STOCK CRÍTICO ---
            cout << "\n--- PRODUCTOS CON STOCK CRITICO O AGOTADO ---" << endl;
            cout << left << setw(8) << "ID" << setw(30) << "PRODUCTO" << setw(15) << "STOCK ACTUAL" << "STOCK MINIMO" << endl;
            cout << "----------------------------------------------------------------------" << endl;

            Header h = leerHeader("productos.bin");
            bool hayCriticos = false;

            for (int i = 0; i < h.cantidadRegistros; i++) {

                Producto p = leerRegistroPorIndice<Producto>("productos.bin", i);
                
                if (p.activo) {
                    // Usamos tu función de cálculo de stock
                    int stockActual = calcularStockActual(p);
                    if (stockActual <= p.stockminimo) {
                        cout << left << setw(8) << p.id 
                             << setw(30) << p.nombre 
                             << setw(15) << stockActual 
                             << p.stockminimo << " Unid." << endl;
                        hayCriticos = true;
                    }
                }
            }

            if (!hayCriticos) cout << "\n[OK] Todos los productos tienen un nivel de stock saludable." << endl;
            pausarYlimpiarpantalla();

        } else if (opcion == 2) {
            // --- REPORTE 2: HISTORIAL DE CLIENTE ---
            int idCliente;
            if (!pedirEnteroCancelable("\nIngrese ID del Cliente para ver su historial", idCliente)) continue;

            // Validamos existencia con tu template
            if (!validarExistencia<Cliente>("clientes.bin", idCliente)) {
                cout << "[!] Cliente no encontrado o inactivo." << endl;
            } else {
                char nombreCliente[100];
                obtenerNombrePorID<Cliente>("clientes.bin", idCliente, nombreCliente);
                
                cout << "\n==================================================" << endl;
                cout << "  HISTORIAL DE COMPRAS: " << nombreCliente << " (ID: " << idCliente << ")" << endl;
                cout << "==================================================" << endl;
                
                Header ht = leerHeader("transacciones.bin");
                bool tieneCompras = false;
                
                for (int i = 0; i < ht.cantidadRegistros; i++) {
                    // Usamos tu template de lectura por índice para transacciones
                    Transaccion t = leerRegistroPorIndice<Transaccion>("transacciones.bin", i);
                    
                    // Filtramos: Solo ventas ('V'), del cliente específico y que estén activas
                    if (t.activo && t.tipo == 'V' && t.idRelacionado == idCliente) {
                        tieneCompras = true;
                        cout << "\n> FECHA: " << t.fecha << " | FACTURA #" << t.id << endl;
                        cout << "  TOTAL: $" << fixed << setprecision(2) << t.total << endl;
                        cout << "  DETALLE DE PRODUCTOS:" << endl;

                        // Listamos los items de esa factura (usando t.cantidaditems como está en tu struct)
                        for(int j = 0; j < t.cantidaditems; j++) {
                            char nombreProd[100];
                            obtenerNombrePorID<Producto>("productos.bin", t.detalles[j].idProducto, nombreProd);
                            
                            cout << "    - " << left << setw(5) << t.detalles[j].cantidad << " x " 
                                 << setw(25) << nombreProd 
                                 << " ($" << t.detalles[j].precioUnitario << " c/u)" << endl;
                        }
                        cout << "  ------------------------------------------------" << endl;
                    }
                }
                if(!tieneCompras) cout << "\n[i] Este cliente aun no ha realizado compras registradas." << endl;
            }
            pausarYlimpiarpantalla();
        }
        
    } while (opcion != 0);
}

void crearBackup() {
    system("mkdir backups 2> nul");
    
    time_t t = time(0);
    struct tm * now = localtime(&t);
    char prefijo[100];
    sprintf(prefijo, "backups/%04d%02d%02d_%02d%02d_", 
            now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min);

    const char* archivos[] = {"tienda.bin", "productos.bin", "proveedores.bin", "clientes.bin", "transacciones.bin"};
    int cont = 0;

    cout << "\n>>> INICIANDO RESPALDO DE SEGURIDAD <<<" << endl;
    for (const char* nombre : archivos) {
        char destino[200];
        sprintf(destino, "%s%s", prefijo, nombre);

        ifstream src(nombre, ios::binary);
        if (src) {
            ofstream dst(destino, ios::binary);
            dst << src.rdbuf(); // Copia bit a bit rápida
            cout << "[OK] Respaldado: " << nombre << endl;
            cont++;
        }
    }
    cout << "---------------------------------------" << endl;
    cout << "[i] Backup finalizado. " << cont << " archivos asegurados." << endl;
    pausarYlimpiarpantalla();
}

int main() {
    // Configuración inicial
    SetConsoleOutputCP(CP_UTF8);
    
    // 1. INICIALIZACIÓN (Crea archivos con Headers si no existen)
    inicializartienda(); 
    crearArchivoBinario("productos.bin"); 
    crearArchivoBinario("proveedores.bin");
    crearArchivoBinario("clientes.bin");
    crearArchivoBinario("transacciones.bin");
    
    // 2. CARGA DE DATOS DE LA TIENDA
    Tienda miTienda;
    ifstream fTienda("tienda.bin", ios::binary);
    if (fTienda) {
        fTienda.read(reinterpret_cast<char*>(&miTienda), sizeof(Tienda));
        fTienda.close();
    }

    bool ejecutando = true;
    int opcion, subOp;

    do {
        system("cls");
        cout << "╔═══════════════════════════════════════════╗" << endl;
        cout << "║      SISTEMA DE GESTIÓN DE INVENTARIO     ║" << endl;
        cout << "║ Tienda: " << left << setw(33) << miTienda.nombre << " ║" << endl;
        cout << "║ RIF:    " << left << setw(33) << miTienda.rif << " ║" << endl;
        cout << "╚═══════════════════════════════════════════╝" << endl;
        cout << "1. Gestión de Productos" << endl;
        cout << "2. Gestión de Proveedores" << endl;
        cout << "3. Gestión de Clientes" << endl;
        cout << "4. Gestión de Transacciones" << endl;
        cout << "5. Verificar Integridad Referencial" << endl;
        cout << "6. Reportes Analíticos" << endl;
        cout << "7. Crear Backup Manual" << endl;
        cout << "8. Salir" << endl;
        // Validación de entrada para el menú principal
        opcion = leerDato<int>("Seleccione una opción: ");
         
        switch (opcion) {
            case 1: // PRODUCTOS
                do {
                    subOp = submenu_producto();
                    switch(subOp) {
                        case 1: if(validarExistencia<Proveedor>("proveedores.bin")) crearProducto(); else cout << "[!] Error: No hay proveedores.\n"; break;
                        case 2: if(validarExistencia<Producto>("productos.bin")) buscarProducto(); else cout << "[!] No hay productos.\n"; break;
                        case 3: if(validarExistencia<Producto>("productos.bin")) actualizarProducto(); else cout << "[!] No hay productos.\n"; break;
                        case 4: if(validarExistencia<Producto>("productos.bin")) listarProductos(); else cout << "[!] No hay productos.\n"; break;
                        case 5: if(validarExistencia<Producto>("productos.bin")) eliminarProducto(); else cout << "[!] No hay productos.\n"; break;
                        case 6: if(validarExistencia<Producto>("productos.bin")) restaurarProducto(); else cout << "[!] No hay productos.\n"; break;
                    }
                    if(subOp != 0) pausarYlimpiarpantalla();
                } while(subOp != 0);
                break;

            case 2: // PROVEEDORES
                do {
                    subOp = submenu_proveedor();
                    switch(subOp) {
                        case 1: crearProveedor(); break;
                        case 2: if(validarExistencia<Proveedor>("proveedores.bin")) buscarProveedor(); else cout << "[!] No hay proveedores.\n"; break;
                        case 3: if(validarExistencia<Proveedor>("proveedores.bin")) actualizarProveedor(); else cout << "[!] No hay proveedores.\n"; break;
                        case 4: if(validarExistencia<Proveedor>("proveedores.bin")) listarProveedores(); else cout << "[!] No hay proveedores.\n"; break;
                        case 5: if(validarExistencia<Proveedor>("proveedores.bin")) eliminarProveedor(); else cout << "[!] No hay proveedores.\n"; break;
                        case 6: if(validarExistencia<Proveedor>("proveedores.bin")) restaurarProveedor(); else cout << "[!] No hay proveedores.\n"; break;
                    }
                    if(subOp != 0) pausarYlimpiarpantalla();
                } while(subOp != 0);
                break;

            case 3: // CLIENTES
                do {
                    subOp = submenu_cliente();
                    switch(subOp) {
                        case 1: crearCliente(); break;
                        case 2: if(validarExistencia<Cliente>("clientes.bin")) buscarCliente(); else cout << "[!] No hay clientes.\n"; break;
                        case 3: if(validarExistencia<Cliente>("clientes.bin")) actualizarCliente(); else cout << "[!] No hay clientes.\n"; break;
                        case 4: if(validarExistencia<Cliente>("clientes.bin")) listarClientes(); else cout << "[!] No hay clientes.\n"; break;
                        case 5: if(validarExistencia<Cliente>("clientes.bin")) eliminarCliente(); else cout << "[!] No hay clientes.\n"; break;
                        case 6: if(validarExistencia<Cliente>("clientes.bin")) restaurarCliente(); else cout << "[!] No hay clientes.\n"; break;
                    }
                    if(subOp != 0) pausarYlimpiarpantalla();
                } while(subOp != 0);
                break;

            case 4: // TRANSACCIONES
                do {
                    subOp = submenu_transaccion();
                    switch(subOp) {
                        case 1: if(validarExistencia<Producto>("productos.bin")) registrarCompra(); else cout << "[!] Registre productos primero.\n"; break;
                        case 2: if(validarExistencia<Producto>("productos.bin") && validarExistencia<Cliente>("clientes.bin")) registrarVenta(); else cout << "[!] Faltan Clientes o Productos.\n"; break;
                        case 3: if(validarExistencia<Transaccion>("transacciones.bin")) buscarTransaccion(); else cout << "[!] No hay facturas.\n"; break;
                        case 4: if(validarExistencia<Transaccion>("transacciones.bin")) listarTransacciones(); else cout << "[!] No hay facturas.\n"; break;
                        case 5: if(validarExistencia<Transaccion>("transacciones.bin")) cancelarTransacciones(); else cout << "[!] No hay facturas.\n"; break;
                        case 6: if(validarExistencia<Transaccion>("transacciones.bin")) restaurarTransacciones(); else cout << "[!] No hay facturas.\n"; break;
                    }
                    if(subOp != 0) pausarYlimpiarpantalla();
                } while(subOp != 0);
                break;
            case 5: verificarIntegridadReferencial(); pausarYlimpiarpantalla(); break;
            case 6: generarReportes(); pausarYlimpiarpantalla(); break;
            case 7: crearBackup(); pausarYlimpiarpantalla(); break;
            case 8:
                cout << "Cerrando sistema de " << miTienda.nombre << "..." << endl;
                ejecutando = false;
                break;

            default:
                cout << "[!] Opción fuera de rango." << endl;
                pausarYlimpiarpantalla();
        }
    } while (ejecutando);

    return 0;
}
