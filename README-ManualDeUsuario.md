# Manual de Usuario - Proyecto de Gestión de Inventario

Bienvenido al repositorio de **Programación 2**. Este repositorio contiene la evolución de un Sistema de Gestión de Inventario desarrollado en C++, dividido en tres fases principales.

---

## Estructura del Repositorio

El proyecto está dividido en tres etapas principales, reflejando el aprendizaje progresivo en el manejo del lenguaje C++:

### 1-Punteros (Fase 1)
En esta primera fase, el sistema se construyó estructurado en un solo archivo principal (`Proyecto_fase_1.cpp`). Utiliza arreglos dinámicos y punteros para la gestión en memoria de Productos, Proveedores, Clientes y Transacciones utilizando `structs` primitivos. 
- **Cómo usar**: Simplemente compila el archivo `Proyecto_fase_1.cpp` con cualquier compilador de C++ (por ejemplo, `g++ Proyecto_fase_1.cpp -o proyecto1`) y ejecuta el binario resultante. Todas las operaciones se realizan en la memoria RAM y se pierden al cerrar el programa.

### 2-Ficheros.bin (Fase 2)
Esta fase introduce la **persistencia de datos** en la aplicación a través de archivos binarios. La estructura del código es similar a la Fase 1 (`Proyecto_Fase_2.cpp`), pero cuenta con rutinas para guardar en disco y cargar al inicio del programa toda la información usando las librerías `fstream`, garantizando que la información del sistema (productos, clientes, proveedores, compras, ventas) perdure de una sesión a otra.
- **Cómo usar**: Compila el archivo `Proyecto_Fase_2.cpp` y ejecútalo. El programa creará automáticamente archivos `.bin` o `.dat` en el mismo directorio donde sea ejecutado para almacenar y recuperar la información cada vez que inicies sesión.

### 3-Modularización_y_POO (Fase 3)
La etapa final y más estructurada del sistema. Todo el código fue dividido en múltiples archivos utilizando **Programación Orientada a Objetos (POO)**. Existe una separación limpia entre clases y conceptos: *Tienda, Productos, Proveedores, Clientes, Transacciones, Persistencia, Interfaz, y Utilidades*. Además cuenta con un archivo `Makefile` para la compilación.
- **Cómo usar**: 
  1. Abre tu terminal y dirígete a la carpeta `3-Modularizacion_y_POO/Proyecto_3`.
  2. Ejecuta el comando `make` (si usas MinGW o herramientas similares en Windows) para compilar todo el proyecto de forma automática utilizando el Makefile provisto.
  3. Ejecuta el binario generado (por defecto `inventario_app.exe` o análogo en el Makefile).
  4. Los datos se seguirán guardando localmente (en una subcarpeta de `datos/` como se haya configurado el gestor de archivos).

---

## Funcionalidades Globales del Sistema
En cualquiera de las tres fases, las entidades principales que podrás manejar a través de los menús en la consola son:

### Menú principal
Al ejecutar el sistema principal, se muestra:

1. Gestión de Productos
2. Gestión de Proveedores
3. Gestión de Clientes
4. Gestión de Transacciones
5. Verificar Integridad Referencial
6. Reportes Analíticos
7. Crear Backup Manual
8. Salir (incluye backup automático)

### Gestión de Productos
Opciones disponibles:
1. Registrar nuevo producto
2. Buscar producto (por ID o nombre)
3. Actualizar producto
4. Listar todos los productos activos
5. Eliminar producto (inactivar)
6. Restaurar producto eliminado

*Flujo básico de registro:*
1. Ingresar nombre, código, descripción.
2. Ingresar precio, stock inicial y stock mínimo.
3. Guardar para que se asigne ID automáticamente.

### Gestión de Proveedores
Opciones disponibles:
1. Registrar proveedor
2. Buscar proveedor (por ID o nombre)
3. Actualizar proveedor
4. Listar proveedores activos
5. Eliminar proveedor (inactivar)
6. Restaurar proveedor eliminado

*Datos clave:*
- Nombre
- RIF
- Teléfono

### Gestión de Clientes
Opciones disponibles:
1. Registrar cliente
2. Buscar cliente (por ID o nombre)
3. Actualizar cliente
4. Listar clientes activos
5. Eliminar cliente (inactivar)
6. Restaurar cliente eliminado

*Datos clave:*
- Nombre
- Cédula/RIF

### Gestión de Transacciones
Opciones disponibles:
1. Registrar compra (a proveedor)
2. Registrar venta (a cliente)
3. Buscar transacciones
4. Listar transacciones activas
5. Cancelar/Anular transacción
6. Restaurar transacción anulada

*Flujo de compra/venta:*
1. Elegir tipo de transacción.
2. Indicar ID relacionado (proveedor o cliente).
3. Agregar uno o varios detalles (producto, cantidad, precio unitario).
4. Confirmar para calcular y guardar total.

> **Nota**: El desarrollo fue hecho en un entorno Windows y algunas fases requieren las librerías `#include <windows.h>` por configuraciones de la terminal, por lo cual se recomienda compilar bajo ese mismo S.O.
