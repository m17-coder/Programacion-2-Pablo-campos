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

void mostrarInstruccionESC() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 8); // 8 = Gris oscuro (Color tenue)
    cout << " [i] Presione ESC en cualquier campo para cancelar y volver al menu principal." << endl;
    SetConsoleTextAttribute(hConsole, 7); // 7 = Restaura el color original (Blanco/Gris)
    cout << endl; // Un salto de linea para separar de los inputs
}

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
    cout << "Seleccione una opción: ";
    cin >> opcion;
    vaciarBuffer();
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
    cout << "Seleccione una opción: ";
    cin >> opcion;
    vaciarBuffer();
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
    cout << "Seleccione una opción: ";
    cin >> opcion;
    vaciarBuffer();
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

void pedirSoloLetras(const char* mensaje, char* destino, int longitud_texto) {
    bool valido;
    do {
        valido = true;
        cout << mensaje;
        cin.getline(destino, longitud_texto);

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

    return h.registrosActivos > 0;
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

bool pedirEnteroCancelable(const char* etiqueta, int &valor) {
    cout << etiqueta << ": ";
    string entrada = "";
    char c;
    while (true) {
        c = getch();
        if (c == 27) { // ESC
            cout << " [CANCELADO]" << endl;
            pausarYlimpiarpantalla();
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

Producto leerProductoPorIndice(int indice) {
    Producto p;
    // Abrimos en modo lectura binaria
    ifstream archivo("productos.bin", ios::binary);

    if (!archivo) {
        cout << "[!] Error: No se pudo abrir el archivo de productos." << endl;
        // Opcional: inicializar p en limpio si falla
        p.id = -1; 
        return p;
    }

    // CALCULAMOS LA POSICIÓN:
    // Saltamos el Header + (el número de índice multiplicado por el tamaño de cada producto)
    long posicion = sizeof(Header) + (indice * sizeof(Producto));

    // Movemos el puntero de lectura (get pointer)
    archivo.seekg(posicion, ios::beg);

    // Leemos el bloque de datos directamente a nuestra variable p
    archivo.read(reinterpret_cast<char*>(&p), sizeof(Producto));

    archivo.close();
    return p;
}

Proveedor leerProveedorPorIndice(int indice) {
    Proveedor p;
    ifstream archivo("proveedores.bin", ios::binary);
    
    if (!archivo) {
        cout << "[!] Error: No se pudo abrir el archivo de proveedores." << endl;
        p.id = -1; 
        return p;
    }

    // Calculamos posición: Saltamos Header + (indice * tamaño del struct)
    long posicion = sizeof(Header) + (indice * sizeof(Proveedor));
    archivo.seekg(posicion, ios::beg);
    archivo.read(reinterpret_cast<char*>(&p), sizeof(Proveedor));
    
    archivo.close();
    return p;
}

void escribirProveedorPorIndice(int indice, Proveedor p) {
    fstream archivo("proveedores.bin", ios::in | ios::out | ios::binary);
    
    if (!archivo) {
        cout << "[!] Error: No se pudo actualizar el registro del proveedor." << endl;
        return;
    }

    long posicion = sizeof(Header) + (indice * sizeof(Proveedor));
    archivo.seekp(posicion, ios::beg);
    archivo.write(reinterpret_cast<const char*>(&p), sizeof(Proveedor));
    
    archivo.close();
}

Cliente leerClientePorIndice(int indice) {
    Cliente c;
    ifstream archivo("clientes.bin", ios::binary);
    
    if (!archivo) {
        cout << "[!] Error: No se pudo abrir el archivo de clientes." << endl;
        c.id = -1; // Marcador de error
        return c;
    }

    // Posicion = Header + (indice * tamaño del struct Cliente)
    long posicion = sizeof(Header) + (indice * sizeof(Cliente));
    archivo.seekg(posicion, ios::beg);
    archivo.read(reinterpret_cast<char*>(&c), sizeof(Cliente));
    
    archivo.close();
    return c;
}

void escribirClientePorIndice(int indice, Cliente c) {
    // Abrimos en modo in/out para poder sobrescribir una parte específica
    fstream archivo("clientes.bin", ios::in | ios::out | ios::binary);
    
    if (!archivo) {
        cout << "[!] Error fatal: No se pudo actualizar el registro del cliente." << endl;
        return;
    }

    long posicion = sizeof(Header) + (indice * sizeof(Cliente));
    archivo.seekp(posicion, ios::beg);
    archivo.write(reinterpret_cast<const char*>(&c), sizeof(Cliente));
    
    archivo.close();
}

bool hayDineroEnCaja(float montoRequerido) {
    ifstream archivo("tienda.bin", ios::binary);
    if (!archivo) {
        cout << "[!] Error: No se pudo abrir tienda.bin para consultar el saldo." << endl;
        return false;
    }

    // Si guardas la tienda como un único objeto en el archivo:
    Tienda miTienda;
    archivo.read(reinterpret_cast<char*>(&miTienda), sizeof(Tienda));
    archivo.close();

    // Comparamos el monto contra el saldo de la tienda
    return (miTienda.caja >= montoRequerido);
}

void crearProducto() {
    Header h = leerHeader("productos.bin");
    Producto nuevo;
    char confirmacion;
    char nombreProveedor[100] = "";

    if (!obtenerFechaActual(nuevo.fechaRegistro)) return;

    nuevo.id = h.ProximoId;
    
    cout << "\n==================================================" << endl;
    cout << "       REGISTRO DE PRODUCTO (ID: " << nuevo.id << ") " << endl;
    cout << "==================================================" << endl;
    
    mostrarInstruccionESC();

    if (!pedirTextoCancelable("Codigo", nuevo.codigo, 20)) return;
    if (!pedirTextoCancelable("Nombre del Producto", nuevo.nombre, 100)) return;
    if (!pedirTextoCancelable("Descripcion", nuevo.descripcion, 200)) return;

    bool idValido = false;
    do {
        if (!pedirEnteroCancelable("ID del Proveedor", nuevo.idProveedor)) return;

        if (existeProveedor(nuevo.idProveedor)) {
            obtenerNombreGenerico("proveedores.bin", nuevo.idProveedor, nombreProveedor);
            cout << "  [OK] Proveedor detectado: " << nombreProveedor << endl;
            idValido = true;
        } else {
            cout << "  [!] Error: El Proveedor ID " << nuevo.idProveedor << " no existe o esta inactivo." << endl;
            cout << "      Reintente o presione ESC para salir." << endl;
        }
    } while (!idValido);

    int tempPrecio;
    do {
        if (!pedirEnteroCancelable("Precio Unitario ($)", tempPrecio)) return;
        if (tempPrecio < 0) {
            cout << "  [!] El precio no puede ser un numero negativo." << endl;
        }
    } while (tempPrecio < 0);
    nuevo.precio = (float)tempPrecio;

    do {
        if (!pedirEnteroCancelable("Stock Inicial", nuevo.stockInicial)) return;
        if (nuevo.stockInicial < 0) {
            cout << "  [!] No puede iniciar un producto con inventario negativo." << endl;
        }
    } while (nuevo.stockInicial < 0);
    
    do {
        if (!pedirEnteroCancelable("Stock Minimo (Alerta)", nuevo.stockminimo)) return;
        if (nuevo.stockminimo < 0) {
            cout << "  [!] El stock minimo no puede ser negativo." << endl;
        }
    } while (nuevo.stockminimo < 0);

    // Inicialización de valores estadísticos y de control
    nuevo.totalVendido = 0;
    nuevo.totalComprado = 0;
    nuevo.activo = true;
    nuevo.fechaCreacion = time(0);
    nuevo.fechaUltimaModificacion = time(0);

    // Cuadro de Resumen para el usuario
    cout << "\n--------------------------------------------------" << endl;
    cout << "          REVISIÓN DE DATOS PRE-GUARDADO          " << endl;
    cout << "--------------------------------------------------" << endl;
    cout << " > PRODUCTO:  " << nuevo.nombre << endl;
    cout << " > PROVEEDOR: " << nombreProveedor << endl;
    cout << " > PRECIO:    $ " << fixed << setprecision(2) << nuevo.precio << endl;
    cout << " > STOCK INI: " << nuevo.stockInicial << endl;
    cout << " > S. MINIMO: " << nuevo.stockminimo << endl;
    cout << "--------------------------------------------------" << endl;
    cout << " ¿Desea guardar este producto en el sistema? (S/N): ";
    
    cin >> confirmacion;
    vaciarBuffer();

    if (toupper(confirmacion) == 'S') {
        // Escritura en modo APP (Append) para agregar al final del archivo binario
        ofstream archivo("productos.bin", ios::binary | ios::app);
        if (archivo) {
            archivo.write(reinterpret_cast<const char*>(&nuevo), sizeof(Producto));
            archivo.close();

            // Actualización del Header del archivo
            h.cantidadRegistros++;
            h.ProximoId++;
            h.registrosActivos++;
            actualizarHeader("productos.bin", h);

            cout << "\n[OK] Producto registrado y persistido en disco exitosamente." << endl;
        } else {
            cout << "\n[!] Error fatal: No se pudo acceder al archivo productos.bin." << endl;
        }
    } else {
        cout << "\n[i] Operacion descartada. No se realizaron cambios." << endl;
    }

    pausarYlimpiarpantalla();
}
void buscarProducto(){
    char opcion;
    pedirTextoCancelable("Buscar por nombre (N) o por ID (I)", &opcion, 2);
    mostrarInstruccionESC();
    if(toupper(opcion) == 'N'){
        char nombreBusqueda[100];
        if (!pedirTextoCancelable("Ingrese el nombre o parte del nombre del producto a buscar", nombreBusqueda, 100)) return;
        cin.ignore();
        cin.getline(nombreBusqueda, 100);

        int idsEncontrados[100];
        int cantidadEncontrados = buscarProductosPorNombre(nombreBusqueda, idsEncontrados);

        if (cantidadEncontrados == 0) {
            cout << "\n[!] No se encontraron productos que coincidan con '" << nombreBusqueda << "'." << endl;
        } else {
            cout << "\n[OK] Se encontraron " << cantidadEncontrados << " producto(s) que coinciden con '" << nombreBusqueda << "':" << endl;
            for (int i = 0; i < cantidadEncontrados; i++) {
                mostrarProducto(idsEncontrados[i]);
                cout << "--------------------------------------------------" << endl;
            }
        }
    } else if(toupper(opcion) == 'I'){
    int idBuscar;
    if (!pedirEnteroCancelable("Ingrese el ID del producto a buscar", idBuscar)) return;
    if (!existeProducto(idBuscar)) {
        cout << "\n[!] El producto con ID " << idBuscar << " no existe o fue eliminado." << endl;
        pausarYlimpiarpantalla();
        return;
    }
    mostrarProducto(idBuscar);
    pausarYlimpiarpantalla();
  }
}
void actualizarProducto() {
    int idBuscar;
    int indice;

    cout << "\n--- ACTUALIZAR PRODUCTO ---" << endl;
    mostrarInstruccionESC(); 

    // 1. Buscamos el ID con opción de cancelar
    if (!pedirEnteroCancelable("Ingrese el ID del producto a actualizar", idBuscar)) return;

    indice = buscarProductoPorId(idBuscar);
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

    if (modificado) {
        p.fechaUltimaModificacion = time(0);
        archivo.seekp(posicion, ios::beg);
        archivo.write(reinterpret_cast<const char*>(&p), sizeof(Producto));
        cout << "\n[OK] Producto actualizado correctamente." << endl;
    } else {
        cout << "\n[i] No se realizaron cambios." << endl;
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
    int idBuscar;
    if (!pedirEnteroCancelable("Ingrese el ID del producto a eliminar", idBuscar)) return;
    mostrarInstruccionESC();
    int indice;
    if (!pedirEnteroCancelable("ID del producto a eliminar", idBuscar)) return;
    indice = buscarProductoPorId(idBuscar);
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
    mostrarInstruccionESC();
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
        mostrarInstruccionESC();
        int idBuscar;
        if (!pedirEnteroCancelable("Ingrese el ID del proveedor a buscar", idBuscar)) {
            // Al entrar aquí, ya se detectó el ESC
            return; // Salimos de la función inmediatamente
        }
        if (!existeProveedor(idBuscar)) {
            cout << "\n[!] El proveedor con ID " << idBuscar << " no existe o fue eliminado." << endl;
            pausarYlimpiarpantalla();
            return;
        }
        mostrarProveedor(idBuscar);
        pausarYlimpiarpantalla();
}
void actualizarProveedor() {
    int idBuscar; 
    if (!pedirEnteroCancelable("Ingrese el ID del proveedor a actualizar", idBuscar)) {
    // Al entrar aquí, ya se detectó el ESC
    return; // Salimos de la función inmediatamente
}
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
    if (!archivo) {
        cout << "[!] Error al abrir el archivo de proveedores." << endl;       
        return;
    }

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
    if (!existeProveedor(1)) { // Verificar si hay al menos un proveedor activo antes de pedir ID
        cout << "\n[!] No hay proveedores activos para eliminar." << endl;
        pausarYlimpiarpantalla();
        return;
    }
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
    mostrarInstruccionESC();

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
void crearCliente() {
    Header h = leerHeader("clientes.bin");
    Cliente nuevo;
    char confirmacion;
    
    if (!obtenerFechaActual(nuevo.fechaRegistro)) {
        cout << "\n[!] Error al obtener fecha del sistema." << endl;
        pausarYlimpiarpantalla();
        return; 
    }
    
    nuevo.id = h.ProximoId;
    system("cls");
    cout << "=================================================" << endl;
    cout << "       REGISTRO DE NUEVO CLIENTE (ID: " << nuevo.id << ")" << endl;
    cout << "      (Presione ESC en cualquier paso para salir)  " << endl;
    cout << "=================================================" << endl;

    // 1. Nombre
    if (!pedirTextoCancelable("Nombre del Cliente", nuevo.nombre, 100)) return;

    // 2. Cédula (Validando duplicados)
    do {
        if (!pedirTextoCancelable("Cedula/RIF (Identificador)", nuevo.cedula, 20)) return;
        
        if (cedulaDuplicada(nuevo.cedula)) {
            cout << "[!] Error: Esta Cedula ya pertenece a un cliente activo." << endl;
        }
    } while (cedulaDuplicada(nuevo.cedula));

    // 3. Teléfono
    if (!pedirTextoCancelable("Telefono de contacto", nuevo.telefono, 20)) return;

    // 4. Email (Con tu validación existente)
    do {
        if (!pedirTextoCancelable("Correo Electronico", nuevo.email, 100)) return;
        if (!validarEmail(nuevo.email)) {
            cout << "[!] Formato de email invalido (ejemplo@dominio.com)." << endl;
        }
    } while (!validarEmail(nuevo.email));

    // 5. Dirección
    if (!pedirTextoCancelable("Direccion de habitacion/fiscal", nuevo.direccion, 200)) return;

    // Inicialización de campos numéricos y estado
    nuevo.totalComprasRealizadas = 0;
    nuevo.montoTotalGastado = 0.0;
    nuevo.activo = true;
    nuevo.fechaCreacion = time(0);
    nuevo.fechaUltimaModificacion = time(0);

    // --- PARTE DE CONFIRMACIÓN (Igual a Proveedor) ---
    cout << "\n========================================" << endl;
    cout << "       VERIFIQUE LOS DATOS DEL CLIENTE   " << endl;
    cout << "========================================" << endl;
    cout << " ID:        " << nuevo.id << endl;
    cout << " Nombre:    " << nuevo.nombre << endl;
    cout << " Cedula:    " << nuevo.cedula << endl;
    cout << " Telefono:  " << nuevo.telefono << endl;
    cout << " Email:     " << nuevo.email << endl;
    cout << " Registro:  " << nuevo.fechaRegistro << endl;
    cout << "----------------------------------------" << endl;
    cout << " ¿Desea guardar el registro? (S/N): ";
    
    cin >> confirmacion;
    vaciarBuffer();

    if (toupper(confirmacion) == 'S') {
        ofstream archivo("clientes.bin", ios::binary | ios::app);
        if (!archivo) {
            cout << "[!] Error al guardar en el archivo." << endl;
            return;
        }
        archivo.write(reinterpret_cast<const char*>(&nuevo), sizeof(Cliente));
        archivo.close();

        // Actualizar Header
        h.cantidadRegistros++;
        h.ProximoId++;
        h.registrosActivos++;
        actualizarHeader("clientes.bin", h);

        cout << "\n[OK] Cliente registrado exitosamente." << endl;
    } else {
        cout << "\n[!] Operacion cancelada. No se guardaron datos." << endl;
    }

    pausarYlimpiarpantalla();
}
void buscarCliente(){
    if (!tieneDatos("clientes.bin")) {
        cout << "\n[!] No hay clientes registrados en el sistema." << endl;
        pausarYlimpiarpantalla();
        return;
    }
        int idBuscar = pedirEntero("Ingrese el ID del cliente a buscar: ");
        mostrarInstruccionESC();
            if (!existeCliente(idBuscar)) {
                cout << "\n[!] El cliente con ID " << idBuscar << " no existe o fue eliminado." << endl;
                pausarYlimpiarpantalla();
                return;
            }
            mostrarcliente(idBuscar);
            pausarYlimpiarpantalla();
}
void actualizarCliente() {
    if (!tieneDatos("clientes.bin")) {
        cout << "\n[!] No hay clientes registrados para actualizar." << endl;
        pausarYlimpiarpantalla();
        return;
    }

    fstream archivo("clientes.bin", ios::binary | ios::in | ios::out);
    if (!archivo) {
        cout << "[!] Error critico al abrir el archivo de clientes." << endl;
        pausarYlimpiarpantalla();
        return;
    }

    char cedulaBusqueda[20];
    system("cls");
    cout << "=================================================" << endl;
    cout << "             ACTUALIZAR DATOS DE CLIENTE         " << endl;
    cout << "=================================================" << endl;
    mostrarInstruccionESC();
    if (!pedirTextoCancelable("Ingrese la Cedula/RIF a buscar", cedulaBusqueda, 20)) {
        archivo.close();
        return;
    }

    Header h;
    archivo.read(reinterpret_cast<char*>(&h), sizeof(Header));

    Cliente temp;
    bool encontrado = false;
    long posicionRegistro = 0;

    for (int i = 0; i < h.cantidadRegistros; i++) {
        posicionRegistro = archivo.tellg(); 
        archivo.read(reinterpret_cast<char*>(&temp), sizeof(Cliente));

        if (temp.activo && strcmp(temp.cedula, cedulaBusqueda) == 0) {
            encontrado = true;
            break;
        }
    }

    if (!encontrado) {
        cout << "\n[!] No se encontro un cliente activo con la cedula: " << cedulaBusqueda << endl;
        archivo.close();
        pausarYlimpiarpantalla();
        return;
    }

    cout << "\n[i] Cliente encontrado: " << temp.nombre << endl;
    cout << "[i] Pulse enter para dejar los mismos datos." << endl;
    
    char nuevoNombre[100], nuevoTlf[20], nuevoEmail[100], nuevaDir[200];

    if (!pedirTextoCancelable("Nuevo Nombre", nuevoNombre, 100)) { archivo.close(); return; }
    if (strlen(nuevoNombre) > 0) strcpy(temp.nombre, nuevoNombre);

    if (!pedirTextoCancelable("Nuevo Telefono", nuevoTlf, 20)) { archivo.close(); return; }
    if (strlen(nuevoTlf) > 0) strcpy(temp.telefono, nuevoTlf);

    do {
        if (!pedirTextoCancelable("Nuevo Correo", nuevoEmail, 100)) { archivo.close(); return; }
        if (strlen(nuevoEmail) > 0) {
            if (validarEmail(nuevoEmail)) {
                strcpy(temp.email, nuevoEmail);
                break;
            } else {
                cout << "[!] Error: Formato de correo invalido." << endl;
            }
        } else break; 
    } while (true);

    if (!pedirTextoCancelable("Nueva Direccion", nuevaDir, 200)) { archivo.close(); return; }
    if (strlen(nuevaDir) > 0) strcpy(temp.direccion, nuevaDir);

    temp.fechaUltimaModificacion = time(0);

    cout << "\n¿Desea guardar los cambios realizados? (S/N): ";
    char confirmacion;
    cin >> confirmacion;
    vaciarBuffer();

    if (toupper(confirmacion) == 'S') {
        archivo.seekp(posicionRegistro, ios::beg);
        archivo.write(reinterpret_cast<const char*>(&temp), sizeof(Cliente));
        cout << "\n[OK] Datos del cliente actualizados correctamente." << endl;
    } else {
        cout << "\n[!] Operacion cancelada. Los datos originales se mantienen." << endl;
    }

    archivo.close();
    pausarYlimpiarpantalla();
}
void listarClientes() {
    if (!tieneDatos("clientes.bin")) {
        cout << "\n[!] No hay clientes registrados para mostrar." << endl;
        pausarYlimpiarpantalla();
        return;
    }

    // 2. Apertura del archivo
    ifstream archivo("clientes.bin", ios::binary);
    if (!archivo) {
        cout << "[!] Error al abrir el archivo de clientes." << endl;
        return;
    }

    // 3. Lectura del Header
    Header h;
    archivo.read(reinterpret_cast<char*>(&h), sizeof(Header));

    if (h.registrosActivos == 0) {
        cout << "\n[!] No hay clientes activos en el sistema." << endl;
        archivo.close();
        pausarYlimpiarpantalla();
        return;
    }

    Cliente temp;

    cout << "\n==================================================================================" << endl;
    cout << "                              LISTADO DE CLIENTES" << endl;
    cout << "==================================================================================" << endl;
    cout << left << setw(8)  << "ID" 
         << setw(30) << "NOMBRE" 
         << setw(15) << "CEDULA/RIF" 
         << setw(15) << "TELEFONO" 
         << setw(12) << "REGISTRO" << endl;
    cout << "----------------------------------------------------------------------------------" << endl;

    // 5. Recorrido de los registros
    for (int i = 0; i < h.cantidadRegistros; i++) {
        archivo.read(reinterpret_cast<char*>(&temp), sizeof(Cliente));

        // Solo mostramos si el registro está activo (Borrado Lógico)
        if (temp.activo) {
            cout << left << setw(8)  << temp.id 
                 << setw(30) << temp.nombre 
                 << setw(15) << temp.cedula 
                 << setw(15) << temp.telefono 
                 << setw(12) << temp.fechaRegistro << endl;
        }
    }

    // 6. Pie de tabla con resumen
    cout << "==================================================================================" << endl;
    cout << " Total de registros activos: " << h.registrosActivos << endl;
    cout << "==================================================================================" << endl;

    archivo.close();
    pausarYlimpiarpantalla();
}
void eliminarCliente() {
    // 1. Verificación inicial de datos
    if (!tieneDatos("clientes.bin")) {
        cout << "\n[!] No hay clientes registrados para eliminar." << endl;
        pausarYlimpiarpantalla();
        return;
    }

    // 2. Apertura en modo lectura/escritura
    fstream archivo("clientes.bin", ios::binary | ios::in | ios::out);
    if (!archivo) {
        cout << "[!] Error al abrir el archivo de clientes." << endl;
        return;
    }

    char cedulaBusqueda[20];
    system("cls");
    cout << "=================================================" << endl;
    cout << "               ELIMINAR CLIENTE                  " << endl;
    cout << "=================================================" << endl;
    mostrarInstruccionESC();
    if (!pedirTextoCancelable("Ingrese la Cedula del cliente a eliminar", cedulaBusqueda, 20)) {
        archivo.close();
        return;
    }

    Header h;
    archivo.read(reinterpret_cast<char*>(&h), sizeof(Header));

    Cliente temp;
    bool encontrado = false;
    long posicionRegistro = 0;

    // 3. Búsqueda del registro activo
    for (int i = 0; i < h.cantidadRegistros; i++) {
        posicionRegistro = archivo.tellg(); // Guardamos la posición antes de leer
        archivo.read(reinterpret_cast<char*>(&temp), sizeof(Cliente));

        if (temp.activo && strcmp(temp.cedula, cedulaBusqueda) == 0) {
            encontrado = true;
            break;
        }
    }

    if (!encontrado) {
        cout << "\n[!] No se encontro un cliente activo con la cedula: " << cedulaBusqueda << endl;
        archivo.close();
        pausarYlimpiarpantalla();
        return;
    }

    // 4. Confirmación de eliminación
    cout << "\n[i] Cliente localizado: " << temp.nombre << endl;
    cout << "¿ESTA SEGURO QUE DESEA ELIMINAR ESTE REGISTRO? (S/N): ";
    char confirmacion;
    cin >> confirmacion;
    vaciarBuffer();

    if (toupper(confirmacion) == 'S') {
        // --- PROCESO DE BORRADO LÓGICO ---
        temp.activo = false; // Cambiamos el estado
        temp.fechaUltimaModificacion = time(0);

        // Volvemos a la posición del registro y sobreescribimos
        archivo.seekp(posicionRegistro, ios::beg);
        archivo.write(reinterpret_cast<const char*>(&temp), sizeof(Cliente));
        
        // Cerramos para liberar el archivo antes de actualizar el header
        archivo.close();

        // 5. Actualización del Header (restamos uno a los activos)
        h.registrosActivos--;
        actualizarHeader("clientes.bin", h);

        cout << "\n[OK] Cliente eliminado exitosamente (Borrado Logico)." << endl;
    } else {
        cout << "\n[!] Operacion cancelada. El cliente sigue activo." << endl;
        archivo.close();
    }

    pausarYlimpiarpantalla();
}

void restaurarCliente() {
    // 1. Verificación inicial de datos
    if (!tieneDatos("clientes.bin")) {
        cout << "\n[!] No hay registros en el archivo de clientes." << endl;
        pausarYlimpiarpantalla();
        return;
    }

    // 2. Apertura en modo lectura/escritura (fstream)
    fstream archivo("clientes.bin", ios::binary | ios::in | ios::out);
    if (!archivo) {
        cout << "[!] Error al abrir el archivo de clientes." << endl;
        return;
    }

    char cedulaBusqueda[20];
    system("cls");
    cout << "=================================================" << endl;
    cout << "             RESTAURAR CLIENTE                   " << endl;
    cout << "=================================================" << endl;

    mostrarInstruccionESC();
    if (!pedirTextoCancelable("Ingrese la Cedula del cliente a restaurar", cedulaBusqueda, 20)) {
        archivo.close();
        return;
    }

    Header h;
    archivo.read(reinterpret_cast<char*>(&h), sizeof(Header));

    Cliente temp;
    bool encontrado = false;
    long posicionRegistro = 0;

    // 3. Búsqueda del registro ELIMINADO (!temp.activo)
    for (int i = 0; i < h.cantidadRegistros; i++) {
        posicionRegistro = archivo.tellg(); 
        archivo.read(reinterpret_cast<char*>(&temp), sizeof(Cliente));

        if (!temp.activo && strcmp(temp.cedula, cedulaBusqueda) == 0) {
            encontrado = true;
            break;
        }
    }

    if (!encontrado) {
        cout << "\n[!] No se encontro un cliente ELIMINADO con esa cedula." << endl;
        cout << "    (Si el cliente ya esta activo, no necesita restaurarse)." << endl;
        archivo.close();
        pausarYlimpiarpantalla();
        return;
    }

    // 4. Confirmación de Restauración
    cout << "\n[i] Cliente localizado en papelera: " << temp.nombre << endl;
    cout << "¿Desea ACTIVAR nuevamente este registro? (S/N): ";
    char confirmacion;
    cin >> confirmacion;
    vaciarBuffer();

    if (toupper(confirmacion) == 'S') {
        // --- PROCESO DE RESTAURACIÓN ---
        temp.activo = true; // Volvemos a activar el registro
        temp.fechaUltimaModificacion = time(0);

        // Posicionamiento aleatorio y sobreescritura
        archivo.seekp(posicionRegistro, ios::beg);
        archivo.write(reinterpret_cast<const char*>(&temp), sizeof(Cliente));
        
        // Cerramos para actualizar el header por separado
        archivo.close();

        // 5. Actualización del Header (sumamos uno a los activos)
        h.registrosActivos++;
        actualizarHeader("clientes.bin", h);

        cout << "\n[OK] Cliente restaurado exitosamente." << endl;
    } else {
        cout << "\n[!] Operacion cancelada. El cliente permanece en papelera." << endl;
        archivo.close();
    }

    pausarYlimpiarpantalla();
}
void registrarCompra() {
    Header hProd = leerHeader("productos.bin");
    if (hProd.registrosActivos == 0) {
        cout << "\n[!] ERROR: No hay productos registrados." << endl;
        pausarYlimpiarpantalla(); return;
    }

    Header hTrans = leerHeader("transacciones.bin");
    Transaccion t;
    
    // Inicialización
    t.id = hTrans.ProximoId;
    t.tipo = 'C';
    t.activo = true;
    t.total = 0;
    t.cantidaditems = 0;
    obtenerFechaActual(t.fecha);
    t.fechaCreacion = time(0);
    t.fechaUltimaModificacion = time(0);

    cout << "\n--- REGISTRAR NUEVA COMPRA (ID: " << t.id << ") ---" << endl;
    mostrarInstruccionESC();
    
    if (!pedirEnteroCancelable("ID del Proveedor", t.idRelacionado)) return;
    if (!existeProveedor(t.idRelacionado)) { // Asegúrate de tener esta validación
        cout << "[!] Proveedor no encontrado." << endl;
        pausarYlimpiarpantalla(); return;
    }

    if (!pedirTextoCancelable("Breve descripcion", t.descripcion, 200)) return;

    int numItems;
    do {
        if (!pedirEnteroCancelable("Cantidad de productos diferentes", numItems)) return;
        if (numItems <= 0 || numItems > 30) cout << "[!] Error: Maximo 30 items.\n";
    } while (numItems <= 0 || numItems > 30);

    // 1. CAPTURA TEMPORAL (No tocamos archivos todavía)
    for (int i = 0; i < numItems; i++) {
        cout << "\n-- Item " << (i + 1) << " --" << endl;
        int idProd;
        if (!pedirEnteroCancelable("ID del Producto", idProd)) return;

        int idxProd = buscarProductoPorId(idProd);
        if (idxProd == -1) {
            cout << "[!] Producto no existe.\n"; i--; continue;
        }

        // Validar duplicados en la misma factura
        bool yaExiste = false;
        for(int j = 0; j < t.cantidaditems; j++) {
            if(t.detalles[j].idProducto == idProd) yaExiste = true;
        }
        if(yaExiste) {
            cout << "[!] Producto ya agregado.\n"; i--; continue;
        }

        int cant;
        if (!pedirEnteroCancelable("Cantidad adquirida", cant)) return;
        
        float precioU;
        cout << "Costo unitario ($): ";
        cin >> precioU; vaciarBuffer();
        if (precioU <= 0) { cout << "[!] Precio invalido.\n"; i--; continue; }

        // Guardamos en el struct temporal
        t.detalles[t.cantidaditems].idProducto = idProd;
        t.detalles[t.cantidaditems].cantidad = cant;
        t.detalles[t.cantidaditems].precioUnitario = precioU;
        
        t.total += (cant * precioU);
        t.cantidaditems++;
    }

    // 2. VALIDACIÓN DE CAJA
    if (!hayDineroEnCaja(t.total)) {
        cout << "\n[!] ERROR: Saldo insuficiente en tienda ($" << t.total << " requeridos)." << endl;
        pausarYlimpiarpantalla(); return;
    }

    // 3. RESUMEN Y CONFIRMACIÓN
    system("cls");
    cout << "==================================================" << endl;
    cout << "          RESUMEN DE COMPRA #" << t.id << endl;
    cout << "==================================================" << endl;
    cout << " PROVEEDOR ID: " << t.idRelacionado << " | TOTAL: $" << t.total << endl;
    cout << "--------------------------------------------------" << endl;
    
    char confirm;
    cout << "¿Desea procesar esta compra y pagar al proveedor? (S/N): ";
    cin >> confirm; vaciarBuffer();

    if (toupper(confirm) == 'S') {
        // 4. ACTUALIZACIÓN FÍSICA (Aquí recién escribimos en disco)
        
        // A. Productos (DENTRO DEL FOR)
        for (int i = 0; i < t.cantidaditems; i++) {
            int idx = buscarProductoPorId(t.detalles[i].idProducto);
            // Usamos lógica de apertura y cierre rápida
            fstream archivoProd("productos.bin", ios::in | ios::out | ios::binary);
            long pos = sizeof(Header) + (idx * sizeof(Producto));
            Producto p;
            archivoProd.seekg(pos, ios::beg);
            archivoProd.read(reinterpret_cast<char*>(&p), sizeof(Producto));
            
            p.totalComprado += t.detalles[i].cantidad;
            p.fechaUltimaModificacion = time(0);
            
            archivoProd.seekp(pos, ios::beg);
            archivoProd.write(reinterpret_cast<const char*>(&p), sizeof(Producto));
            archivoProd.close();
        }

        // B. Proveedor (FUERA DEL FOR)
        int idxP = buscarIndiceProveedor(t.idRelacionado);
        if (idxP != -1) {
            Proveedor prov = leerProveedorPorIndice(idxP);
            prov.totalSuministros++;
            prov.totalPagado += t.total;
            prov.fechaUltimaModificacion = time(0);
            escribirProveedorPorIndice(idxP, prov);
        }

        // C. Transacción, Header y Caja
        ofstream archivoTrans("transacciones.bin", ios::app | ios::binary);
        archivoTrans.write(reinterpret_cast<const char*>(&t), sizeof(Transaccion));
        archivoTrans.close();

        hTrans.cantidadRegistros++;
        hTrans.ProximoId++;
        hTrans.registrosActivos++;
        actualizarHeader("transacciones.bin", hTrans);
        actualizarCajaTienda(t.total, false); // Resta dinero

        cout << "\n[OK] Compra exitosa. Stock y Caja actualizados." << endl;
    } else {
        cout << "\n[i] Compra cancelada. No se realizo ningun pago." << endl;
    }
    
    pausarYlimpiarpantalla();
}

void registrarVenta() {
    Header hProd = leerHeader("productos.bin");
    Header hClie = leerHeader("clientes.bin");
    
    if (hProd.registrosActivos == 0 || hClie.registrosActivos == 0) {
        cout << "\n[!] ERROR: Faltan datos (Productos o Clientes) para operar." << endl;
        pausarYlimpiarpantalla();
        return;
    }

    Header hTrans = leerHeader("transacciones.bin");
    Transaccion t;
    
    // Inicialización básica
    t.id = hTrans.ProximoId;
    t.tipo = 'V';
    t.activo = true;
    t.total = 0;
    t.cantidaditems = 0;
    obtenerFechaActual(t.fecha);
    t.fechaCreacion = time(0);
    t.fechaUltimaModificacion = time(0);

    cout << "\n--- NUEVA VENTA (ID: " << t.id << ") ---" << endl;
    mostrarInstruccionESC();

    // 1. Selección de Cliente
    if (!pedirEnteroCancelable("ID del Cliente", t.idRelacionado)) return;
    if (!existeCliente(t.idRelacionado)) {
        cout << "[!] Cliente no encontrado. Operacion abortada." << endl;
        pausarYlimpiarpantalla();
        return;
    }

    if (!pedirTextoCancelable("Descripcion", t.descripcion, 200)) return;

    // 2. Cantidad de renglones/items
    int numItems;
    do {
        if (!pedirEnteroCancelable("Cantidad de productos diferentes", numItems)) return;
        if (numItems <= 0 || numItems > 30) cout << "[!] Debe ser entre 1 y 30." << endl;
    } while (numItems <= 0 || numItems > 30);

    // 3. Captura de Items (Sin modificar archivos todavia)
    for (int i = 0; i < numItems; i++) {
        cout << "\n-- Item " << (i + 1) << " --" << endl;
        int idP;
        if (!pedirEnteroCancelable("ID Producto", idP)) return;

        int idx = buscarProductoPorId(idP);
        if (idx == -1) {
            cout << "[!] Producto no existe. Intente otro ID." << endl;
            i--; continue;
        }

        // Validar duplicados en la misma factura
        bool duplicado = false;
        for(int j=0; j < t.cantidaditems; j++) {
            if(t.detalles[j].idProducto == idP) duplicado = true;
        }
        if(duplicado) {
            cout << "[!] El producto ya esta en la lista." << endl;
            i--; continue;
        }

        // Verificar Stock
        Producto p = leerProductoPorIndice(idx); // Funcion auxiliar para leer 1 solo registro
        int disponible = (p.stockInicial + p.totalComprado) - p.totalVendido;

        int cant;
        if (!pedirEnteroCancelable("Cantidad", cant)) return;
        
        if (cant <= 0 || cant > disponible) {
            cout << "[!] Cantidad invalida o Stock insuficiente (Disp: " << disponible << ")" << endl;
            i--; continue;
        }

        // Llenar detalle temporal
        t.detalles[t.cantidaditems].idProducto = idP;
        t.detalles[t.cantidaditems].cantidad = cant;
        t.detalles[t.cantidaditems].precioUnitario = p.precio;
        
        t.total += (cant * p.precio);
        t.cantidaditems++;
    }

    // 4. CUADRO DE RESUMEN Y CONFIRMACIÓN
    system("cls");
    cout << "==================================================" << endl;
    cout << "             RESUMEN DE LA VENTA #" << t.id << endl;
    cout << "==================================================" << endl;
    cout << " CLIENTE ID: " << t.idRelacionado << endl;
    cout << " FECHA:      " << t.fecha << endl;
    cout << "--------------------------------------------------" << endl;
    cout << left << setw(10) << "ID PROD" << setw(20) << "CANT." << "SUBTOTAL" << endl;
    
    for (int i = 0; i < t.cantidaditems; i++) {
        cout << left << setw(10) << t.detalles[i].idProducto 
             << setw(20) << t.detalles[i].cantidad 
             << "$ " << (t.detalles[i].cantidad * t.detalles[i].precioUnitario) << endl;
    }
    cout << "--------------------------------------------------" << endl;
    cout << " TOTAL A COBRAR: $ " << fixed << setprecision(2) << t.total << endl;
    cout << "==================================================" << endl;
    
    char confirm;
    cout << "¿Desea procesar y guardar esta venta? (S/N): ";
    cin >> confirm;
    vaciarBuffer();

    if (toupper(confirm) == 'S') {
        // 5. ACTUALIZACIÓN FINAL (Recién aquí tocamos los archivos)
        
        // A. Actualizar stocks de productos
        for (int i = 0; i < t.cantidaditems; i++) {
    int idx = buscarProductoPorId(t.detalles[i].idProducto);
    
    // Abrimos el archivo para modificarlo (in/out)
    fstream archivo("productos.bin", ios::in | ios::out | ios::binary);
    if (archivo) {
        // 1. Calculamos la posición del producto
        long posicion = sizeof(Header) + (idx * sizeof(Producto));
        
        // 2. Leemos el producto actual
        Producto p;
        archivo.seekg(posicion, ios::beg);
        archivo.read(reinterpret_cast<char*>(&p), sizeof(Producto));

        // 3. Modificamos el acumulador de ventas
        p.totalVendido += t.detalles[i].cantidad;
        p.fechaUltimaModificacion = time(0);

        // 4. Volvemos a la posición para SOBRESCRIBIRLO
        archivo.seekp(posicion, ios::beg);
        archivo.write(reinterpret_cast<const char*>(&p), sizeof(Producto));
        
        archivo.close();
    }
}

        int idxCli = buscarClientePorId(t.idRelacionado);
        if (idxCli != -1) {
            // 1. Leemos los datos actuales del cliente
            Cliente cli = leerClientePorIndice(idxCli);
            
            // 2. Sumamos la nueva venta a sus estadísticas
            cli.totalComprasRealizadas++;        // Aumenta el contador de visitas
            cli.montoTotalGastado += t.total;    // Suma el dinero al acumulador histórico
            cli.fechaUltimaModificacion = time(0);
            
            // 3. Sobrescribimos el registro en el archivo
            escribirClientePorIndice(idxCli, cli);
        }

        // B. Guardar Transacción
        ofstream fTrans("transacciones.bin", ios::app | ios::binary);
        fTrans.write(reinterpret_cast<const char*>(&t), sizeof(Transaccion));
        fTrans.close();

        // C. Actualizar Header y Caja
        hTrans.cantidadRegistros++;
        hTrans.ProximoId++;
        hTrans.registrosActivos++;
        actualizarHeader("transacciones.bin", hTrans);
        actualizarCajaTienda(t.total, true); 

        cout << "\n[OK] Venta procesada exitosamente." << endl;
    } else {
        cout << "\n[i] Venta cancelada por el usuario. No se afecto el stock." << endl;
    }

    pausarYlimpiarpantalla();
}

void buscarTransacciones() {
    cout << "\n--- BUSCAR TRANSACCION ---" << endl;
    cout << "1. Por ID de Transaccion\n2. Cancelar\nSeleccione: ";
    int op; cin >> op; vaciarBuffer();

    if (op == 1) {
        int id = pedirEntero("Ingrese ID: ");
        mostrarTransaccion(id);
    }
    pausarYlimpiarpantalla();
}

void listarTransacciones() {
    ifstream archivo("transacciones.bin", ios::binary);
    if (!archivo) {
        cout << "[!] Error al abrir historial de transacciones.\n"; return;
    }

    Header h;
    archivo.read(reinterpret_cast<char*>(&h), sizeof(Header));

    if (h.cantidadRegistros == 0) {
        cout << "\n[i] No hay transacciones registradas.\n";
        archivo.close(); pausarYlimpiarpantalla(); return;
    }

    cout << "\n================================================================================" << endl;
    cout << left << setw(8) << "ID" << setw(10) << "TIPO" << setw(15) << "FECHA" 
         << setw(15) << "RELACIONADO" << setw(15) << "TOTAL ($)" << "ESTADO" << endl;
    cout << "================================================================================" << endl;

    Transaccion t;
    while (archivo.read(reinterpret_cast<char*>(&t), sizeof(Transaccion))) {
        cout << left << setw(8) << t.id 
             << setw(10) << (t.tipo == 'C' ? "COMPRA" : "VENTA") 
             << setw(15) << t.fecha 
             << setw(15) << t.idRelacionado 
             << setw(15) << fixed << setprecision(2) << t.total 
             << (t.activo ? "ACTIVO" : "ANULADA") << endl;
    }
    cout << "================================================================================" << endl;
    
    archivo.close();
    pausarYlimpiarpantalla();
}

void cancelarTransaccion() {
    if (!tieneDatos("transacciones.bin")) {
        cout << "[!] No hay transacciones registradas.\n";
        pausarYlimpiarpantalla(); return;
    }
    
    int idBuscar;
    mostrarInstruccionESC();
    // Usamos la versión cancelable para ser consistentes
    if (!pedirEnteroCancelable("Ingrese el ID de la transaccion a anular", idBuscar)) return;

    // ... (Tu lógica de búsqueda de índice se mantiene igual) ...
    int indice = -1;
    ifstream archivoLectura("transacciones.bin", ios::binary);
    Header hL; archivoLectura.read(reinterpret_cast<char*>(&hL), sizeof(Header));
    Transaccion tL;
    for (int i = 0; i < hL.cantidadRegistros; i++) {
        archivoLectura.read(reinterpret_cast<char*>(&tL), sizeof(Transaccion));
        if (tL.id == idBuscar) { indice = i; break; }
    }
    archivoLectura.close();

    if (indice == -1) {
        cout << "[!] Transaccion no encontrada.\n";
        pausarYlimpiarpantalla(); return;
    }

    fstream archivo("transacciones.bin", ios::in | ios::out | ios::binary);
    long posicion = sizeof(Header) + (indice * sizeof(Transaccion));
    archivo.seekg(posicion, ios::beg);
    Transaccion t;
    archivo.read(reinterpret_cast<char*>(&t), sizeof(Transaccion));

    if (!t.activo) {
        cout << "[!] Esta transaccion ya se encuentra anulada.\n";
        archivo.close(); pausarYlimpiarpantalla(); return;
    }

    // Mostramos que vamos a anular
    mostrarTransaccion(t.id);
    cout << "\n¿Esta seguro de ANULAR esta transaccion y revertir el stock/caja? (S/N): ";
    char conf; 
    cin >> conf; 
    vaciarBuffer();

    if (toupper(conf) == 'S') {
        // 1. REVERSIÓN DE STOCK
        for (int i = 0; i < t.cantidaditems; i++) {
            int idxProd = buscarProductoPorId(t.detalles[i].idProducto);
            if (idxProd != -1) {
                fstream fProd("productos.bin", ios::in | ios::out | ios::binary);
                long posP = sizeof(Header) + (idxProd * sizeof(Producto));
                fProd.seekg(posP, ios::beg);
                Producto p;
                fProd.read(reinterpret_cast<char*>(&p), sizeof(Producto));

                if (t.tipo == 'C') p.totalComprado -= t.detalles[i].cantidad;
                else if (t.tipo == 'V') p.totalVendido -= t.detalles[i].cantidad;
                
                fProd.seekp(posP, ios::beg);
                fProd.write(reinterpret_cast<char*>(&p), sizeof(Producto));
                fProd.close();
            }
        }

        // 2. REVERSIÓN DE CAJA (IMPORTANTE)
        if (t.tipo == 'C') {
            actualizarCajaTienda(t.total, true);  // Si anulamos compra, recuperamos dinero
        } else {
            actualizarCajaTienda(t.total, false); // Si anulamos venta, devolvemos dinero
        }

    if (t.tipo == 'V') {
    int idxCli = buscarClientePorId(t.idRelacionado);
    if (idxCli != -1) {
        Cliente c = leerClientePorIndice(idxCli);
        c.totalComprasRealizadas--;
        c.montoTotalGastado -= t.total;
        c.fechaUltimaModificacion = time(0);
        escribirClientePorIndice(idxCli, c);
         }
    } else if (t.tipo == 'C') {
    int idxProv = buscarIndiceProveedor(t.idRelacionado);
    if (idxProv != -1) {
        Proveedor p = leerProveedorPorIndice(idxProv);
        p.totalSuministros--;
        p.totalPagado -= t.total;
        p.fechaUltimaModificacion = time(0);
        escribirProveedorPorIndice(idxProv, p);
         }
    }

        // 3. MARCAR COMO INACTIVA
        t.activo = false;
        t.fechaUltimaModificacion = time(0);
        archivo.seekp(posicion, ios::beg);
        archivo.write(reinterpret_cast<char*>(&t), sizeof(Transaccion));
        
        Header h = leerHeader("transacciones.bin");
        h.registrosActivos--;
        actualizarHeader("transacciones.bin", h);

        cout << "\n[OK] Transaccion anulada: Stock y Caja ajustados.\n";
    }
    archivo.close();
    pausarYlimpiarpantalla();
}

void restaurarTransacciones() {
    if (!tieneDatos("transacciones.bin")) {
        cout << "[!] No hay transacciones registradas.\n";
        pausarYlimpiarpantalla(); return;
    }

    int idBuscar;
    mostrarInstruccionESC();
    // 1. Uso de función cancelable con ESC
    if (!pedirEnteroCancelable("Ingrese el ID de la transaccion anulada a restaurar", idBuscar)) return;

    // --- Búsqueda del índice ---
    int indice = -1;
    ifstream archivoLectura("transacciones.bin", ios::binary);
    Header hL; archivoLectura.read(reinterpret_cast<char*>(&hL), sizeof(Header));
    Transaccion tL;
    for (int i = 0; i < hL.cantidadRegistros; i++) {
        archivoLectura.read(reinterpret_cast<char*>(&tL), sizeof(Transaccion));
        if (tL.id == idBuscar) { indice = i; break; }
    }
    archivoLectura.close();

    if (indice == -1) {
        cout << "[!] Transaccion no encontrada.\n";
        pausarYlimpiarpantalla(); return;
    }

    fstream archivo("transacciones.bin", ios::in | ios::out | ios::binary);
    long posicion = sizeof(Header) + (indice * sizeof(Transaccion));
    archivo.seekg(posicion, ios::beg);
    Transaccion t;
    archivo.read(reinterpret_cast<char*>(&t), sizeof(Transaccion));

    if (t.activo) {
        cout << "[!] La transaccion ya se encuentra ACTIVA.\n";
        archivo.close(); pausarYlimpiarpantalla(); return;
    }

    // 2. Verificación de viabilidad (Stock y Caja)
    if (t.tipo == 'V') {
        // Para restaurar una Venta: Necesitamos tener Stock
        for (int i = 0; i < t.cantidaditems; i++) {
            int idxProd = buscarProductoPorId(t.detalles[i].idProducto);
            if (idxProd != -1) {
                Producto p = leerProductoPorIndice(idxProd);
                int stockActual = (p.stockInicial + p.totalComprado) - p.totalVendido;
                
                if (stockActual < t.detalles[i].cantidad) {
                    cout << "\n[!] ERROR: No hay stock suficiente de '" << p.nombre << "' para restaurar la venta.\n";
                    archivo.close(); pausarYlimpiarpantalla(); return;
                }
            }
        }
    } else if (t.tipo == 'C') {
        if (!hayDineroEnCaja(t.total)) { 
            cout << "\n[!] ERROR: No se puede restaurar la compra." << endl;
            cout << "[!] Saldo insuficiente en la tienda para cubrir este gasto." << endl;
            archivo.close(); 
            pausarYlimpiarpantalla(); 
            return;
         }
    }

    cout << "\n¿Confirmar RESTAURACION de la transaccion #" << t.id << "? (S/N): ";
    char conf; cin >> conf; vaciarBuffer();


    if (t.tipo == 'V') {
    int idxCli = buscarClientePorId(t.idRelacionado);
    if (idxCli != -1) {
        Cliente c = leerClientePorIndice(idxCli);
        c.totalComprasRealizadas++;
        c.montoTotalGastado += t.total;
        c.fechaUltimaModificacion = time(0);
        escribirClientePorIndice(idxCli, c);
         }
    } else if (t.tipo == 'C') {
    int idxProv = buscarIndiceProveedor(t.idRelacionado);
    if (idxProv != -1) {
        Proveedor p = leerProveedorPorIndice(idxProv);
        p.totalSuministros++;
        p.totalPagado += t.total;
        p.fechaUltimaModificacion = time(0);
        escribirProveedorPorIndice(idxProv, p);
            }
        }
    if (toupper(conf) == 'S') {
        // 3. REACTIVACIÓN DE STOCK
        for (int i = 0; i < t.cantidaditems; i++) {
            int idxProd = buscarProductoPorId(t.detalles[i].idProducto);
            if (idxProd != -1) {
                fstream fProd("productos.bin", ios::in | ios::out | ios::binary);
                long posP = sizeof(Header) + (idxProd * sizeof(Producto));
                fProd.seekg(posP, ios::beg);
                Producto p;
                fProd.read(reinterpret_cast<char*>(&p), sizeof(Producto));

                if (t.tipo == 'C') p.totalComprado += t.detalles[i].cantidad;
                else if (t.tipo == 'V') p.totalVendido += t.detalles[i].cantidad;
                
                fProd.seekp(posP, ios::beg);
                fProd.write(reinterpret_cast<char*>(&p), sizeof(Producto));
                fProd.close();
            }
        }

        // 4. REACTIVACIÓN EN CAJA (NUEVO)
        if (t.tipo == 'C') {
            actualizarCajaTienda(t.total, false); // Vuelve a restar el dinero (Gasto)
        } else {
            actualizarCajaTienda(t.total, true);  // Vuelve a sumar el dinero (Ingreso)
        }

        // 5. MARCAR COMO ACTIVA
        t.activo = true;
        t.fechaUltimaModificacion = time(0);
        archivo.seekp(posicion, ios::beg);
        archivo.write(reinterpret_cast<char*>(&t), sizeof(Transaccion));
        
        Header h = leerHeader("transacciones.bin");
        h.registrosActivos++;
        actualizarHeader("transacciones.bin", h);

        cout << "\n[OK] Transaccion restaurada. Inventario y Caja sincronizados.\n";
    }

    archivo.close();
    pausarYlimpiarpantalla();
}

void verificarIntegridadReferencial() {
    cout << "\n==================================================" << endl;
    cout << "     DIAGNOSTICO DE INTEGRIDAD DE DATOS           " << endl;
    cout << "==================================================" << endl;
    
    int erroresEncontrados = 0;

    // 1. Verificar Productos -> Proveedores
    cout << "[i] Verificando relacion: Productos -> Proveedores..." << endl;
    ifstream archProd("productos.bin", ios::binary);
    if (archProd) {
        Header hProd; archProd.read(reinterpret_cast<char*>(&hProd), sizeof(Header));
        Producto p;
        for (int i = 0; i < hProd.cantidadRegistros; i++) {
            archProd.read(reinterpret_cast<char*>(&p), sizeof(Producto));
            if (p.activo) {
                // Si el producto está activo, su proveedor DEBE existir y estar activo
                if (!existeProveedor(p.idProveedor)) {
                    cout << "  [!] ALERTA: Producto ID " << p.id << " ('" << p.nombre << "') apunta al Proveedor ID " 
                         << p.idProveedor << " el cual NO EXISTE o fue eliminado." << endl;
                    erroresEncontrados++;
                }
            }
        }
        archProd.close();
    }

    // 2. Verificar Transacciones -> Entidades y Productos
    cout << "[i] Verificando relacion: Transacciones -> Clientes/Proveedores/Productos..." << endl;
    ifstream archTrans("transacciones.bin", ios::binary);
    if (archTrans) {
        Header hTrans; archTrans.read(reinterpret_cast<char*>(&hTrans), sizeof(Header));
        Transaccion t;
        for (int i = 0; i < hTrans.cantidadRegistros; i++) {
            archTrans.read(reinterpret_cast<char*>(&t), sizeof(Transaccion));
            if (t.activo) {
                // Verificar Entidad Relacionada
                if (t.tipo == 'C' && !existeProveedor(t.idRelacionado)) {
                    cout << "  [!] ALERTA: Transaccion COMPRA ID " << t.id << " apunta al Proveedor ID " 
                         << t.idRelacionado << " (No encontrado/Inactivo)." << endl;
                    erroresEncontrados++;
                } else if (t.tipo == 'V' && !existeCliente(t.idRelacionado)) {
                    cout << "  [!] ALERTA: Transaccion VENTA ID " << t.id << " apunta al Cliente ID " 
                         << t.idRelacionado << " (No encontrado/Inactivo)." << endl;
                    erroresEncontrados++;
                }

                // Verificar Detalles (Productos en la factura)
                for (int j = 0; j < t.cantidaditems; j++) {
                    if (!existeProducto(t.detalles[j].idProducto)) {
                        cout << "  [!] ALERTA: Transaccion ID " << t.id << " contiene el Producto ID " 
                             << t.detalles[j].idProducto << " (No encontrado/Inactivo)." << endl;
                        erroresEncontrados++;
                    }
                }
            }
        }
        archTrans.close();
    }

    cout << "--------------------------------------------------" << endl;
    if (erroresEncontrados == 0) {
        cout << "[OK] Excelente. No se detectaron referencias rotas." << endl;
        cout << "[OK] La integridad de la base de datos esta al 100%." << endl;
    } else {
        cout << "[!] DIAGNOSTICO: Se encontraron " << erroresEncontrados << " errores de integridad." << endl;
        cout << "[!] Se recomienda revisar los registros manuales." << endl;
    }
    cout << "==================================================" << endl;
    
    pausarYlimpiarpantalla();
}

void generarReportes() {
    if(!tieneDatos("productos.bin") && !tieneDatos("transacciones.bin")) {
        cout << "\n[!] No hay datos suficientes para generar reportes analíticos." << endl;
        cout << "[!] Asegurese de tener productos registrados y transacciones realizadas." << endl;
        pausarYlimpiarpantalla();
        return;
    }
    int opcion;
    do {
        cout << "\n╔═══════════════════════════════════════════╗" << endl;
        cout << "║           REPORTES ANALÍTICOS             ║" << endl;
        cout << "╚═══════════════════════════════════════════╝" << endl;
        cout << "1. Reporte de Productos con Stock Critico" << endl;
        cout << "2. Historial de Compras por Cliente" << endl;
        cout << "0. Volver" << endl;
        opcion = pedirEntero("Seleccione un reporte: ");

        if (opcion == 1) {
            // REPORTE 1: STOCK CRÍTICO
            cout << "\n--- PRODUCTOS CON STOCK CRITICO O AGOTADO ---" << endl;
            ifstream archivo("productos.bin", ios::binary);
            if (!archivo) { cout << "[!] Error de archivo.\n"; continue; }
            
            Header h; archivo.read(reinterpret_cast<char*>(&h), sizeof(Header));
            Producto p;
            bool hayCriticos = false;
            
            cout << left << setw(8) << "ID" << setw(30) << "PRODUCTO" << setw(15) << "STOCK ACTUAL" << "STOCK MINIMO" << endl;
            cout << "----------------------------------------------------------------------" << endl;
            
            for (int i = 0; i < h.cantidadRegistros; i++) {
                archivo.read(reinterpret_cast<char*>(&p), sizeof(Producto));
                if (p.activo) {
                    int stockActual = calcularStockActual(p);
                    if (stockActual <= p.stockminimo) {
                        hayCriticos = true;
                        cout << left << setw(8) << p.id << setw(30) << p.nombre 
                             << setw(15) << stockActual << p.stockminimo << endl;
                    }
                }
            }
            if (!hayCriticos) cout << "[OK] Todos los productos tienen un nivel de stock saludable." << endl;
            archivo.close();
            pausarYlimpiarpantalla();

        } else if (opcion == 2) {
            // REPORTE 2: HISTORIAL DE CLIENTE
            int idCliente = pedirEntero("\nIngrese ID del Cliente para ver su historial: ");
            if (!existeCliente(idCliente)) {
                cout << "[!] Cliente no encontrado o inactivo." << endl;
            } else {
                mostrarcliente(idCliente); // Tu función existente que imprime los datos básicos
                cout << "\n--- HISTORIAL DE COMPRAS DEL CLIENTE ---" << endl;
                
                ifstream archTrans("transacciones.bin", ios::binary);
                if (archTrans) {
                    Header ht; archTrans.read(reinterpret_cast<char*>(&ht), sizeof(Header));
                    Transaccion t;
                    bool tieneCompras = false;
                    
                    for (int i = 0; i < ht.cantidadRegistros; i++) {
                        archTrans.read(reinterpret_cast<char*>(&t), sizeof(Transaccion));
                        // Filtramos ventas ('V') asociadas a este cliente y que no estén anuladas
                        if (t.tipo == 'V' && t.idRelacionado == idCliente && t.activo) {
                            tieneCompras = true;
                            cout << " > Fecha: " << t.fecha << " | Factura #" << t.id << " | Total: $" << fixed << setprecision(2) << t.total << endl;
                            // Imprimir el detalle de los productos recuperando el nombre
                            for(int j = 0; j < t.cantidaditems; j++){
                                char nombreProd[100];
                                obtenerNombreGenerico("productos.bin", t.detalles[j].idProducto, nombreProd);
                                cout << "    - " << t.detalles[j].cantidad << "x " << nombreProd << " ($" << t.detalles[j].precioUnitario << " c/u)" << endl;
                            }
                        }
                    }
                    if(!tieneCompras) cout << "  [i] Este cliente aun no ha realizado compras." << endl;
                    archTrans.close();
                }
            }
            pausarYlimpiarpantalla();
        }
    } while (opcion != 0);
}

void crearBackup() {
    // Generar prefijo de archivo basado en la fecha y hora actual del PC
    time_t t_actual = time(0);
    struct tm * tiempo = localtime(&t_actual);
    char prefijo[50];
    // Formato: backup_YYYYMMDD_HHMM_
    sprintf(prefijo, "backup_%04d%02d%02d_%02d%02d_", 
            (tiempo->tm_year + 1900), (tiempo->tm_mon + 1), tiempo->tm_mday, 
            tiempo->tm_hour, tiempo->tm_min);

    const char* archivos[] = {
        "tienda.bin", 
        "productos.bin", 
        "proveedores.bin", 
        "clientes.bin", 
        "transacciones.bin"
    };

    cout << "\n==================================================" << endl;
    cout << "          CREANDO RESPALDO DE SEGURIDAD           " << endl;
    cout << "==================================================" << endl;
    
    int respaldados = 0;

    for (int i = 0; i < 5; i++) {
        char nombreDestino[100];
        strcpy(nombreDestino, prefijo);
        strcat(nombreDestino, archivos[i]); // Concatena el prefijo con el nombre original

        ifstream origen(archivos[i], ios::binary);
        if (origen) {
            ofstream destino(nombreDestino, ios::binary);
            // Copia extremadamente rápida usando el buffer nativo de C++
            destino << origen.rdbuf(); 
            
            cout << "[OK] Archivo asegurado: " << nombreDestino << endl;
            destino.close();
            respaldados++;
        } else {
            cout << "[!] Archivo vacio o inexistente (ignorado): " << archivos[i] << endl;
        }
        origen.close();
    }

    cout << "--------------------------------------------------" << endl;
    if (respaldados > 0) {
        cout << "[EXITO] Se genero copia de seguridad de " << respaldados << " archivos." << endl;
    } else {
        cout << "[!] No se pudo respaldar la informacion. Verifique los archivos origen." << endl;
    }
    cout << "==================================================" << endl;
    
    pausarYlimpiarpantalla();
}

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
        cout << "5. Verificar Integridad referencial" << endl;
        cout << "6. Reportes Analiticos" << endl;
        cout << "7. Crear Backup Manual" << endl;
        cout << "8. Salir" << endl;
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
                        case 6: restaurarCliente(); break;
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
                        case 6: restaurarTransacciones(); break;
                    }
                } while(subOp != 0);
                break;
            
            case 5:
                verificarIntegridadReferencial();
                break;

            case 6:
                generarReportes();
                break;
            
            case 7:
                crearBackup();
                break;
            case 8:
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
   