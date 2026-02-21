#include <iostream>
#include <iomanip>
#include <limits>
#include <string.h>
#include <windows.h>
#include <cstring>
using namespace std;   

/*
# REPÚBLICA BOLIVARIANA DE VENEZUELA
## UNIVERSIDAD RAFAEL URDANETA
## FACULTAD DE INGENIERÍA
## ESCUELA DE INGENIERÍA DE COMPUTACIÓN

**ASIGNATURA:** PROGRAMACIÓN 2  
**PROFESOR:** ING. VICTOR KNEIDER  
**TRIMESTRE:** 2026-A

---

# PROYECTO 1: SISTEMA DE GESTIÓN DE INVENTARIO

## DESCRIPCIÓN GENERAL

Desarrolla un sistema completo de gestión de inventario que permita administrar productos, proveedores, clientes y transacciones comerciales. El sistema debe demostrar dominio avanzado de **punteros, memoria dinámica y estructuras** en C++, implementando operaciones CRUD completas, manejo robusto de memoria y algoritmos de búsqueda eficientes.

## OBJETIVOS DE APRENDIZAJE

1. Implementar estructuras de datos dinámicas con punteros
2. Gestionar memoria dinámica de forma eficiente y segura
3. Aplicar operaciones CRUD (Crear, Leer, Actualizar, Eliminar)
4. Desarrollar algoritmos de búsqueda y validación
5. Implementar redimensionamiento automático de arrays dinámicos
6. Aplicar buenas prácticas de programación y documentación

## TECNOLOGÍAS REQUERIDAS

- **Lenguaje:** C++ (estándar C++11 o superior)
- **Compilador:** g++ o cualquier compilador compatible
- **Gestión de memoria:** Manual (new/delete)

---

## ESPECIFICACIONES TÉCNICAS

### 1. ESTRUCTURAS DE DATOS OBLIGATORIAS BASE

Se propone una estructura de datos base y tienen libertad para añadir atributos a cualquier estructura del modelo de datos.

#### 1.1 Estructura Producto
```cpp
struct Producto {
    int id;                    // Identificador único (autoincremental)
    char codigo[20];           // Código del producto (ej: "PROD-001")
    char nombre[100];          // Nombre del producto
    char descripcion[200];     // Descripción del producto
    int idProveedor;           // ID del proveedor asociado
    float precio;              // Precio unitario
    int stock;                 // Cantidad en inventario
    char fechaRegistro[11];    // Formato: YYYY-MM-DD
};
```

#### 1.2 Estructura Proveedor
```cpp
struct Proveedor {
    int id;                    // Identificador único (autoincremental)
    char nombre[100];          // Nombre del proveedor
    char rif[20];              // RIF o identificación fiscal
    char telefono[20];         // Teléfono de contacto
    char email[100];           // Correo electrónico
    char direccion[200];       // Dirección física
    char fechaRegistro[11];    // Formato: YYYY-MM-DD
};
```

#### 1.3 Estructura Cliente
```cpp
struct Cliente {
    int id;                    // Identificador único (autoincremental)
    char nombre[100];          // Nombre completo del cliente
    char cedula[20];           // Cédula o RIF
    char telefono[20];         // Teléfono de contacto
    char email[100];           // Correo electrónico
    char direccion[200];       // Dirección física
    char fechaRegistro[11];    // Formato: YYYY-MM-DD
};
```

#### 1.4 Estructura Transacción (CASO ESPECIAL: Esta estructura puede separarse como se comentó en clase, tienen libertad de hacerlo.)
```cpp
struct Transaccion {
    int id;                    // Identificador único (autoincremental)
    char tipo[10];             // "COMPRA" o "VENTA"
    int idProducto;            // ID del producto involucrado
    int idRelacionado;         // ID del proveedor (compra) o cliente (venta)
    int cantidad;              // Cantidad de unidades
    float precioUnitario;      // Precio por unidad en esta transacción
    float total;               // cantidad * precioUnitario
    char fecha[11];            // Formato: YYYY-MM-DD
    char descripcion[200];     // Notas adicionales (opcional)
};
```

#### 1.5 Estructura Principal: Tienda
```cpp
struct Tienda {
    char nombre[100];          // Nombre de la tienda
    char rif[20];              // RIF de la tienda
    
    // Arrays dinámicos de entidades
    Producto* productos;
    int numProductos;
    int capacidadProductos;
    
    Proveedor* proveedores;
    int numProveedores;
    int capacidadProveedores;
    
    Cliente* clientes;
    int numClientes;
    int capacidadClientes;
    
    Transaccion* transacciones;
    int numTransacciones;
    int capacidadTransacciones;
    
    // Contadores para IDs autoincrementales
    int siguienteIdProducto;
    int siguienteIdProveedor;
    int siguienteIdCliente;
    int siguienteIdTransaccion;
};
```

---

### 2. FUNCIONES OBLIGATORIAS

#### 2.1 Funciones de Inicialización y Liberación
```cpp
// Inicializa la tienda con capacidad inicial
void inicializarTienda(Tienda* tienda, const char* nombre, const char* rif);

// Libera toda la memoria dinámica
void liberarTienda(Tienda* tienda);
```

**Requisitos:**
- Capacidad inicial de cada array: 5 elementos
- Todos los contadores deben inicializarse en 0
- Los IDs deben comenzar en 1

---

#### 2.2 Funciones CRUD - PRODUCTOS

##### 2.2.1 Crear Producto
```cpp
void crearProducto(Tienda* tienda);
```

**Requisitos:**
1. Permitir cancelación en cualquier momento (usuario ingresa "CANCELAR" o "0") 
2. Solicitar confirmación antes de guardar
3. Validaciones obligatorias:
   - Código no duplicado
   - Precio > 0
   - Stock >= 0
   - ID de proveedor debe existir (Debe ser creado previamente)
4. Asignar ID automáticamente (autoincremental)
5. Redimensionar array si está lleno (duplicar capacidad)
6. Fecha de registro automática (fecha actual del sistema)

**Flujo:**
```
1. Preguntar: "¿Desea registrar un nuevo producto? (S/N)"
2. Si N o "CANCELAR" → volver al menú
3. Solicitar datos uno por uno (permitir cancelar en cada paso)
4. Validar ID de proveedor existe
5. Mostrar resumen de datos ingresados
6. Confirmar: "¿Guardar producto? (S/N)"
7. Si S → guardar y redimensionar si es necesario
```

##### 2.2.2 Buscar Producto
```cpp
void buscarProducto(Tienda* tienda);
```

**Requisitos:**
Presentar menú de opciones de búsqueda:
```
1. Buscar por ID (exacto)
2. Buscar por nombre (coincidencia parcial)
3. Buscar por código (coincidencia parcial)
4. Listar por proveedor (mostrar todos los productos de un proveedor)
0. Cancelar
```

**Implementación de coincidencia parcial:**
- Ignorar mayúsculas/minúsculas
- Buscar substring en el campo correspondiente
- Mostrar TODOS los resultados que coincidan

##### 2.2.3 Actualizar Producto
```cpp
void actualizarProducto(Tienda* tienda);
```

**Requisitos:**
1. Buscar producto por ID
2. Si no existe → mensaje de error y volver
3. Mostrar datos actuales
4. Presentar menú de campos editables:
```
¿Qué desea editar?
1. Código
2. Nombre
3. Descripción
4. Proveedor
5. Precio
6. Stock
7. Guardar cambios
0. Cancelar sin guardar
```

5. Permitir editar múltiples campos antes de guardar
6. Validar cada campo según reglas de creación
7. Confirmar antes de guardar cambios

##### 2.2.4 Actualizar Stock Manualmente
```cpp
void actualizarStockProducto(Tienda* tienda);
```

**Requisitos:**
- Buscar producto por ID
- Mostrar stock actual
- Permitir ajuste manual (+/-)
- Validar que stock final >= 0
- Confirmar cambio

##### 2.2.5 Listar Productos
```cpp
void listarProductos(Tienda* tienda);
```

**Requisitos:**
- Mostrar todos los productos en formato tabla
- Incluir: ID, Código, Nombre, Proveedor, Precio, Stock
- Mostrar nombre del proveedor (no solo ID)

##### 2.2.6 Eliminar Producto
```cpp
void eliminarProducto(Tienda* tienda);
```

**Requisitos:**
- Buscar producto por ID
- Mostrar datos del producto a eliminar
- Advertir si tiene transacciones asociadas
- Confirmar eliminación
- NO eliminar físicamente, solo marcar (o implementar eliminación real moviendo elementos)

---

#### 2.3 Funciones CRUD - PROVEEDORES

##### 2.3.1 Crear Proveedor
```cpp
void crearProveedor(Tienda* tienda);
```

**Requisitos similares a crearProducto:**
- Permitir cancelación
- Validar RIF único
- Validar email (formato básico: contiene @)
- Asignar ID autoincremental
- Redimensionar array si necesario

##### 2.3.2 Buscar Proveedor
```cpp
void buscarProveedor(Tienda* tienda);
```

**Opciones:**
```
1. Buscar por ID
2. Buscar por nombre (coincidencia parcial)
3. Buscar por RIF
0. Cancelar
```

##### 2.3.3 Actualizar Proveedor
```cpp
void actualizarProveedor(Tienda* tienda);
```

**Campos editables:**
- Nombre
- RIF
- Teléfono
- Email
- Dirección

##### 2.3.4 Listar Proveedores
```cpp
void listarProveedores(Tienda* tienda);
```

##### 2.3.5 Eliminar Proveedor
```cpp
void eliminarProveedor(Tienda* tienda);
```

**Requisitos:**
- Verificar si tiene productos asociados
- Si tiene productos → no permitir eliminar (o preguntar si desea reasignar)

---

#### 2.4 Funciones CRUD - CLIENTES

##### 2.4.1 Crear Cliente
```cpp
void crearCliente(Tienda* tienda);
```

**Validaciones:**
- Cédula/RIF único
- Email válido
- Permitir cancelación

##### 2.4.2 Buscar Cliente
```cpp
void buscarCliente(Tienda* tienda);
```

**Opciones:**
```
1. Buscar por ID
2. Buscar por nombre (coincidencia parcial)
3. Buscar por cédula/RIF
0. Cancelar
```

##### 2.4.3 Actualizar Cliente
```cpp
void actualizarCliente(Tienda* tienda);
```

##### 2.4.4 Listar Clientes
```cpp
void listarClientes(Tienda* tienda);
```

##### 2.4.5 Eliminar Cliente
```cpp
void eliminarCliente(Tienda* tienda);
```

---

#### 2.5 Funciones de TRANSACCIONES

##### 2.5.1 Registrar Compra (a Proveedor)
```cpp
void registrarCompra(Tienda* tienda);
```

**Requisitos:**
1. Solicitar ID de producto
2. Verificar que el producto exista
3. Solicitar ID de proveedor
4. Verificar que el proveedor exista
5. Solicitar cantidad
6. Solicitar precio unitario de compra
7. Calcular total automáticamente
8. Mostrar resumen
9. Confirmar transacción
10. **INCREMENTAR** el stock del producto automáticamente
11. Guardar transacción con tipo = "COMPRA"
12. Redimensionar array de transacciones si es necesario

##### 2.5.2 Registrar Venta (a Cliente)
```cpp
void registrarVenta(Tienda* tienda);
```

**Requisitos:**
1. Solicitar ID de producto
2. Verificar que el producto exista
3. Mostrar stock disponible
4. Solicitar ID de cliente
5. Verificar que el cliente exista
6. Solicitar cantidad
7. **Validar que haya stock suficiente**
8. Usar precio del producto (o permitir precio de venta diferente)
9. Calcular total automáticamente
10. Mostrar resumen
11. Confirmar transacción
12. **DECREMENTAR** el stock del producto automáticamente
13. Guardar transacción con tipo = "VENTA"
14. Redimensionar array si es necesario

##### 2.5.3 Buscar Transacciones
```cpp
void buscarTransacciones(Tienda* tienda);
```

**Opciones:**
```
1. Buscar por ID de transacción
2. Buscar por ID de producto
3. Buscar por ID de cliente
4. Buscar por ID de proveedor
5. Buscar por fecha exacta (YYYY-MM-DD)
6. Buscar por tipo (COMPRA/VENTA)
0. Cancelar
```

##### 2.5.4 Listar Transacciones
```cpp
void listarTransacciones(Tienda* tienda);
```

**Formato:**
Mostrar en tabla: ID, Tipo, Producto, Cliente/Proveedor, Cantidad, Total, Fecha

##### 2.5.5 Cancelar/Anular Transacción
```cpp
void cancelarTransaccion(Tienda* tienda);
```

**Requisitos:**
1. Buscar transacción por ID
2. Mostrar detalles completos
3. Advertir sobre reversión de stock:
   - Si es COMPRA → restar del stock
   - Si es VENTA → sumar al stock
4. Confirmar cancelación
5. Ajustar stock automáticamente
6. Marcar transacción como cancelada (o eliminarla)

---

#### 2.6 Funciones Auxiliares Obligatorias

##### 2.6.1 Redimensionamiento de Arrays
```cpp
void redimensionarProductos(Tienda* tienda);
void redimensionarProveedores(Tienda* tienda);
void redimensionarClientes(Tienda* tienda);
void redimensionarTransacciones(Tienda* tienda);
```

**Algoritmo obligatorio:**
```cpp
1. Crear nuevo array con capacidad * 2
2. Copiar todos los elementos del array viejo al nuevo
3. Liberar memoria del array viejo
4. Actualizar puntero y capacidad
```

##### 2.6.2 Validaciones
```cpp
bool validarEmail(const char* email);
bool validarFecha(const char* fecha);
bool existeProducto(Tienda* tienda, int id);
bool existeProveedor(Tienda* tienda, int id);
bool existeCliente(Tienda* tienda, int id);
bool codigoDuplicado(Tienda* tienda, const char* codigo);
bool rifDuplicado(Tienda* tienda, const char* rif);
```

##### 2.6.3 Búsquedas
```cpp
int buscarProductoPorId(Tienda* tienda, int id);
int buscarProveedorPorId(Tienda* tienda, int id);
int buscarClientePorId(Tienda* tienda, int id);

// Búsquedas por coincidencia parcial (retornan array de índices)
int* buscarProductosPorNombre(Tienda* tienda, const char* nombre, int* numResultados);
```

##### 2.6.4 Utilidades
```cpp
void obtenerFechaActual(char* buffer);  // Formato YYYY-MM-DD
void convertirAMinusculas(char* cadena);
bool contieneSubstring(const char* texto, const char* busqueda);
```

---

### 3. ESTRUCTURA DE MENÚS

#### Menú Principal
```
╔═══════════════════════════════════════════╗
║   SISTEMA DE GESTIÓN DE INVENTARIO       ║
║   Tienda: [Nombre de la Tienda]          ║
╚═══════════════════════════════════════════╝

1. Gestión de Productos
2. Gestión de Proveedores
3. Gestión de Clientes
4. Gestión de Transacciones
5. Salir

Seleccione una opción: 
```

#### Submenú: Gestión de Productos
```
╔═══════════════════════════════════════════╗
║        GESTIÓN DE PRODUCTOS              ║
╚═══════════════════════════════════════════╝

1. Registrar nuevo producto
2. Buscar producto
3. Actualizar producto
4. Actualizar stock manualmente
5. Listar todos los productos
6. Eliminar producto
0. Volver al menú principal

Seleccione una opción:
```

#### Submenú: Gestión de Proveedores
```
╔═══════════════════════════════════════════╗
║       GESTIÓN DE PROVEEDORES             ║
╚═══════════════════════════════════════════╝

1. Registrar proveedor
2. Buscar proveedor
3. Actualizar proveedor
4. Listar proveedores
5. Eliminar proveedor
0. Volver al menú principal

Seleccione una opción:
```

#### Submenú: Gestión de Clientes
```
╔═══════════════════════════════════════════╗
║         GESTIÓN DE CLIENTES              ║
╚═══════════════════════════════════════════╝

1. Registrar cliente
2. Buscar cliente
3. Actualizar cliente
4. Listar clientes
5. Eliminar cliente
0. Volver al menú principal

Seleccione una opción:
```

#### Submenú: Gestión de Transacciones
```
╔═══════════════════════════════════════════╗
║       GESTIÓN DE TRANSACCIONES           ║
╚═══════════════════════════════════════════╝

1. Registrar compra (a proveedor)
2. Registrar venta (a cliente)
3. Buscar transacciones
4. Listar todas las transacciones
5. Cancelar/Anular transacción
0. Volver al menú principal

Seleccione una opción:
```

---

### 4. CASOS DE USO OBLIGATORIOS

El programa debe ser capaz de ejecutar los siguientes escenarios completos:

#### Caso de Uso 1: Registro Completo de Proveedor y Producto
```
1. Crear proveedor "Distribuidora XYZ"
2. Crear producto "Laptop HP" asociado al proveedor
3. Buscar el producto por nombre
4. Listar todos los productos
```

#### Caso de Uso 2: Compra de Inventario
```
1. Registrar compra de 50 unidades de "Laptop HP"
2. Verificar que el stock se incrementó correctamente
3. Buscar transacciones por producto
```

#### Caso de Uso 3: Venta con Validación de Stock
```
1. Intentar vender 100 unidades (debe rechazar por stock insuficiente)
2. Vender 30 unidades a un cliente
3. Verificar que el stock se redujo correctamente
4. Listar transacciones por cliente
```

#### Caso de Uso 4: Cancelación de Transacción
```
1. Cancelar la venta de 30 unidades
2. Verificar que el stock volvió a su valor anterior
3. Verificar que la transacción está marcada como cancelada
```

#### Caso de Uso 5: Edición Selectiva
```
1. Actualizar producto: cambiar solo precio y descripción
2. Actualizar proveedor: cambiar solo teléfono
3. Verificar cambios en listados
```

#### Caso de Uso 6: Búsquedas Avanzadas
```
1. Buscar productos por coincidencia parcial de nombre ("lap" debe encontrar "Laptop")
2. Listar todos los productos de un proveedor específico
3. Buscar transacciones por fecha exacta
```

#### Caso de Uso 7: Cancelación Durante Registro
```
1. Iniciar registro de producto
2. Ingresar algunos datos
3. Cancelar antes de confirmar
4. Verificar que no se creó el producto
```

---

### 5. VALIDACIONES Y MANEJO DE ERRORES

#### 5.1 Validaciones de Entrada

| Campo | Validación |
|-------|-----------|
| Email | Debe contener '@' y al menos un '.' después del @ |
| Fecha | Formato YYYY-MM-DD, valores válidos |
| Precio | Debe ser > 0 |
| Stock | Debe ser >= 0 |
| Cantidad (transacción) | Debe ser > 0 |
| RIF/Código | No duplicados |
| IDs relacionados | Deben existir en sus respectivas entidades |

#### 5.2 Mensajes de Error Obligatorios
```cpp
// Ejemplos de mensajes claros
"ERROR: El proveedor con ID 5 no existe."
"ERROR: Stock insuficiente. Disponible: 20, Solicitado: 30"
"ERROR: El código 'PROD-001' ya está registrado."
"ERROR: Formato de email inválido."
"ADVERTENCIA: Este proveedor tiene 5 productos asociados."
```

#### 5.3 Confirmaciones Obligatorias

Solicitar confirmación (S/N) antes de:
- Guardar nuevo registro
- Actualizar registro existente
- Eliminar registro
- Cancelar transacción
- Salir del programa (si hay datos no guardados)

---

### 6. GESTIÓN DE MEMORIA

#### 6.1 Reglas Obligatorias

1. **Inicialización:**
   - Todos los arrays dinámicos deben inicializarse con capacidad 5
   - Usar `new` para asignación dinámica

2. **Liberación:**
   - Implementar función `liberarTienda()` que libere TODA la memoria
   - Llamar al liberar antes de salir del programa
   - Usar `delete[]` para arrays dinámicos

3. **Redimensionamiento:**
   - Cuando un array esté lleno (num == capacidad), duplicar su capacidad
   - Implementar función específica para cada tipo de array
   - Copiar datos al nuevo array antes de liberar el viejo

4. **Sin fugas de memoria:**
   - Valgrind debe reportar 0 bytes perdidos
   - Cada `new` debe tener su correspondiente `delete`

#### 6.2 Ejemplo de Redimensionamiento
```cpp
void redimensionarProductos(Tienda* tienda) {
    int nuevaCapacidad = tienda->capacidadProductos * 2;
    Producto* nuevoArray = new Producto[nuevaCapacidad];
    
    // Copiar datos
    for (int i = 0; i < tienda->numProductos; i++) {
        nuevoArray[i] = tienda->productos[i];
    }
    
    // Liberar viejo y actualizar
    delete[] tienda->productos;
    tienda->productos = nuevoArray;
    tienda->capacidadProductos = nuevaCapacidad;
}
```

---

### 7. FORMATO DE SALIDA

#### 7.1 Listado de Productos
```
╔══════════════════════════════════════════════════════════════════════════╗
║                         LISTADO DE PRODUCTOS                             ║
╠════╦═══════════╦══════════════════╦══════════════╦═══════╦════════╦══════╣
║ ID ║  Código   ║     Nombre       ║  Proveedor   ║ Precio║ Stock  ║ Fecha║
╠════╬═══════════╬══════════════════╬══════════════╬═══════╬════════╬══════╣
║  1 ║ PROD-001  ║ Laptop HP        ║ Distrib. XYZ ║ 450.00║   50   ║ 2025 ║
║  2 ║ PROD-002  ║ Mouse Logitech   ║ TechStore    ║  15.00║  200   ║ 2025 ║
╚════╩═══════════╩══════════════════╩══════════════╩═══════╩════════╩══════╝

Total de productos: 2
```

#### 7.2 Detalle de Transacción
```
╔═══════════════════════════════════════════════════════════╗
║              DETALLE DE TRANSACCIÓN                       ║
╠═══════════════════════════════════════════════════════════╣
║ ID Transacción: 1                                         ║
║ Tipo: VENTA                                               ║
║ Producto: Laptop HP (ID: 1)                               ║
║ Cliente: Juan Pérez (ID: 3)                               ║
║ Cantidad: 5 unidades                                      ║
║ Precio Unitario: $450.00                                  ║
║ Total: $2,250.00                                          ║
║ Fecha: 2025-01-20                                         ║
║ Descripción: Venta corporativa                            ║
╚═══════════════════════════════════════════════════════════╝
```

---

### 8. DOCUMENTACIÓN OBLIGATORIA

#### 8.1 README del Proyecto

Incluir archivo `README.md` con:
- Descripción del proyecto
- Instrucciones de compilación
- Instrucciones de ejecución
- Estructura del código
- Funcionalidades implementadas
- Casos de prueba ejecutados
---

### 9. ENTREGA
  - Código fuente (`.cpp`, `.h`)
  - README.md
  - Manual de usuario (PDF)
  - Capturas de pantalla de ejecución

---
### 10. PREGUNTAS FRECUENTES

**P: ¿Puedo usar vectores de STL en lugar de arrays dinámicos?**  
R: No. El objetivo es practicar gestión manual de memoria con punteros.

**P: ¿Qué pasa si el usuario ingresa texto donde se espera un número?**  
R: Debes validar y manejar este error, solicitando nuevamente el dato correcto.

**P: ¿Puedo usar funciones de C++ como `std::string`?**  
R: Puedes usar `std::string` para lectura temporal, pero las estructuras deben usar `char[]` como se especifica  (en proyectos posteriores lo agradeceras)

**P: ¿La cancelación debe funcionar en todos los pasos del registro?**  
R: Sí, el usuario debe poder cancelar el proceso en cualquier momento durante la creación de registros. Tienen libertad creativa de como implementarlo.
---


## NOTAS FINALES
Este proyecto evalúa tu capacidad para:
1. Gestionar memoria dinámica de forma segura y eficiente
2. Implementar estructuras de datos complejas con punteros
3. Desarrollar algoritmos de búsqueda y validación
4. Crear interfaces de usuario claras y funcionales
5. Documentar código de manera simple
**¡Éxito en tu proyecto!**

---

**Ing. Victor Kneider**  
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
    int stock;                 // Cantidad en inventario
    char fechaRegistro[11];    // Formato: YYYY-MM-DD
};

struct Proveedor {
    int id;                    // Identificador único (autoincremental)
    char nombre[100];          // Nombre del proveedor
    char rif[20];              // RIF o identificación fiscal
    char telefono[20];         // Teléfono de contacto
    char email[100];           // Correo electrónico
    char direccion[200];       // Dirección física
    char fechaRegistro[11];    // Formato: YYYY-MM-DD
};

struct Cliente {
    int id;                    // Identificador único (autoincremental)
    char nombre[100];          // Nombre completo del cliente
    char cedula[20];           // Cédula o RIF
    char telefono[20];         // Teléfono de contacto
    char email[100];           // Correo electrónico
    char direccion[200];       // Dirección física
    char fechaRegistro[11];    // Formato: YYYY-MM-DD
};

struct Transaccion {
    int id;                    // Identificador único (autoincremental)
    char tipo[10];             // "COMPRA" o "VENTA"
    int idProducto;            // ID del producto involucrado
    int idRelacionado;         // ID del proveedor (compra) o cliente (venta)
    int cantidad;              // Cantidad de unidades
    float precioUnitario;      // Precio por unidad en esta transacción
    float total;               // cantidad * precioUnitario
    char fecha[11];            // Formato: YYYY-MM-DD
    char descripcion[200];     // Notas adicionales (opcional)
};

struct Tienda {
    char nombre[100];          // Nombre de la tienda
    char rif[20];              // RIF de la tienda
    
    // Arrays dinámicos de entidades
    Producto* productos;
    int numProductos;
    int capacidadProductos;
    
    Proveedor* proveedores;
    int numProveedores;
    int capacidadProveedores;
    
    Cliente* clientes;
    int numClientes;
    int capacidadClientes;
    
    Transaccion* transacciones;
    int numTransacciones;
    int capacidadTransacciones;
    
    // Contadores para IDs autoincrementales
    int siguienteIdProducto;
    int siguienteIdProveedor;
    int siguienteIdCliente;
    int siguienteIdTransaccion;
};
// Crea la tienda con arreglos dinamicos

bool existeProveedor(Tienda* tienda, int id){
    for (int i = 0; i < tienda->numProveedores; i++) {
        if (tienda->proveedores[i].id == id) {
            return true;
        }
    }
    return false;
}
bool existeCliente(Tienda* tienda, int id){
    for (int i = 0; i < tienda->numClientes; i++) {
        if (tienda->clientes[i].id == id) {
            return true;
        }
    }
    return false;
}
void obtenerFechaActual(char* buffer){
    SYSTEMTIME st;
    GetLocalTime(&st);
    sprintf(buffer, "%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay);
}
bool codigoDuplicado(Tienda* tienda, const char* codigo){
    for (int i = 0; i < tienda->numProductos; i++) {
        if (strcmp(tienda->productos[i].codigo, codigo) == 0) {
            return true;
        }
    }
    return false;
}
void redimensionarProductos(Tienda* tienda){
    int nuevaCapacidad = tienda->capacidadProductos * 2;
    Producto* nuevoArray = new Producto[nuevaCapacidad];
    
    // Copiar datos
    for (int i = 0; i < tienda->numProductos; i++) {
        nuevoArray[i] = tienda->productos[i];
    }
    
    // Liberar viejo y actualizar
    delete[] tienda->productos;
    tienda->productos = nuevoArray;
    tienda->capacidadProductos = nuevaCapacidad;
}

void inicializarTienda(Tienda* tienda, const char* nombre, const char* rif){
    strcpy(tienda->nombre, nombre);
    strcpy(tienda->rif, rif);
    
    tienda->numProductos = 0;
    tienda->capacidadProductos = 5;
    tienda->productos = new Producto[tienda->capacidadProductos];
    
    tienda->numProveedores = 0;
    tienda->capacidadProveedores = 5;
    tienda->proveedores = new Proveedor[tienda->capacidadProveedores];
    
    tienda->numClientes = 0;
    tienda->capacidadClientes = 5;
    tienda->clientes = new Cliente[tienda->capacidadClientes];
    
    tienda->numTransacciones = 0;
    tienda->capacidadTransacciones = 5;
    tienda->transacciones = new Transaccion[tienda->capacidadTransacciones];
    
    tienda->siguienteIdProducto = 1;
    tienda->siguienteIdProveedor = 1;
    tienda->siguienteIdCliente = 1;
    tienda->siguienteIdTransaccion = 1;
}
void liberarTienda(Tienda* tienda){
    delete[] tienda->productos;
    delete[] tienda->proveedores;
    delete[] tienda->clientes;
    delete[] tienda->transacciones;
    delete tienda;

}
// Funciones CRUD - PRODUCTOS
void crearProducto(Tienda* tienda){
    char codigo[20];
    char nombre[100];
    char descripcion[200];
    int idProveedor;
    float precio;
    int stock;
    char buffer[11];
    char respuesta;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    do{
    cout << "Ingrese el código del producto: ";
    cin.getline(codigo, 20);
    } while(codigoDuplicado(tienda, codigo));
    cout << "Ingrese el nombre del producto: ";
    cin.getline(nombre, 100);
    do{
    cout << "Ingrese el ID del proveedor: ";
    cin >> idProveedor;
    } while(!existeProveedor(tienda, idProveedor));
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    do{
    cout << "Ingrese el precio unitario: ";
    cin >> precio;
    } while(precio < 0);
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    do{
    cout << "Ingrese el stock inicial: ";
    cin >> stock;
    } while(stock < 0);
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Ingrese la descripción del producto: ";
    cin.getline(descripcion, 200);
    cout << "Resumen del producto a registrar:" << endl;
    cout << "Código: " << codigo << endl;
    cout << "Nombre: " << nombre << endl;
    cout << "ID Proveedor: " << idProveedor << endl;
    cout << "Precio Unitario: " << precio << endl;
    cout << "Stock: " << stock << endl;
    obtenerFechaActual(buffer);
    cout <<"Fecha de registro: "<<buffer<<endl;
    cout << "¿Guardar producto? (S/N): ";
    cin >> respuesta;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (respuesta == 'S' || respuesta == 's') {
        if (tienda->numProductos >= tienda->capacidadProductos) {
            redimensionarProductos(tienda);
        }
        tienda->productos[tienda->numProductos].id = tienda->siguienteIdProducto++;
        strcpy(tienda->productos[tienda->numProductos].codigo, codigo);
        strcpy(tienda->productos[tienda->numProductos].nombre, nombre);
        strcpy(tienda->productos[tienda->numProductos].descripcion, descripcion);
        tienda->productos[tienda->numProductos].idProveedor = idProveedor;
        tienda->productos[tienda->numProductos].precio = precio;
        tienda->productos[tienda->numProductos].stock = stock;
        tienda->numProductos++;
    }
}
void buscarProducto(Tienda* tienda){}
void actualizarProducto(Tienda* tienda){}
void actualizarStockProducto(Tienda* tienda){}
void listarProductos(Tienda* tienda){}
void eliminarProducto(Tienda* tienda){}
// Funciones CRUD - PROVEEDORES
void crearProveedor(Tienda* tienda){}
void buscarProveedor(Tienda* tienda){}
void actualizarProveedor(Tienda* tienda){}
void listarProveedores(Tienda* tienda){}
void eliminarProveedor(Tienda* tienda){}
// Funciones CRUD - CLIENTES
void crearCliente(Tienda* tienda){}
void buscarCliente(Tienda* tienda){}
void actualizarCliente(Tienda* tienda){}
void listarClientes(Tienda* tienda){}
void eliminarCliente(Tienda* tienda){}
// Funciones de TRANSACCIONES
void registrarCompra(Tienda* tienda){}
void registrarVenta(Tienda* tienda){}
void buscarTransacciones(Tienda* tienda){}
void listarTransacciones(Tienda* tienda){}
void cancelarTransaccion(Tienda* tienda){}
// Funciones Auxiliares

void redimensionarProveedores(Tienda* tienda){
    int nuevaCapacidad = tienda->capacidadProveedores * 2;
    Proveedor* nuevoespacio = new Proveedor[nuevaCapacidad];
    for (int i = 0; i < tienda->numProveedores; i++) {
        nuevoespacio[i] = tienda->proveedores[i];
    }
    delete[] tienda->proveedores;
    tienda->proveedores = nuevoespacio;
    tienda->capacidadProveedores = nuevaCapacidad;
}
void redimensionarClientes(Tienda* tienda){
    int nuevaCapacidad = tienda->capacidadClientes * 2;
    Cliente* nuevoespacio = new Cliente[nuevaCapacidad];
    for (int i = 0; i < tienda->numClientes; i++) {
        nuevoespacio[i] = tienda->clientes[i];
    }
    delete[] tienda->clientes;
    tienda->clientes = nuevoespacio;
    tienda->capacidadClientes = nuevaCapacidad;
}
void redimensionarTransacciones(Tienda* tienda){
    int nuevaCapacidad = tienda->capacidadTransacciones * 2;
    Transaccion* nuevoespacio = new Transaccion[nuevaCapacidad];
    for (int i = 0; i < tienda->numTransacciones; i++) {
        nuevoespacio[i] = tienda->transacciones[i];
    }
    delete[] tienda->transacciones;
    tienda->transacciones = nuevoespacio;
    tienda->capacidadTransacciones = nuevaCapacidad;
}
bool validarEmail(const char* email){
    
}
bool validarFecha(const char* fecha){
    
}
bool existeProducto(Tienda* tienda, int id){
    for (int i = 0; i < tienda->numProductos; i++) {
        if (tienda->productos[i].id == id) {
            return true;
        }
    }
    return false;
}

bool rifDuplicado(Tienda* tienda, const char* rif){
    for (int i = 0; i < tienda->numProveedores; i++) {
        if (strcmp(tienda->proveedores[i].rif, rif) == 0) {
            return true;
        }
    }
    for (int i = 0; i < tienda->numClientes; i++) {
        if (strcmp(tienda->clientes[i].cedula, rif) == 0) {
            return true;
        }
    }
    return false;
}
int buscarProductoPorId(Tienda* tienda, int id){

}
int buscarProveedorPorId(Tienda* tienda, int id){}
int buscarClientePorId(Tienda* tienda, int id){

}
int* buscarProductosPorNombre(Tienda* tienda, const char* nombre, int* numResultados){

}
void convertirAMinusculas(char* cadena){

}
bool contieneSubstring(const char* texto, const char* busqueda){

}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    bool flag = true;
    char nombreTienda[100];
    char rifTienda[20];
    cout << "Ingrese el nombre de la tienda: ";
    cin.getline(nombreTienda, 100);
    cout << "Ingrese el RIF de la tienda: ";
    cin.getline(rifTienda, 20);
    Tienda* tienda = new Tienda;
    inicializarTienda(tienda, nombreTienda, rifTienda);
    int anchoInterno = 43; 
    int prefijoLargo = 11;
    int espacioRestante = anchoInterno - prefijoLargo - (int)strlen(nombreTienda) + 1;
    do{
        cout << "╔═══════════════════════════════════════════╗" << endl;
        cout << "║      SISTEMA DE GESTIÓN DE INVENTARIO     ║" << endl;
        cout << "║   Tienda: " << nombreTienda << setw(espacioRestante) << "║" << endl;
        cout << "╚═══════════════════════════════════════════╝" << endl;
        cout << "1. Gestión de Productos" << endl;
        cout << "2. Gestión de Proveedores" << endl;
        cout << "3. Gestión de Clientes" << endl;
        cout << "4. Gestión de Transacciones" << endl;
        cout << "5. Salir" << endl;
        cout << "Seleccione una opción: ";
        int opcion;
        cin >> opcion;
        switch (opcion) {
            case 1:
                cout << "Gestión de Productos seleccionada." << endl;
                break;
            case 2:
                cout << "Gestión de Proveedores seleccionada." << endl;
                break;
            case 3:
                cout << "Gestión de Clientes seleccionada." << endl;
                break;
            case 4:
                cout << "Gestión de Transacciones seleccionada." << endl;
                break;
            case 5:
                cout << "Saliendo del programa." << endl;
                liberarTienda(tienda);
                flag = false;
                return 0;
            default:
                cout << "Opción inválida. Intente de nuevo." << endl;
        } 
    } while (flag);
}







