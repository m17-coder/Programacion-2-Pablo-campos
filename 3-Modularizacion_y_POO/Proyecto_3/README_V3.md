# README_V3 - Proyecto 3 (POO y Modularizacion)

## 1. Descripcion General
Sistema de gestion de inventario desarrollado en C++ con Programacion Orientada a Objetos (POO), modularizado por dominios:
- Tienda
- Productos
- Proveedores
- Clientes
- Transacciones
- Persistencia
- Interfaz
- Utilidades

El sistema trabaja con archivos binarios para mantener la informacion entre ejecuciones.

## 2. Requisitos
- Sistema operativo Windows (probado con consola de Windows).
- Compilador g++ con soporte C++11 o superior.
- `make` disponible en terminal (por ejemplo, MinGW/MSYS2).

## 3. Compilacion
Ubicarse en la carpeta del proyecto:

```powershell
cd C:\Users\Pc\git-Pablo\Programacion-2-Pablo-campos\3-Modularizacion_y_POO\Proyecto_3
```

Compilar con Makefile:

```powershell
mingw32-make
```

Esto genera el ejecutable:
- `sistema.exe`

### 3.1 Limpieza de compilacion
Para eliminar ejecutable y objetos:

```powershell
make clean
```

## 4. Ejecucion
Con el ejecutable compilado:

```powershell
./sistema.exe
```

Al iniciar, el sistema:
1. Inicializa los archivos binarios en `datos/` si no existen.
2. Inicializa los datos base de la tienda.
3. Muestra el menu principal.

## 5. Estructura del Proyecto

```text
Proyecto_3/
├── main_final.cpp
├── Makefile
├── Proyecto_Fase_3.cpp
├── datos/
│   └── backups/
├── interfaz/
│   ├── Interfaz.cpp
│   └── Interfaz.hpp
├── persistencia/
│   ├── Constantes.hpp
│   ├── GestorArchivos.cpp
│   └── GestorArchivos.hpp
├── Productos/
│   ├── PRODUCTOS.CPP
│   ├── PRODUCTOS.HPP
│   ├── OPERACIONESPRODCUTOS.CPP
│   └── OPERACIONESPRODUCTOS.HPP
├── Proveedores/
│   ├── PROVEEDORES.CPP
│   ├── PROVEEDORES.HPP
│   ├── OPERACIONESPROVEEDORES.CPP
│   └── OPERACIONESPROVEEDORES.HPP
├── Clientes/
│   ├── CLIENTES.CPP
│   ├── CLIENTES.HPP
│   ├── OPERACIONESCLIENTES.CPP
│   └── OPERACIONESCLIENTES.HPP
├── Tienda/
│   ├── TIENDA.CPP
│   ├── TIENDA.HPP
│   ├── OPERACIONESTIENDA.CPP
│   └── OPERACIONESTIENDA.HPP
├── Transacciones/
│   ├── TRANSACCIONES.CPP
│   ├── TRANSACCIONES.HPP
│   ├── OPERACIONESTRANSACCIONES.CPP
│   └── OPERACIONESTRANSACCIONES.HPP
├── utilidades/
│   ├── FORMATOS.CPP
│   ├── FORMATOS.HPP
│   ├── VALIDACIONES.CPP
│   └── VALIDACIONES.HPP
└── test de compilacion/
    ├── main_fase4.cpp
    ├── migrador.cpp
    ├── test_size.cpp
    └── test_size2.cpp
```

## 6. Manual de Usuario

## 6.1 Menu principal
Al ejecutar `sistema.exe`, se muestra:

1. Gestion de Productos
2. Gestion de Proveedores
3. Gestion de Clientes
4. Gestion de Transacciones
5. Verificar Integridad Referencial
6. Reportes Analiticos
7. Crear Backup Manual
8. Salir (incluye backup automatico)

## 6.2 Gestion de Productos
Opciones disponibles:
1. Registrar nuevo producto
2. Buscar producto (por ID o nombre)
3. Actualizar producto
4. Listar todos los productos activos
5. Eliminar producto (inactivar)
6. Restaurar producto eliminado

Flujo basico de registro:
1. Ingresar nombre, codigo, descripcion.
2. Ingresar precio, stock inicial y stock minimo.
3. Guardar para que se asigne ID automaticamente.

## 6.3 Gestion de Proveedores
Opciones disponibles:
1. Registrar proveedor
2. Buscar proveedor (por ID o nombre)
3. Actualizar proveedor
4. Listar proveedores activos
5. Eliminar proveedor (inactivar)
6. Restaurar proveedor eliminado

Datos clave:
- Nombre
- RIF
- Telefono

## 6.4 Gestion de Clientes
Opciones disponibles:
1. Registrar cliente
2. Buscar cliente (por ID o nombre)
3. Actualizar cliente
4. Listar clientes activos
5. Eliminar cliente (inactivar)
6. Restaurar cliente eliminado

Datos clave:
- Nombre
- Cedula/RIF

## 6.5 Gestion de Transacciones
Opciones disponibles:
1. Registrar compra (a proveedor)
2. Registrar venta (a cliente)
3. Buscar transacciones
4. Listar transacciones activas
5. Cancelar/Anular transaccion
6. Restaurar transaccion anulada

Flujo de compra/venta:
1. Elegir tipo de transaccion.
2. Indicar ID relacionado (proveedor o cliente).
3. Agregar uno o varios detalles (producto, cantidad, precio unitario).
4. Confirmar para calcular y guardar total.

## 6.6 Backups
El sistema permite:
- Backup manual desde opcion 7 del menu principal.
- Backup automatico al salir por opcion 8.

Los respaldos se guardan en:
- `datos/backups/`

## 7. Archivos Binarios Utilizados
- `datos/tienda.bin`
- `datos/productos.bin`
- `datos/proveedores.bin`
- `datos/clientes.bin`
- `datos/transacciones.bin`

Estos archivos son creados automaticamente cuando no existen.

## 8. Notas Tecnicas
- El proyecto usa una arquitectura modular con separacion `.hpp` / `.cpp`.
- La persistencia esta centralizada en `GestorArchivos`.
- Se utilizan operaciones de alta, busqueda, modificacion, inactivacion y restauracion para las entidades principales.
- El Makefile incluye archivos `.cpp` y `.CPP` para mantener compatibilidad con la nomenclatura actual del proyecto.

## 9. Solucion de Problemas Rapida
1. Error `make: command not found`:
   - Instalar MinGW/MSYS2 o usar un entorno con `make`.

2. Error de compilacion por `g++` no encontrado:
   - Verificar que g++ este instalado y agregado al PATH.

3. No aparecen datos guardados:
   - Confirmar que la carpeta `datos/` exista y que haya permisos de escritura.
   - Ejecutar desde la carpeta `Proyecto_3` para conservar rutas relativas correctas.
