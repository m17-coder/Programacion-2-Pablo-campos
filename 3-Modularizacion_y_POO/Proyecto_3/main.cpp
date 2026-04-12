#include <iostream>
#include <iomanip>
#include <limits>
#include "Productos/OPERACIONESPRODUCTOS.HPP"
#include "persistencia/GestorArchivos.hpp"

using namespace std;

// --- FUNCIONES DE INTERFAZ (UI/UX) separadas de la lógica de negocio ---

void submenuRegistrarProducto() {
    Producto p;
    char buffer[200];
    
    cout << "\n==================================================" << endl;
    cout << "             NUEVO PRODUCTO (REGISTRO)             " << endl;
    cout << "==================================================" << endl;

    cout << "Nombre del Producto: ";
    cin.getline(buffer, 100);
    p.setNombre(buffer);
    
    cout << "Codigo Interno (Ej: P-001): ";
    cin.getline(buffer, 20);
    p.setCodigo(buffer);

    cout << "Breve Descripcion: ";
    cin.getline(buffer, 200);
    p.setDescripcion(buffer);

    int idProv;
    cout << "ID del Proveedor suministrador: ";
    cin >> idProv;
    p.setIdProveedor(idProv);

    float precio;
    cout << "Precio de Venta ($): ";
    cin >> precio;
    p.setPrecio(precio);

    int stockMinIMO, stockInicIAL;
    cout << "Stock Minimo: ";
    cin >> stockMinIMO;
    p.setStockminimo(stockMinIMO);

    cout << "Stock Inicial: ";
    cin >> stockInicIAL;
    p.setStockInicial(stockInicIAL);

    if(cin.fail()) { cin.clear(); }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    // LLAMAMOS A LA LÓGICA DE NEGOCIO PASANDO EL OBJETO EN BLANCO
    if (registrarProducto(p)) {
        cout << "[OK] Producto " << p.getNombre() << " registrado exitosamente con el ID: " << p.getId() << endl;
    } else {
        cout << "[ERROR] Fallo al intentar guardar el producto." << endl;
    }
}

void submenuListarProductos() {
    cout << "\n================================================================================" << endl;
    cout << "                         LISTA DE PRODUCTOS REGISTRADOS                           " << endl;
    cout << "================================================================================" << endl;
    cout << left << setw(6) << "ID" << setw(25) << "NOMBRE" << setw(12) << "PRECIO" << setw(10) << "STOCK" << setw(10) << "PROV" << endl;
    cout << "--------------------------------------------------------------------------------" << endl;

    Producto productosActivos[100];
    int cantidad = obtenerProductosActivos(productosActivos, 100);

    if (cantidad == 0) {
        cout << "                        No hay productos activos para mostrar.                   " << endl;
    } else {
        for (int i = 0; i < cantidad; i++) {
            cout << left 
                 << setw(6) << productosActivos[i].getId() 
                 << setw(25) << productosActivos[i].getNombre()
                 << "$ " << setw(10) << fixed << setprecision(2) << productosActivos[i].getPrecio() 
                 << setw(10) << productosActivos[i].getExistencia() 
                 << setw(10) << productosActivos[i].getIdProveedor() 
                 << endl;
        }
    }
    cout << "================================================================================" << endl;
}


void submenuBuscarProducto() {
    int opcion;
    cout << "\n--- BUSCAR PRODUCTOS ---" << endl;
    cout << "1. Buscar por ID" << endl;
    cout << "2. Buscar por Nombre" << endl;
    cout << "Seleccione: ";
    cin >> opcion;

    if (cin.fail()) { cin.clear(); cin.ignore(1000, '\n'); return; }

    if (opcion == 1) {
        int id;
        cout << "Ingrese el ID: ";
        cin >> id;
        Producto p;
        if (obtenerProductoPorId(id, p)) {
            cout << "\n[!] PRODUCTO ENCONTRADO:" << endl;
            cout << "ID: " << p.getId() << " - Nombre: " << p.getNombre() << " - Precio: $" << p.getPrecio() << " - Activo: " << (p.getActivo() ? "SI" : "NO") << endl;
        } else {
            cout << "[!] Producto no encontrado." << endl;
        }
    } else if (opcion == 2) {
        cin.ignore(1000, '\n'); // Limpiar el Enter
        char nombre[100];
        cout << "Ingrese nombre (o parte de el): ";
        cin.getline(nombre, 100);
        
        Producto lista[100];
        int cant = buscarProductosPorNombre(nombre, lista, 100);
        if (cant > 0) {
            cout << "\nSe encontraron " << cant << " coincidencias:" << endl;
            for (int i = 0; i < cant; i++) {
                cout << "-> ID: " << lista[i].getId() << " - " << lista[i].getNombre() << endl;
            }
        } else {
            cout << "[!] No se encontraron coincidencias." << endl;
        }
    }
}

void submenuActualizarProducto() {
    int id;
    cout << "\n--- ACTUALIZAR PRODUCTO ---" << endl;
    cout << "Ingrese ID del producto a modificar: ";
    cin >> id;

    Producto p;
    if (!obtenerProductoPorId(id, p)) {
        cout << "[!] Producto no encontrado o no se pudo leer." << endl;
        return;
    }
    
    if (!p.getActivo()) {
        cout << "[!] El producto esta inactivo (en papelera). No se puede editar sin restaurar." << endl;
        return;
    }

    cin.ignore(1000, '\n');
    char buffer[200];
    
    cout << "Nuevo Nombre (actual: " << p.getNombre() << "): ";
    cin.getline(buffer, 100);
    if(buffer[0] != '\0') p.setNombre(buffer);

    cout << "Nuevo Codigo (actual: " << p.getCodigo() << "): ";
    cin.getline(buffer, 20);
    if(buffer[0] != '\0') p.setCodigo(buffer);

    cout << "Nueva Descripcion (actual: " << p.getDescripcion() << "): ";
    cin.getline(buffer, 200);
    if(buffer[0] != '\0') p.setDescripcion(buffer);

    float precio;
    cout << "Nuevo Precio ($) (actual: " << p.getPrecio() << "): ";
    cin >> precio;
    p.setPrecio(precio);

    int stockMin;
    cout << "Nuevo Stock Minimo (actual: " << p.getStockminimo() << "): ";
    cin >> stockMin;
    p.setStockminimo(stockMin);

    if (modificarProducto(id, p)) {
        cout << "[OK] Producto modificado con exito." << endl;
    } else {
        cout << "[ERROR] Fallo al intentar modificar producto." << endl;
    }
}

void submenuEliminarProducto() {
    int id;
    cout << "\n--- ELIMINAR (INACTIVAR) PRODUCTO ---" << endl;
    cout << "Ingrese ID del producto a enviar a papelera: ";
    cin >> id;

    if (inactivarProducto(id)) {
        cout << "[OK] Producto inactivado correctamente." << endl;
    } else {
        cout << "[ERROR] Fallo al inactivar. Puede que ya estuviese inactivo o no exista el ID." << endl;
    }
}

void submenuRestaurarProducto() {
    cout << "\n--- RESTAURAR PRODUCTOS ---" << endl;
    Producto inactivos[100];
    int cantidad = obtenerProductosInactivos(inactivos, 100);
    
    if (cantidad == 0) {
        cout << "[i] No hay productos en la papelera." << endl;
        return;
    }
    
    cout << "Productos inactivos:" << endl;
    for (int i = 0; i < cantidad; i++) {
        cout << "ID: " << inactivos[i].getId() << " - Nombre: " << inactivos[i].getNombre() << endl;
    }

    int id;
    cout << "Ingrese ID a restaurar: ";
    cin >> id;
    
    if (activarProducto(id)) {
        cout << "[OK] Producto restaurado satisfactoriamente." << endl;
    } else {
        cout << "[ERROR] Imposible restaurar el ID especificado." << endl;
    }
}

// -----------------------------------------------------------------------

int main() {
    GestorArchivos::inicializarSistemaArchivos();

    cout << "==================================================" << endl;
    cout << "   TEST FASE 3: LÓGICA DE NEGOCIO Y UI SEPARADAS  " << endl;
    cout << "==================================================" << endl;

    int opcion;
    do {
        cout << "\nMenu Principal (Test)" << endl;
        cout << "1. Registrar nuevo producto" << endl;
        cout << "2. Listar productos" << endl;
        cout << "3. Buscar producto" << endl;
        cout << "4. Actualizar producto" << endl;
        cout << "5. Eliminar (Inactivar) producto" << endl;
        cout << "6. Restaurar producto" << endl;
        cout << "0. Salir" << endl;
        cout << "Seleccione: ";
        cin >> opcion;
        cin.ignore(1000, '\n');
                if (cin.fail()) {
                cin.clear(); // Limpiar el estado de error
                cin.ignore(1000, '\n');
                opcion = -1; // Forzar que siga en el loop
                continue;
            }

        switch(opcion) {
            case 1:
                cin.ignore(1000, '\n');
                submenuRegistrarProducto();
                break;
            case 2:
                cin.ignore(1000, '\n');
                submenuListarProductos();
                break;
            case 3:
                submenuBuscarProducto();
                break;
            case 4:
                submenuActualizarProducto();
                break;
            case 5:
                submenuEliminarProducto();
                break;
            case 6:
                submenuRestaurarProducto();
                break;
            case 0:
                cout << "Saliendo..." << endl;
                break;
            default:
                cout << "Opcion invalida." << endl;
                break;
        }
    } while (opcion != 0);

    return 0;
}