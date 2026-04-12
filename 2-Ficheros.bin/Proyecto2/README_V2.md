# 🛠️ Sistema de Gestión de Inventario (Fase 2) - Documentación Técnica

Este sistema permite la gestión persistente de una tienda mediante archivos binarios, aplicando acceso aleatorio para optimizar la memoria y asegurar que los datos no se pierdan al cerrar el programa.

---

## 📊 1. Arquitectura de Datos (Estructuras y Tamaños)

El sistema utiliza estructuras fijas para garantizar que cada registro ocupe el mismo espacio en el archivo binario, permitiendo el uso de `seekg()` y `seekp()`.

| Estructura | Componentes Principales | Tamaño Estimado (`sizeof`) |
| :--- | :--- | :--- |
| **Header** | `int` cantidadRegistros, `int` ultimoID | **8 Bytes** |
| **Tienda** | `char[50]` nombre, `double` ingresos/egresos | **66 Bytes** |
| **Producto** | `int` id, `char[50]` nombre, `int` existencia, `double` precio | **66 - 72 Bytes** |
| **Cliente** | `int` id, `char[50]` nombre, `char[20]` tlf, `bool` activo | **80 Bytes** |
| **Proveedor** | `int` id, `char[50]` nombre, `char[50]` empresa, `bool` activo | **108 Bytes** |
| **Detalle** | `int` idProducto, `int` cantidad, `double` precioUnitario | **16 Bytes** |
| **Transaccion**| `int` id, `char` tipo, `Detalle[10]` items, `bool` activo | **~180 - 200 Bytes** |

> **Nota:** Los tamaños exactos pueden variar levemente según el alineamiento del compilador, pero la lógica de acceso aleatorio se adapta automáticamente mediante el operador `sizeof()`.

---

## 📖 2. Manual de Usuario Sencillo

### 🚀 Inicio Rápido
1.  **Ejecución:** Al abrir el programa, verás el nombre de tu tienda y el menú principal.
2.  **Configuración Inicial:** Si es la primera vez, el sistema creará automáticamente los archivos `.bin`.

### 📦 Gestión de Inventario
* **Productos:** Puedes agregar productos nuevos, editar sus precios o eliminarlos (borrado lógico).
* **Clientes/Proveedores:** Registra a quién le compras y a quién le vendes. Es obligatorio tener al menos uno de cada uno para poder facturar.

### 💰 Compras y Ventas
1.  Selecciona **Gestión de Transacciones**.
2.  **Registrar Compra:** Aumenta el stock de tus productos y resta dinero de la caja (egresos).
3.  **Registrar Venta:** Disminuye el stock y suma dinero a la caja (ingresos).
4.  **Validación de Stock:** El sistema no te permitirá vender más de lo que tienes físicamente en existencia.

### ⚠️ Anulaciones y Restauraciones
* **Anular Factura:** Si te equivocas en una venta, puedes anularla. El sistema devolverá los productos al inventario y restará el dinero del balance automáticamente.
* **Restaurar:** Si anulaste algo por error, puedes reactivarlo (siempre que tengas stock suficiente).

### 📋 Reportes y Seguridad
* **Reportes:** Genera un resumen de ventas, productos bajo stock y balance general.
* **Backup:** Crea una copia de seguridad de tus archivos en la carpeta `backups/` para evitar pérdida de datos.

---

## 🛠️ Requerimientos del Desarrollador
* **Compilador:** GCC / MinGW (C++11 o superior).
* **Librerías:** `<fstream>`, `<iostream>`, `<iomanip>`.
* **Sistema:** Windows (usa `windows.h` para funciones de consola).

---
**Desarrollado por:** m17-coder  
**Institución:** Universidad Rafael Urdaneta (URU)