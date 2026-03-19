#include <iostream>
#include <iomanip>
#include <limits>
#include <string.h>
#include <windows.h>
#include <cstring>
#include <fstream>
#include <conio.h>
using namespace std;   

/*# REPÚBLICA BOLIVARIANA DE VENEZUELA
## UNIVERSIDAD RAFAEL URDANETA
## FACULTAD DE INGENIERÍA
## ESCUELA DE INGENIERÍA DE COMPUTACIÓN

**ASIGNATURA:** PROGRAMACIÓN 2  
**PROFESOR:** ING. VICTOR KNEIDER  
**TRIMESTRE:** 2026-A

---

# PROYECTO 2: SISTEMA DE GESTIÓN DE INVENTARIO CON PERSISTENCIA Y ACCESO ALEATORIO

## DESCRIPCIÓN GENERAL

Evoluciona tu sistema de gestión de inventario del Proyecto 1 implementando **persistencia de datos mediante archivos binarios** y **acceso aleatorio**. El sistema debe demostrar dominio de operaciones de lectura/escritura binaria, navegación eficiente en archivos usando `seekg()/seekp()`, y gestión de relaciones entre múltiples archivos binarios sin depender de la carga total en memoria (RAM).

---

## OBJETIVOS DE APRENDIZAJE

1. Implementar persistencia de datos con archivos binarios.
2. Dominar operaciones de lectura/escritura binaria con la librería `<fstream>`.
3. Aplicar acceso aleatorio y cálculo de offsets usando `seekg()` y `seekp()`.
4. Comprender el mapeo entre estructuras en memoria y su representación física en disco.
5. Gestionar índices, posiciones y borrado lógico en archivos binarios.
6. Implementar integridad referencial entre múltiples archivos mediante IDs.

---

## FILOSOFÍA DE ACCESO ALEATORIO

### Concepto Fundamental

Este proyecto **NO carga todos los datos en memoria**. A diferencia del Proyecto 1, donde todo el sistema vivía en la RAM usando arreglos dinámicos, aquí los datos permanecen en disco y **solo se carga un registro a la vez en memoria cuando se necesita procesar**.

### Ciclo de Vida de una Operación

Para cualquier acción que involucre un registro (leer, modificar, eliminar lógicamente), tu programa debe seguir estrictamente este flujo:
1. Abrir el archivo binario correspondiente.
2. Calcular la posición exacta en bytes del registro buscado.
3. Desplazar el puntero de lectura/escritura (`seekg` o `seekp`).
4. Leer o escribir **únicamente ese registro**.
5. Cerrar el archivo inmediatamente.

### Ventajas de este enfoque

- **Eficiencia de Memoria:** El uso de RAM es mínimo (apenas unos bytes para las variables temporales) sin importar si gestionas 10 o 100,000 registros.
- **Persistencia Inmediata:** Si ocurre una falla eléctrica (muy común) o el programa crashea, la información ya procesada está segura en el disco.

---

## 1. MODELO DE PERSISTENCIA Y ARQUITECTURA

El sistema debe crear **un archivo binario independiente por cada estructura principal**: `tienda.bin`, `productos.bin`, `proveedores.bin`, `clientes.bin` y `transacciones.bin`.

### 1.1 Estructura Interna de los Archivos

Cada archivo binario debe contar con un **Header (Encabezado)** al principio, seguido de los registros de datos:

```
[HEADER: 16 bytes] → Metadata administrativa del archivo
[REGISTRO 0]
[REGISTRO 1]
...
[REGISTRO N]
```

**Estructura obligatoria del Header:**

```cpp
struct ArchivoHeader {
    int cantidadRegistros;      // Total histórico de registros
    int proximoID;              // Siguiente ID a asignar (Autoincremental)
    int registrosActivos;       // Registros que no están marcados como eliminados
    int version;                // Control de versión del archivo
};
```

El Header evita tener que recorrer todo el archivo secuencialmente para saber cuántos elementos existen o cuál es el próximo ID válido.

---

## 2. ADAPTACIÓN DE ESTRUCTURAS PARA PERSISTENCIA

Para guardar estructuras en archivos binarios, estas no pueden contener punteros, arreglos dinámicos, ni strings de C++ (`std::string`). Su tamaño en bytes (`sizeof`) debe ser estático y predecible.

A continuación, se muestra cómo debe evolucionar la estructura Producto de tu Proyecto 1 para soportar la fase 2. Debes aplicar esta misma lógica de transformación para Proveedor, Cliente y Transacción.

```cpp
struct Producto {
    // 1. Datos básicos (Usar arreglos de char estáticos, NO std::string)
    int id;
    char codigo[20];
    char nombre[100];
    char descripcion[200];
    float precio;
    int stock;
    
    // 2. Llaves Foráneas (Relaciones)
    int idProveedor;                 
    
    // 4. Estadísticas del registro
    int stockMinimo;                 
    int totalVendido;                
    
    // 5. Metadata de Control Obligatoria
    bool eliminado;                  // Para BORRADO LÓGICO
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};
```

### 2.2 Estructura Transacción con Múltiples Productos (Archivo: transacciones.bin)
Originalmente en la primera fase del proyecto, una transacción podía unicamente con tener UN SOLO PRODUCTO, pero deben convertirla para que pueda almacenar más de un producto por transacción. (Se debe mantener los mismos patrones de llaves foráneas)

### 2.2 Principios para adaptar el resto de estructuras
- **Tienda (`tienda.bin`):** Contendrá un único registro. Elimina todos los punteros a los arreglos de entidades. Solo debe guardar datos de la empresa y contadores estadísticos globales.
- **Arreglos de Relaciones:** Si un Proveedor tiene múltiples Productos, usa un arreglo fijo de enteros (ej. `int productosIDs[100]`) para guardar los IDs, acompañado de su respectivo contador (`int cantidadProductos`).
- **Borrado Lógico:** Toda estructura debe tener el flag `bool eliminado`. En un archivo binario, borrar físicamente un registro del medio corrompe las posiciones matemáticas de los demás.

---

## 3. LÓGICA DE OPERACIONES CON ARCHIVOS (CRUD)

A continuación se detallan los requerimientos lógicos para las operaciones base. Es tu responsabilidad como ingeniero traducir estos algoritmos a código funcional en C++.

### 3.1 Inicialización y Gestión de Headers
Debes implementar funciones aisladas para leer y actualizar el header de cualquier archivo:

- `bool inicializarArchivo(const char* nombreArchivo)`: Crea el archivo si no existe y escribe un `ArchivoHeader` inicializado en 0.
- `ArchivoHeader leerHeader(const char* nombreArchivo)`: Abre el archivo, lee los primeros bytes correspondientes al header y lo retorna.
- `bool actualizarHeader(const char* nombreArchivo, ArchivoHeader header)`: Sobrescribe únicamente la sección del header.

### 3.2 Acceso Aleatorio y Cálculo de Offsets
La clave del proyecto es la función matemática para ubicar un registro sin recorrer el archivo.

**Fórmula de posición en bytes:**
```
sizeof(ArchivoHeader) + (indice * sizeof(TuEstructura))
```

> Nota: El indice (posición física 0, 1, 2...) no siempre es igual al ID del registro, especialmente si hay borrados lógicos. Debes hacer una función de búsqueda secuencial que reciba un ID y retorne su indice físico.

### 3.3 Creación de un Registro (Append)
Lógica requerida:

- Leer el header actual para obtener el proximoID.
- Asignar ese ID a tu nueva estructura y marcarla como `eliminado = false`.
- Abrir el archivo en modo adición/binario (`ios::app | ios::binary` o usar `seekp(0, ios::end)`).
- Escribir la estructura.
- Actualizar los contadores en el header y guardarlo nuevamente.

### 3.4 Actualización (Update) y Borrado Lógico (Delete)
Lógica requerida:

- Buscar el índice del registro mediante su ID.
- Calcular su posición exacta en bytes.
- Posicionar el cursor de escritura (`seekp`) en ese byte.
- Escribir la estructura modificada (o la estructura con `eliminado = true`) sobrescribiendo los datos anteriores.

---

## 4. GESTIÓN DE RELACIONES Y TRANSACCIONES

### 4.1 Registrar una Compra/Venta
El proceso de registrar una transacción ahora involucra múltiples accesos a disco para mantener la coherencia.

**Pasos algorítmicos obligatorios para una Venta:**

1. Buscar y leer el Producto y el Cliente desde sus respectivos archivos.
2. Validar reglas de negocio (ej. ¿Hay stock suficiente? ¿El cliente existe?).
3. Si es válido, instanciar un registro Transaccion.
4. Guardar la Transaccion al final de `transacciones.bin`.
5. Modificar el Producto en memoria (restar stock, sumar estadísticas, añadir el ID de la transacción a su arreglo de historial) y actualizar su registro en el archivo binario.
6. Modificar el Cliente en memoria (sumar gastos, añadir ID de transacción) y actualizar su registro en el archivo binario.

---

## 5. MANTENIMIENTO E INTEGRIDAD (NUEVAS FUNCIONALIDADES)

### 5.1 Integridad Referencial
Debes programar un módulo de diagnóstico `verificarIntegridadReferencial()` que detecte "referencias rotas".

¿Qué debe hacer el algoritmo?

- Recorrer `productos.bin`. Por cada producto activo, extraer su `idProveedor`.
- Buscar en `proveedores.bin` si ese ID existe y no está eliminado. Si no existe, registrar el error.
- Repetir la lógica cruzada para Transacciones (verificar que el `idProducto` y el `idCliente/Proveedor` aún existan).
- Imprimir un reporte de salud de la base de datos al finalizar.

### 5.2 Respaldo de Datos (Backup)
Implementar una función `crearBackup()`:

- Debe crear una carpeta (o usar un prefijo en el nombre) con la fecha y hora actual.
- Debe copiar byte a byte los 5 archivos .bin operativos a este nuevo destino seguro.

### 5.3 Reportes Analíticos (Lectura en Lote)
Se requiere un submenú para leer datos y generar estadísticas:

- **Productos con stock crítico:** Recorrer productos y filtrar los que tengan `stock <= stockMinimo`.
- **Historial de Cliente:** Dado un ID de cliente, imprimir sus datos básicos, buscar todas las transacciones asociadas a su arreglo `comprasIDs[]` e imprimir el detalle recuperando el nombre del producto involucrado.

---

## 6. EXPERIENCIA DE USUARIO

### 6.1. IMPRESIÓN DE DATOS
- Todos los datos que se muestren en el sistema deben estar correctamente formateados usando tablas, colores, caracteres, etc

### 6.2. PRACTICIDAD
- Se debe tomar en cuenta que el sistema busca ser práctico, tal vez opciones de los submenus deberian repensarse para poder brindar la mejor experiencia de usuario. Por ejemplo: Cuando muestras la información de un producto ¿No sería bueno mostrar también la información del proveedor que lo vende, o solamente su id te parece suficiente?

## 7. ENTREGABLES

### 7.1 Código Fuente
- Archivo .cpp principal (o modularizado en .h / .cpp).
- Uso estricto de nomenclatura y comentarios descriptivos sobre los cálculos lógicos de bytes.

### 7.2 Archivos de Datos de Prueba
- Tu entrega debe venir acompañada de los archivos .bin pre-poblados con al menos: 15 productos, 5 proveedores, 8 clientes y 25 transacciones que demuestren el funcionamiento.

### 7.3 Documentación Técnica (`README_V2.md`)
- Diagrama de las estructuras y sus tamaños en bytes (`sizeof`).
- Manual de usuario sencillo. */
/* ... (Comentarios del enunciado omitidos por brevedad, se asume que los mantienes) ... */

struct Producto {
    int id;                    // Identificador único (autoincremental)
    char codigo[20];           // Código del producto (ej: "PROD-001")
    char nombre[100];          // Nombre del producto
    char descripcion[200];     // Descripción del producto
    int idProveedor;           // ID del proveedor asociado
    float precio;              // Precio unitario
    int stockminimo;           // Cantidad mínima en inventario
    int totalVendido;          // Cantidad total vendida (para estadísticas)
    int stockInicial;  // Cuánto había al registrarlo
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
    int stockminimo;
    int stockvendido;
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
    int stockminimo;
    int stockvendido;
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
    int idProducto;            // ID del producto involucrado
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

void actualizarCajaTienda(double monto, bool esIngreso) {
    fstream file("tienda.bin", ios::in | ios::out | ios::binary);
    if (!file) return;

    Tienda info;
    file.read(reinterpret_cast<char*>(&info), sizeof(Tienda));

    if (esIngreso) {
        info.ingresosTotales += monto;
        info.caja += monto;
        info.contadorVentas++;
    } else {
        info.egresosTotales += monto;
        info.caja -= monto;
        info.contadorCompras++;
    }
    
    info.fechaUltimaTransaccion = time(0);

    // Sobrescribimos el registro único
    file.seekp(0, ios::beg);
    file.write(reinterpret_cast<char*>(&info), sizeof(Tienda));
    file.close();
}

void inicializartienda(){
    ifstream archivotienda("tienda.bin", ios::binary);
    if(!archivotienda){
    Tienda tienda;
        strcpy(tienda.nombre, "TECNOSTORE");
        strcpy(tienda.rif, "J-12345678-9");
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


void actualizarHeader(const char* nombreArchivo, Header h) {
    fstream archivo(nombreArchivo, ios::in | ios::out | ios::binary);
    if (archivo) {
        archivo.seekp(0, ios::beg); // Nos movemos al puro inicio
        archivo.write(reinterpret_cast<char*>(&h), sizeof(Header));
        archivo.close();
    }
}

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
    cout << "0. Volver al menú principal" << endl;
    cout << "Seleccione una opción: ";
    cin >> opcion;
    vaciarBuffer();
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
    cout << "Seleccione una opción: ";
    cin >> opcion;
    vaciarBuffer();
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
    cout << "5. Eliminar cliente" << endl;
    cout << "0. Volver al menú principal" << endl;
    cout << "Seleccione una opción: ";
    cin >> opcion;
    vaciarBuffer();
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
    cout << "0. Volver al menú principal" << endl;
    cout << "Seleccione una opción: ";
    cin >> opcion;
    vaciarBuffer();
    return opcion;
}

// Retorna false si el usuario presionó la tecla ESC
bool pedirTextoCancelable(const char* mensaje, char* destino, int tamano) {
    cout << mensaje << " (o presione ESC para cancelar): ";
    
    int i = 0;
    char tecla;

    while (true) {
        tecla = _getch(); // Captura la tecla al instante sin esperar el Enter

        if (tecla == 27) { // 27 es el código ASCII de 'Escape'
            cout << "\n[!] Operación cancelada por el usuario." << endl;
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

bool existeProveedor(int Id_a_buscar){
    ifstream archivo("proveedores.bin", ios::binary);
    
    // Si el archivo no existe, obviamente no existe el proveedor
    if (!archivo) return false;

    // 1. Leer el Header para saber cuántos registros recorrer
    Header header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(Header));

    Proveedor temp;
    
    // 2. Recorrer los registros buscando el ID
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read(reinterpret_cast<char*>(&temp), sizeof(Proveedor));

        // 3. Verificamos ID y que esté ACTIVO (Borrado lógico)
        if (temp.id == Id_a_buscar && temp.activo) {
            archivo.close();
            return true; // Lo encontramos
        }
    } 
    archivo.close();
    return false;
}

void mostrarProveedor(int Id_a_buscar) {
    ifstream archivo("proveedores.bin", ios::binary);
    if (!archivo) return;

    Header header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(Header));

    Proveedor temp;
    
    // Aquí NO usamos existeProveedor, porque ya estamos dentro del archivo.
    // Simplemente buscamos el registro que coincida con el ID.
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read(reinterpret_cast<char*>(&temp), sizeof(Proveedor));

        if (temp.id == Id_a_buscar && temp.activo) {
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
            archivo.close();
            return; // Salimos de la función apenas lo mostramos
        }
    }
    archivo.close();
}

void obtenerNombreGenerico(const char* nombreArchivo, int idBuscar, char nombreDestino[100]) {
    strcpy(nombreDestino, "No encontrado");
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo) return;

    Header h;
    archivo.read(reinterpret_cast<char*>(&h), sizeof(Header));

    // Usamos un bloque if/else para saber qué struct leer según el archivo
    if (strcmp(nombreArchivo, "productos.bin") == 0) {
        Producto temp;
        while (archivo.read(reinterpret_cast<char*>(&temp), sizeof(Producto))) {
            if (temp.id == idBuscar) { 
                strcpy(nombreDestino, temp.nombre); 
                break; 
            }
        }
    } 
    else if (strcmp(nombreArchivo, "clientes.bin") == 0) {
        Cliente temp;
        while (archivo.read(reinterpret_cast<char*>(&temp), sizeof(Cliente))) {
            if (temp.id == idBuscar) { 
                strcpy(nombreDestino, temp.nombre); 
                break; 
            }
        }
    } 
    else if (strcmp(nombreArchivo, "proveedores.bin") == 0) {
        Proveedor temp;
        while (archivo.read(reinterpret_cast<char*>(&temp), sizeof(Proveedor))) {
            if (temp.id == idBuscar) { 
                strcpy(nombreDestino, temp.nombre); 
                break; 
            }
        }
    }

    archivo.close();
}

int calcularStockActual(Producto p) {
    int stock = (p.stockInicial + p.totalComprado) - p.totalVendido;
    return stock;
}

bool existeCliente(int id_aBuscar){
    ifstream archivo("clientes.bin", ios::binary);
    if (!archivo) return false;

    Header header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(Header));

    Cliente temp;
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read(reinterpret_cast<char*>(&temp), sizeof(Cliente));
        if (temp.id == id_aBuscar && temp.activo) {
            archivo.close();
            return true;
        }
    }
    archivo.close();
    return false;
}

void mostrarFechaFormateada(time_t fechaRaw) {
    struct tm *infoTiempo;
    char buffer[20]; // Arreglo de char para la fecha

    infoTiempo = localtime(&fechaRaw);

    // Formato: DD/MM/AAAA HH:MM
    strftime(buffer, 20, "%d/%m/%Y %H:%M", infoTiempo);
    
    cout << buffer; 
}

void mostrarcliente(int Id_a_buscar){
   ifstream archivo("clientes.bin", ios::binary);
    if (!archivo) {
        cout << "[!] Error: No se pudo abrir el archivo de clientes." << endl;
        return;
    }
    Header header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(Header));
    Cliente temp;
    bool encontrado = false;
    cout << "\n--- LISTA DE CLIENTES ---" << endl;
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read(reinterpret_cast<char*>(&temp), sizeof(Cliente));
        if (temp.activo && temp.id == Id_a_buscar) {
            encontrado = true;
            cout << " ID: " << temp.id << " | Nombre: " << temp.nombre << " | Cédula/RIF: " << temp.cedula << endl;
        }
    }
    if (!encontrado) {
        cout << "\n[!] No hay clientes registrados." << endl;
    }
    archivo.close();
}

void mostrarTransaccion(int idBuscar) {
    ifstream archivo("transacciones.bin", ios::binary);
    if (!archivo) return;

    Header header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(Header));
    Transaccion t;
    bool encontrado = false;

    while (archivo.read(reinterpret_cast<char*>(&t), sizeof(Transaccion))) {
        if (t.id == idBuscar) {
            encontrado = true;
            char nombreRelacionado[100];

            cout << "\n====================================================" << endl;
            cout << "             DETALLE DE TRANSACCION #" << t.id << endl;
            cout << "====================================================" << endl;
            
            if (t.tipo == 'V') {
                cout << " TIPO: VENTA";
                obtenerNombreGenerico("clientes.bin", t.idRelacionado, nombreRelacionado);
            } else {
                cout << " TIPO: COMPRA";
                obtenerNombreGenerico("proveedores.bin", t.idRelacionado, nombreRelacionado);
            }
            
            cout << " | ENTIDAD: " << nombreRelacionado << endl;
            cout << " FECHA: " << t.fecha << " | DESC: " << t.descripcion << endl;
            cout << "----------------------------------------------------" << endl;
            cout << left << setw(8) << "ID" << setw(22) << "PRODUCTO" << setw(8) << "CANT" << setw(12) << "P. UNIT" << endl;
            cout << "----------------------------------------------------" << endl;

            // Recorrido de los detalles usando tu arreglo de 30
            for (int j = 0; j < t.cantidaditems; j++) {
                char nombreP[100];
                obtenerNombreGenerico("productos.bin", t.detalles[j].idProducto, nombreP);

                cout << left << setw(8) << t.detalles[j].idProducto 
                     << setw(22) << nombreP
                     << setw(8) << t.detalles[j].cantidad 
                     << "$ " << fixed << setprecision(2) << t.detalles[j].precioUnitario << endl;
            }

            cout << "----------------------------------------------------" << endl;
            cout << right << setw(40) << "TOTAL: $ " << t.total << endl;
            
            if (!t.activo) cout << "\n [ESTADO: ANULADA]" << endl;
            cout << "====================================================" << endl;
            break;
        }
    }
    if (!encontrado) cout << "[!] Transaccion no hallada." << endl;
    archivo.close();
}

void mostrarProducto(int idBuscar) {
    ifstream archivo("productos.bin", ios::binary);
    if (!archivo) return;

    Header header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(Header));

    Producto p;
    bool encontrado = false;

    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read(reinterpret_cast<char*>(&p), sizeof(Producto));

        if (p.id == idBuscar) {
            encontrado = true;

            // LLAMADA A TUS NUEVAS FUNCIONES
            int stockActual = calcularStockActual(p);
            
            char nombreProv[100];
            obtenerNombreGenerico("proveedores.bin", p.idProveedor, nombreProv);

            cout << "\n====================================================" << endl;
            cout << "              FICHA DEL PRODUCTO #" << p.id << endl;
            cout << "====================================================" << endl;
            
            if (p.activo == false) {
                cout << " [!!!] ESTADO: INACTIVO [!!!]" << endl;
            }

            cout << " Nombre:       " << p.nombre << endl;
            cout << " Proveedor:    " << nombreProv << endl; // Usamos la función
            cout << "----------------------------------------------------" << endl;
            cout << " STOCK ACTUAL: " << stockActual << " unidades" << endl; // Usamos la función
            cout << " STOCK MINIMO: " << p.stockminimo << " unidades" << endl;

            if (stockActual <= p.stockminimo) {
                cout << " [ALERTA]: RECOMPRAR YA." << endl;
            }
            
            cout << "====================================================" << endl;
            break;
        }
    }
    archivo.close();
}

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

int pedirEntero(const char* mensaje) {
    int numero;
    while (true) {
        cout << mensaje;
        if (cin >> numero) {
            // Si la lectura fue exitosa, limpiamos el resto del buffer (\n) y salimos
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return numero;
        } else {
            // Si entró aquí, es porque el usuario metió una letra o símbolo
            cout << "[!] Error: Debe ingresar un numero entero." << endl;
            cin.clear(); // Limpia el estado de falla de cin
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Descarta la basura ingresada
        }
    }
}

void pedirSoloLetras(const char* mensaje, char* destino, int tamano) {
    bool valido;
    do {
        valido = true;
        cout << mensaje;
        cin.getline(destino, tamano);

        for (int i = 0; i < strlen(destino); i++) {
            // isalpha revisa si es letra. También permitimos espacios ' '
            if (!isalpha(destino[i]) && destino[i] != ' ') {
                valido = false;
                break;
            }
        }

        if (!valido) {
            cout << "[!] Error: No se permiten numeros o simbolos en este campo." << endl;
        }
    } while (!valido);
}

bool tieneDatos(const char* nombreArchivo) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo) return false;

    Header h;
    archivo.read(reinterpret_cast<char*>(&h), sizeof(Header));
    archivo.close();

    return h.cantidadRegistros > 0;
}

bool existeProducto(int idBuscar){
    ifstream archivo("productos.bin", ios::binary);
    if (!archivo) return false;

    Header header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(Header));

    Producto temp;
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read(reinterpret_cast<char*>(&temp), sizeof(Producto));
        if (temp.id == idBuscar && temp.activo) {
            archivo.close();
            return true;
        }
    }
    archivo.close();
    return false;
}

bool existeTransaccion(int idBuscar) {
    ifstream archivo("transacciones.bin", ios::binary);
    if (!archivo) return false;

    Header header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(Header));

    Transaccion t;
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read(reinterpret_cast<char*>(&t), sizeof(Transaccion));
        if (t.id == idBuscar && t.activo) {
            archivo.close();
            return true;
        }
    }
    archivo.close();
    return false;
}

int buscartransaccionPorId(int id){
   ifstream archivo("transacciones.bin", ios::binary);
    if (!archivo) return -1;

    Header header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(Header));

    Transaccion t;
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read(reinterpret_cast<char*>(&t), sizeof(Transaccion));
        if (t.id == id && t.activo) {
            archivo.close();
            return i; 
        }
    }
    archivo.close();
    return -1; 
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

int buscarProductoPorId(int idBuscado) {
    ifstream archivo("productos.bin", ios::binary);
    if (!archivo) return -1;

    Header header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(Header));

    Producto p;
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read(reinterpret_cast<char*>(&p), sizeof(Producto));
        
        if (p.id == idBuscado && p.activo) {
            archivo.close();
            return i; 
        }
    }
    archivo.close();
    return -1;
}

int buscarIndiceProveedor(int idABuscar, bool soloActivos = true) {
    ifstream archivo("proveedores.bin", ios::binary);
    if (!archivo) return -1;

    Header h;
    archivo.read(reinterpret_cast<char*>(&h), sizeof(Header));
    Proveedor temp;

    for (int i = 0; i < h.cantidadRegistros; i++) {
        archivo.read(reinterpret_cast<char*>(&temp), sizeof(Proveedor));

        if (temp.id == idABuscar) {
            // Si solo queremos activos y está en false, ignoramos y seguimos.
            // Si NO nos importa el estado (soloActivos = false), devolvemos el índice.
            if (!soloActivos || temp.activo) {
                archivo.close();
                return i;
            }
        }
    }
    archivo.close();
    return -1;
}

int buscarClientePorId(int idBuscado){
    ifstream archivo("clientes.bin", ios::binary);
    if (!archivo) return -1; 

    Header header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(Header));

    Cliente c;
    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read(reinterpret_cast<char*>(&c), sizeof(Cliente));
        if (c.id == idBuscado && c.activo) {
            archivo.close();
            return i; 
        }
    }
    archivo.close();
    return -1; 
}

bool contieneSubstring(const char* texto, const char* busqueda){
    if(texto == nullptr || busqueda == nullptr) return false;
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

void convertirAMinusculas(char* cadena){
    if(cadena == nullptr) return;
    for (int i = 0; cadena[i] != '\0'; i++) {
        cadena[i] = tolower(cadena[i]);
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

int buscarProductosPorNombre(const char* nombreBusqueda, int idsEncontrados[100]) {
    ifstream archivo("productos.bin", ios::binary);
    if (!archivo) return 0;

    Header header;
    archivo.read((char*)&header, sizeof(Header));

    Producto p;
    int contador = 0;

    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read((char*)&p, sizeof(Producto));

        // Usamos la función contieneSubstring que ya tienes en tu código
        if (p.activo && contieneSubstring(p.nombre, nombreBusqueda)) {
            idsEncontrados[contador] = p.id;
            contador++;
            
            // Límite de seguridad para no desbordar el arreglo de IDs
            if (contador >= 100) break;
        }
    }

    archivo.close();
    return contador; // Retornamos cuántos encontramos
}

void crearProducto() {
    Header h = leerHeader("productos.bin");
    Producto nuevo;
    char confirmacion;
    char nombreProveedor[100] = "";

    if (!obtenerFechaActual(nuevo.fechaRegistro)) return;

    nuevo.id = h.ProximoId;
    cout << "\n--- REGISTRO DE PRODUCTO (ID: " << nuevo.id << ") ---" << endl;
    cout << "(ESC para cancelar en cualquier campo)" << endl;

    if (!pedirTextoCancelable("Codigo", nuevo.codigo, 20)) return;
    if (!pedirTextoCancelable("Nombre", nuevo.nombre, 100)) return;
    if (!pedirTextoCancelable("Descripcion", nuevo.descripcion, 200)) return;

    bool idValido = false;
    do {
        if (!pedirEnteroCancelable("ID del Proveedor", nuevo.idProveedor)) return;

        if (existeProveedor(nuevo.idProveedor)) {
            obtenerNombreGenerico("proveedores.bin", nuevo.idProveedor, nombreProveedor);
            cout << "[OK] Proveedor vinculado: " << nombreProveedor << endl;
            idValido = true;
        } else {
            cout << "[!] El ID " << nuevo.idProveedor << " no existe. Reintente o presione ESC." << endl;
        }
    } while (!idValido);

    int tempPrecio, tempStock, tempMin;
    
    if (!pedirEnteroCancelable("Precio unitario ($)", tempPrecio)) return;
    nuevo.precio = (float)tempPrecio;

    if (!pedirEnteroCancelable("Stock Inicial", nuevo.stockInicial)) return;
    
    if (!pedirEnteroCancelable("Stock Minimo", nuevo.stockminimo)) return;

    nuevo.totalVendido = 0;
    nuevo.totalComprado = 0;
    nuevo.activo = true;
    nuevo.fechaCreacion = time(0);
    nuevo.fechaUltimaModificacion = time(0);

    cout << "\n==================================================" << endl;
    cout << "          CONFIRMAR DATOS DEL PRODUCTO            " << endl;
    cout << "==================================================" << endl;
    cout << " PRODUCTO:  " << nuevo.nombre << endl;
    cout << " PROVEEDOR: " << nombreProveedor << endl;
    cout << " PRECIO:    $ " << nuevo.precio << endl;
    cout << " STOCK:     " << nuevo.stockInicial << endl;
    cout << "==================================================" << endl;
    cout << "¿Guardar registro? (S/N): ";
    
    cin >> confirmacion;
    vaciarBuffer();

    if (toupper(confirmacion) == 'S') {
        ofstream archivo("productos.bin", ios::binary | ios::app);
        if (archivo) {
            archivo.write(reinterpret_cast<const char*>(&nuevo), sizeof(Producto));
            archivo.close();

            h.cantidadRegistros++;
            h.ProximoId++;
            h.registrosActivos++;
            actualizarHeader("productos.bin", h);
            cout << "\n[OK] Guardado exitosamente." << endl;
        }
    } else {
        cout << "\n[!] Registro cancelado." << endl;
    }
    pausarYlimpiarpantalla();
}
void buscarProducto(){
    int idBuscar = pedirEntero("Ingrese el ID del producto a buscar: ");
    if (!existeProducto(idBuscar)) {
        cout << "\n[!] El producto con ID " << idBuscar << " no existe o fue eliminado." << endl;
        pausarYlimpiarpantalla();
        return;
    }
    mostrarProducto(idBuscar);
    pausarYlimpiarpantalla();
}
void actualizarProducto() {
    int idBuscar = pedirEntero("Ingrese el ID del producto a actualizar: ");
    
    int indice = buscarProductoPorId(idBuscar);
    if (indice == -1) {
        cout << "\n[!] El producto con ID " << idBuscar << " no existe o esta inactivo." << endl;
        pausarYlimpiarpantalla();
        return;
    }

    fstream archivo("productos.bin", ios::in | ios::out | ios::binary);
    if (!archivo) return;

    long posicion = sizeof(Header) + (indice * sizeof(Producto));
    archivo.seekg(posicion, ios::beg);
    
    Producto p;
    archivo.read(reinterpret_cast<char*>(&p), sizeof(Producto));

    cout << "\n--- EDITANDO: " << p.nombre << " ---" << endl;
    cout << "1. Nombre\n2. Descripcion\n3. Precio\n4. Stock Minimo\n0. Cancelar\nSeleccione: ";
    int opcion;
    cin >> opcion;
    vaciarBuffer();

    char tempStr[200];
    bool modificado = true;

    switch(opcion) {
        case 1: 
            if (pedirTextoCancelable("Nuevo Nombre", tempStr, 100)) strcpy(p.nombre, tempStr);
            else modificado = false;
            break;
        case 2: 
            if (pedirTextoCancelable("Nueva Descripcion", tempStr, 200)) strcpy(p.descripcion, tempStr);
            else modificado = false;
            break;
        case 3: 
            cout << "Nuevo Precio ($): "; cin >> p.precio; vaciarBuffer();
            break;
        case 4: 
            p.stockminimo = pedirEntero("Nuevo Stock Minimo: ");
            break;
        case 0: 
            modificado = false;
            break;
        default: 
            cout << "[!] Opcion invalida."; 
            modificado = false;
    }

    if (modificado) {
        p.fechaUltimaModificacion = time(0);
        archivo.seekp(posicion, ios::beg);
        archivo.write(reinterpret_cast<const char*>(&p), sizeof(Producto));
        cout << "\n[OK] Producto actualizado correctamente." << endl;
    }

    archivo.close();
    pausarYlimpiarpantalla();
}
void listarProductos() {
    ifstream archivo("productos.bin", ios::binary);
    if (!archivo) {
        cout << "[!] Error al abrir el archivo de productos." << endl;
        return;
    }

    Header h = leerHeader("productos.bin");
    if (h.registrosActivos == 0) {
        cout << "\n[!] No hay productos registrados para mostrar." << endl;
        archivo.close();
        pausarYlimpiarpantalla();
        return;
    }

    cout << "\n" << string(110, '=') << endl;
    cout << setw(5)  << "ID" 
         << setw(12) << "Codigo" 
         << setw(25) << "Nombre" 
         << setw(10) << "Precio" 
         << setw(8)  << "Stock" 
         << setw(10) << "Minimo" 
         << setw(25) << "Proveedor" << endl;
    cout << string(110, '-') << endl;

    archivo.seekg(sizeof(Header), ios::beg);
    
    Producto p;
    char nombreProv[100];

    while (archivo.read(reinterpret_cast<char*>(&p), sizeof(Producto))) {
        if (p.activo) {
            obtenerNombreGenerico("proveedores.bin", p.idProveedor, nombreProv);

            cout << setw(5)  << p.id 
                 << setw(12) << p.codigo 
                 << setw(25) << (strlen(p.nombre) > 22 ? string(p.nombre).substr(0, 20) + ".." : p.nombre)
                 << setw(10) << fixed << setprecision(2) << p.precio 
                 << setw(8)  << p.stockInicial 
                 << setw(10) << p.stockminimo 
                 << setw(25) << nombreProv;


            if (p.stockInicial <= p.stockminimo) {
                cout << " [!] CRITICO";
            }
            cout << endl;
        }
    }

    cout << string(110, '=') << endl;
    cout << " Total de productos activos: " << h.registrosActivos << endl;

    archivo.close();
    pausarYlimpiarpantalla();
}
void eliminarProducto(){
    int idBuscar = pedirEntero("Ingrese el ID del producto a eliminar: ");
    
    int indice = buscarProductoPorId(idBuscar);
    if (indice == -1) {
        cout << "\n[!] El producto con ID " << idBuscar << " no existe o ya esta inactivo." << endl;
        pausarYlimpiarpantalla();
        return;
    }

    fstream archivo("productos.bin", ios::in | ios::out | ios::binary);
    if (!archivo) return;

    long posicion = sizeof(Header) + (indice * sizeof(Producto));
    archivo.seekg(posicion, ios::beg);
    
    Producto p;
    archivo.read(reinterpret_cast<char*>(&p), sizeof(Producto));
    cout << "\n--- ELIMINAR PRODUCTO: " << p.nombre << " ---" << endl;
    cout << "nombre: " << p.nombre << endl;
    cout << "ID: " << p.id << " | Codigo: " << p.codigo << " | Proveedor ID: " << p.idProveedor << endl;
    cout << "Precio: $ " << fixed << setprecision(2) << p.precio << " | Stock: " << calcularStockActual(p) << endl;
    cout << "\n¿Confirma que desea eliminar el producto '" << p.nombre << "'? (S/N): ";
    char confirmacion;
    cin >> confirmacion;
    vaciarBuffer();

    if (toupper(confirmacion) == 'S') {
        p.activo = false;
        p.fechaUltimaModificacion = time(0);
        archivo.seekp(posicion, ios::beg);
        archivo.write(reinterpret_cast<const char*>(&p), sizeof(Producto));

        Header h = leerHeader("productos.bin");
        h.registrosActivos--;
        actualizarHeader("productos.bin", h);

        cout << "\n[OK] Producto eliminado (borrado lógico)." << endl;
    } else {
        cout << "\n[!] Operación cancelada. No se realizaron cambios." << endl;
    }

    archivo.close();
    pausarYlimpiarpantalla();
}

// Funciones CRUD - PROVEEDORES
void crearProveedor() {
    Header h = leerHeader("proveedores.bin");
    Proveedor nuevo;
    char confirmacion;
    char temp[200]; // Buffer temporal para las entradas
    if (!obtenerFechaActual(nuevo.fechaRegistro)) {
        cout << "\n[i] Sugerencia: Verifique la configuracion de fecha y hora en Windows." << endl;
        pausarYlimpiarpantalla();
        return; // Volver al menú de proveedores sin romper el programa
    }
    nuevo.id = h.ProximoId;
    cout << "\n--- REGISTRO DE PROVEEDOR (ID: " << nuevo.id << ") ---" << endl;

    // 1. Nombre con opción de cancelar
    if (!pedirTextoCancelable("Nombre del Proveedor", nuevo.nombre, 100)) return;

    // 2. RIF con validación de duplicados y cancelación
    do {
        if (!pedirTextoCancelable("RIF (Ej: J-12345678-9)", nuevo.rif, 20)) return;
        
        if (rifDuplicado(nuevo.rif)) {
            cout << "[!] Error: Este RIF ya está registrado por otro proveedor." << endl;
        }
    } while (rifDuplicado(nuevo.rif));

    // 3. Teléfono
    if (!pedirTextoCancelable("Teléfono", nuevo.telefono, 20)) return;

    // 4. Email con validación de formato
    do {
        if (!pedirTextoCancelable("Email", nuevo.email, 100)) return;
        
        if (!validarEmail(nuevo.email)) {
            cout << "[!] Formato de email inválido." << endl;
        }
    } while (!validarEmail(nuevo.email));

    // 5. Dirección
    if (!pedirTextoCancelable("Dirección", nuevo.direccion, 200)) return;
    // --- Inicialización de Auditoría ---
    nuevo.totalSuministros = 0;
    nuevo.totalPagado = 0;
    nuevo.activo = true;
    nuevo.fechaCreacion = time(0);
    nuevo.fechaUltimaModificacion = time(0);

    // --- Resumen y Confirmación Final ---
    cout << "\n========================================" << endl;
    cout << "       CONFIRMAR DATOS DEL REGISTRO      " << endl;
    cout << "========================================" << endl;
    cout << " Nombre:    " << nuevo.nombre << endl;
    cout << " RIF:       " << nuevo.rif << endl;
    cout << " Email:     " << nuevo.email << endl;
    cout << " Registro:  " << nuevo.fechaRegistro << " (Fecha de Sistema)" << endl;
    cout << "----------------------------------------" << endl;
    cout << "¿Desea guardar este proveedor? (S/N): ";
    
    cin >> confirmacion;
    vaciarBuffer();

    if (toupper(confirmacion) != 'S') {
        cout << "[!] Registro cancelado. No se realizaron cambios." << endl;
        pausarYlimpiarpantalla();
        return;
    }

    // --- Escritura Física en Archivo ---
    ofstream archivo("proveedores.bin", ios::binary | ios::app);
    if (!archivo) {
        cout << "[!] Error crítico al abrir el archivo." << endl;
        return;
    }
    archivo.write(reinterpret_cast<const char*>(&nuevo), sizeof(Proveedor));
    archivo.close();

    // --- Actualización del Header ---
    h.cantidadRegistros++;
    h.ProximoId++;
    h.registrosActivos++;
    actualizarHeader("proveedores.bin", h);

    cout << "\n[OK] Proveedor '" << nuevo.nombre << "' registrado con éxito." << endl;
    pausarYlimpiarpantalla();
}
void buscarProveedor(){
        int idBuscar = pedirEntero("Ingrese el ID del proveedor a buscar: ");
        if (!existeProveedor(idBuscar)) {
            cout << "\n[!] El proveedor con ID " << idBuscar << " no existe o fue eliminado." << endl;
            pausarYlimpiarpantalla();
            return;
        }
        mostrarProveedor(idBuscar);
        pausarYlimpiarpantalla();
}
void actualizarProveedor() {
    int idBuscar = pedirEntero("Ingrese el ID del proveedor a actualizar: ");
    
    int indice = buscarIndiceProveedor(idBuscar);
    if (indice == -1) {
        cout << "\n[!] El proveedor con ID " << idBuscar << " no existe o fue eliminado." << endl;
        pausarYlimpiarpantalla();
        return;
    }

    // --- AQUÍ ESTÁ EL MENSAJE QUE PEDISTE ---
    cout << "\n--- PROVEEDOR ACTUAL ---" << endl;
    mostrarProveedor(idBuscar);
    cout << "------------------------\n" << endl;

    fstream archivo("proveedores.bin", ios::in | ios::out | ios::binary);
    if (!archivo) return;

    long posicion = sizeof(Header) + (indice * sizeof(Proveedor));
    archivo.seekg(posicion, ios::beg);
    
    Proveedor p;
    archivo.read(reinterpret_cast<char*>(&p), sizeof(Proveedor));

    int opcion;
    cout << "¿Qué desea modificar?" << endl;
    cout << "1. Nombre\n2. Teléfono\n3. Email\n4. Dirección\n5. Rif\n 0. Cancelar todo\nSeleccione: ";
    cin >> opcion;
    vaciarBuffer();

    char entradaTemp[200]; // Variable temporal para no dañar los datos originales si cancelan

    switch(opcion) {
        case 1: 
            // Usamos nuestra nueva función. Si retorna false, cancelamos.
            if (!pedirTextoCancelable("Nuevo Nombre", entradaTemp, 100)) {
                cout << "\n[!] Operacion cancelada. No se guardaron cambios." << endl;
                archivo.close(); return;
            }
            strcpy(p.nombre, entradaTemp);
            break;
            
        case 2: 
            if (!pedirTextoCancelable("Nuevo Teléfono", entradaTemp, 20)) {
                cout << "\n[!] Operacion cancelada." << endl;
                archivo.close(); return;
            }
            strcpy(p.telefono, entradaTemp);
            break;
            
        case 3: 
            do {
                if (!pedirTextoCancelable("Nuevo Email", entradaTemp, 100)) {
                    cout << "\n[!] Operacion cancelada." << endl;
                    archivo.close(); return;
                }
            } while (!validarEmail(entradaTemp)); // Valida solo si no canceló
            strcpy(p.email, entradaTemp);
            break;
            
        case 4: 
            if (!pedirTextoCancelable("Nueva Dirección", entradaTemp, 200)) {
                cout << "\n[!] Operacion cancelada." << endl;
                archivo.close(); return;
            }
            strcpy(p.direccion, entradaTemp);
            break;
            
        case 5:
            if (!pedirTextoCancelable("Nuevo RIF", entradaTemp, 20)) {
                cout << "\n[!] Operacion cancelada." << endl;
                archivo.close(); return;
            }
            // Verificar que el RIF no esté duplicado
            if (rifDuplicado(entradaTemp)) {
                cout << "\n[!] No se puede actualizar el RIF porque ya está siendo usado por otro proveedor activo." << endl;
                archivo.close(); return;
            }
            strcpy(p.rif, entradaTemp);
            break;

        case 0: 
            archivo.close(); return;
            
        default: 
            cout << "Opción inválida."; archivo.close(); return;
    }

    p.fechaUltimaModificacion = time(0);

    archivo.seekp(posicion, ios::beg);
    archivo.write(reinterpret_cast<const char*>(&p), sizeof(Proveedor));
    
    archivo.close();
    cout << "\n[OK] Proveedor actualizado correctamente." << endl;
    pausarYlimpiarpantalla();
}
void listarProveedores() {
    ifstream archivo("proveedores.bin", ios::binary);
    if (!archivo) {
        cout << "[!] Error: No se pudo abrir el archivo de proveedores." << endl;
        return;
    }

    Header header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(Header));

    // Si el Header dice que hay 0 registros, ni siquiera entramos al bucle
    if (header.cantidadRegistros == 0) {
        cout << "\n[!] No hay proveedores registrados en el sistema." << endl;
        archivo.close();
        pausarYlimpiarpantalla();
        return;
    }

    Proveedor temp;
    bool encontrado = false;

    // Encabezado de la tabla
    cout << "\n==========================================================================================" << endl;
    cout << "                             LISTADO DE PROVEEDORES REGISTRADOS                           " << endl;
    cout << "==========================================================================================" << endl;
    cout << left << setw(5)  << "ID" 
         << setw(30) << "NOMBRE" 
         << setw(15) << "RIF" 
         << setw(15) << "TELEFONO" 
         << setw(15) << "REGISTRO" << endl;
    cout << "------------------------------------------------------------------------------------------" << endl;

    for (int i = 0; i < header.cantidadRegistros; i++) {
        archivo.read(reinterpret_cast<char*>(&temp), sizeof(Proveedor));

        // Solo mostramos los que no tienen borrado lógico (activo == true)
        if (temp.activo) {
            encontrado = true;
            cout << left << setw(5)  << temp.id 
                 << setw(30) << temp.nombre 
                 << setw(15) << temp.rif 
                 << setw(15) << temp.telefono 
                 << setw(15) << temp.fechaRegistro << endl;
        }
    }

    if (!encontrado) {
        cout << "\n[!] Todos los registros existentes han sido marcados como eliminados." << endl;
    }

    cout << "==========================================================================================" << endl;
    cout << " Total de registros activos: " << header.registrosActivos << endl;
    cout << "==========================================================================================" << endl;

    archivo.close();
    pausarYlimpiarpantalla();
}
void eliminarProveedor() {
    int idEliminar = pedirEntero("Ingrese el ID del proveedor a eliminar: ");

    // 1. Validar si existe antes de intentar cualquier cosa
    int indice = buscarIndiceProveedor(idEliminar);
    if (indice == -1) {
        cout << "\n[!] El proveedor con ID " << idEliminar << " no existe o ya fue eliminado." << endl;
        pausarYlimpiarpantalla();
        return;
    }

    // 2. Mostrar lo que se va a eliminar para evitar errores
    cout << "\n--- PROVEEDOR ENCONTRADO ---" << endl;
    mostrarProveedor(idEliminar);
    cout << "----------------------------" << endl;

    // 3. Confirmación con soporte para ESC
    cout << "¿Está seguro de que desea eliminar este registro? (S: Confirmar / ESC: Cancelar): ";
    
    char tecla;
    while (true) {
        tecla = _getch();
        if (tecla == 27) { // Tecla ESC
            cout << "\n[!] Operación de eliminación cancelada por el usuario." << endl;
            pausarYlimpiarpantalla();
            return;
        }
        if (toupper(tecla) == 'S') {
            cout << "S" << endl;
            break; 
        }
        if (toupper(tecla) == 'N') {
            cout << "N" << endl;
            cout << "\n[!] Eliminación cancelada." << endl;
            pausarYlimpiarpantalla();
            return;
        }
    }

    // 4. Proceso de Borrado Lógico (Acceso Aleatorio)
    fstream archivo("proveedores.bin", ios::in | ios::out | ios::binary);
    if (!archivo) {
        cout << "[!] Error al acceder al archivo de datos." << endl;
        return;
    }

    // Calculamos la posición exacta
    long posicion = sizeof(Header) + (indice * sizeof(Proveedor));
    archivo.seekg(posicion, ios::beg);

    Proveedor p;
    archivo.read(reinterpret_cast<char*>(&p), sizeof(Proveedor));

    // Cambiamos el estado y actualizamos auditoría
    p.activo = false;
    p.fechaUltimaModificacion = time(0);

    // Sobrescribimos el registro
    archivo.seekp(posicion, ios::beg);
    archivo.write(reinterpret_cast<const char*>(&p), sizeof(Proveedor));
    archivo.close();

    // 5. ACTUALIZAR EL HEADER (Crucial: restamos uno a los activos)
    Header h = leerHeader("proveedores.bin");
    h.registrosActivos--; 
    actualizarHeader("proveedores.bin", h);

    cout << "\n[OK] Proveedor eliminado lógicamente del sistema." << endl;
    pausarYlimpiarpantalla();
}

void restaurarProveedor() {
    Header h = leerHeader("proveedores.bin");
    
    // 1. Validación de papelera vacía
    if (h.cantidadRegistros == h.registrosActivos) {
        cout << "\n[i] No hay proveedores eliminados para restaurar." << endl;
        cout << "Presione Enter para continuar...";
        _getch();
        return; 
    }

    listarProveedoresEliminados();
    int idIngresado = pedirEntero("\nID a restaurar: ");

    // Buscamos el índice ignorando el estado (soloActivos = false)
    int indice = buscarIndiceProveedor(idIngresado, false);

    if (indice == -1) {
        cout << "[!] ID no existe." << endl;
        pausarYlimpiarpantalla();
        return;
    }

    fstream archivo("proveedores.bin", ios::in | ios::out | ios::binary);
    long posicion = sizeof(Header) + (indice * sizeof(Proveedor));
    archivo.seekg(posicion, ios::beg);

    Proveedor p;
    archivo.read(reinterpret_cast<char*>(&p), sizeof(Proveedor));

    if (p.activo) {
        cout << "[!] El proveedor ya esta activo." << endl;
        archivo.close();
        return;
    }

    // --- EL BLOQUE DE SEGURIDAD CRÍTICO ---
    // Antes de restaurar, verificamos si su RIF ya lo tiene OTRO proveedor activo
    if (rifDuplicado(p.rif)) { 
        cout << "\n[!] ERROR DE INTEGRIDAD:" << endl;
        cout << "[!] No se puede restaurar a '" << p.nombre << "'" << endl;
        cout << "[!] El RIF " << p.rif << " ya esta siendo usado por otro proveedor activo." << endl;
        archivo.close();
        pausarYlimpiarpantalla();
        return;
    }

    // Si pasó la validación, procedemos
    cout << "¿Confirmar restauracion de " << p.nombre << "? (S/N): ";
    char confirm = _getch();
    if (toupper(confirm) != 'S') {
        archivo.close();
        return;
    }

    p.activo = true;
    p.fechaUltimaModificacion = time(0);

    archivo.seekp(posicion, ios::beg);
    archivo.write(reinterpret_cast<const char*>(&p), sizeof(Proveedor));
    archivo.close();

    // Actualizar Header
    h.registrosActivos++;
    actualizarHeader("proveedores.bin", h);

    cout << "\n[OK] Proveedor restaurado sin conflictos." << endl;
    pausarYlimpiarpantalla();
}

// Funciones CRUD - CLIENTES
void crearCliente(){}
void buscarCliente(){}
void actualizarCliente(){}
void listarClientes(){}
void eliminarCliente(){}

// Funciones de TRANSACCIONES
void registrarCompra(){}
void registrarVenta(){}
void buscarTransacciones(){}
void listarTransacciones(){}
void cancelarTransaccion(){}

int main() {
    // Configuración para caracteres especiales y FPS fijos
    SetConsoleOutputCP(CP_UTF8);
    
    // 1. INICIALIZACIÓN DE ARCHIVOS
    inicializartienda(); 
    crearArchivoBinario("productos.bin"); 
    crearArchivoBinario("proveedores.bin");
    crearArchivoBinario("clientes.bin");
    crearArchivoBinario("transacciones.bin");

    // 2. LECTURA DE DATOS DE LA TIENDA (La Fuente de Verdad)
    Tienda miTienda;
    ifstream fTienda("tienda.bin", ios::binary);
    if (fTienda) {
        fTienda.read(reinterpret_cast<char*>(&miTienda), sizeof(Tienda));
        fTienda.close();
    }

    bool ejecutando = true;
    int opcion, subOp;

    do {
        cout << "╔═══════════════════════════════════════════╗" << endl;
        cout << "║      SISTEMA DE GESTIÓN DE INVENTARIO     ║" << endl;
        cout << "║ Tienda: " << left << setw(33) << miTienda.nombre << " ║" << endl;
        cout << "║ RIF:    " << left << setw(33) << miTienda.rif << " ║" << endl;
        cout << "╚═══════════════════════════════════════════╝" << endl;
        cout << "1. Gestión de Productos" << endl;
        cout << "2. Gestión de Proveedores" << endl;
        cout << "3. Gestión de Clientes" << endl;
        cout << "4. Gestión de Transacciones" << endl;
        cout << "5. Salir" << endl;
        cout << "Seleccione una opción: ";
        cin >> opcion;
        vaciarBuffer();

        switch (opcion) {
            case 1:
                do {
                    subOp = submenu_producto();
                    switch(subOp) {
                        case 1: if(tieneDatos("proveedores.bin")) crearProducto(); else cout << "[!] No hay proveedores disponibles para crear un producto.\n"; break;
                        case 2: if(tieneDatos("productos.bin")) buscarProducto(); else cout << "[!] No hay productos.\n"; break;
                        case 3: if(tieneDatos("productos.bin"))actualizarProducto(); else cout << "[!] No hay productos.\n"; break;
                        case 4: if(tieneDatos("productos.bin"))listarProductos();  else cout << "[!] No hay productos.\n"; break;
                        case 5: if(tieneDatos("productos.bin"))eliminarProducto(); else cout << "[!] No hay productos.\n"; break;
                    }
                } while(subOp != 0);
                break;

            case 2:
                do {
                    subOp = submenu_proveedor();
                    switch(subOp) {
                        case 1: crearProveedor(); break;
                        case 2: if(tieneDatos("proveedores.bin")) buscarProveedor(); else cout << "[!] No hay proveedores.\n"; break;
                        case 3: actualizarProveedor(); break;
                        case 4: listarProveedores(); break;
                        case 5: eliminarProveedor(); break;
                        case 6: restaurarProveedor(); break;
                    }
                } while(subOp != 0);
                break;

            case 3:
                do {
                    subOp = submenu_cliente();
                    switch(subOp) {
                        case 1: crearCliente(); break;
                        case 2: if(tieneDatos("clientes.bin")) buscarCliente(); else cout << "[!] No hay clientes.\n"; break;
                        case 3: actualizarCliente(); break;
                        case 4: listarClientes(); break;
                        case 5: eliminarCliente(); break;
                    }
                } while(subOp != 0);
                break;

            case 4:
                do {
                    subOp = submenu_transaccion();
                    switch(subOp) {
                        case 1: registrarCompra(); break;
                        case 2: registrarVenta(); break;
                        case 3: if(tieneDatos("transacciones.bin")) buscarTransacciones(); else cout << "[!] No hay facturas.\n"; break;
                        case 4: listarTransacciones(); break;
                        case 5: cancelarTransaccion(); break;
                    }
                } while(subOp != 0);
                break;
            case 5:
                cout << "Cerrando sesión de " << miTienda.nombre << "..." << endl;
                ejecutando = false;
                break;

            default:
                cout << "[!] Opción inválida." << endl;
                pausarYlimpiarpantalla();
        }
    } while (ejecutando);

    return 0;
}
   