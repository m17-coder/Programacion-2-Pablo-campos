#include <iostream>
#include <iomanip>
#include <limits>

#include "persistencia/GestorArchivos.hpp"
#include "Tienda/OPERACIONESTIENDA.HPP"
#include "Productos/OPERACIONESPRODUCTOS.HPP"
#include "Proveedores/OPERACIONESPROVEEDORES.HPP"
#include "Clientes/OPERACIONESCLIENTES.HPP"
#include "Transacciones/OPERACIONESTRANSACCIONES.HPP"

using namespace std;

// DECLARACIONES
void menuProductos();
void menuProveedores();
void menuClientes();
void menuTransacciones();
void verTienda();

// --- FUNCIONES UTILS ---
void pausar() {
    cout << "\nPresione ENTER para continuar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// --- MENU PRODUCTOS ---
void menuProductos() {
    int opcion;
    do {
        cout << "\n--- GESTION DE PRODUCTOS ---" << endl;
        cout << "1. Registrar Nuevo" << endl;
        cout << "2. Listar Activos" << endl;
        cout << "3. Buscar (ID o Nombre)" << endl;
        cout << "4. Actualizar" << endl;
        cout << "5. Inactivar (Eliminar)" << endl;
        cout << "6. Restaurar" << endl;
        cout << "0. Volver" << endl;
        cout << "Opcion: ";
        cin >> opcion;
        cin.ignore(1000, '\n');

        if (opcion == 1) {
            Producto p; char buf[100]; float f; int i;
            cout << "Nombre: "; cin.getline(buf, 100); p.setNombre(buf);
            cout << "Codigo: "; cin.getline(buf, 100); p.setCodigo(buf);
            cout << "Desc: "; cin.getline(buf, 100); p.setDescripcion(buf);
            cout << "Precio: "; cin >> f; p.setPrecio(f);
            cout << "Stock Inicial: "; cin >> i; p.setStockInicial(i); p.setExistencia(i);
            cout << "Stock Minimo: "; cin >> i; p.setStockminimo(i);
            registrarProducto(p);
            cout << "[Exito] ID: " << p.getId() << endl;
        } else if (opcion == 2) {
            Producto lista[100];
            int c = obtenerProductosActivos(lista, 100);
            for(int i=0; i<c; i++) cout << lista[i].getId() << " | " << lista[i].getNombre() << " | $" << lista[i].getPrecio() << " | Stock: " << lista[i].getExistencia() << endl;
        } else if (opcion == 3) {
            int id; cout << "ID a buscar: "; cin >> id;
            Producto p;
            if(obtenerProductoPorId(id, p)) cout << "ENCONTRADO: " << p.getNombre() << " | Precio: $" << p.getPrecio() << endl;
            else cout << "No encontrado." << endl;
        } else if (opcion == 4) {
            int id; cout << "ID a actualizar: "; cin >> id; cin.ignore(1000, '\n');
            Producto p;
            if(obtenerProductoPorId(id, p)) {
                char buf[100];
                cout << "Nuevo Nombre (actual: " << p.getNombre() << "): "; cin.getline(buf, 100);
                if(buf[0]!='\0') p.setNombre(buf);
                modificarProducto(id, p);
                cout << "Actualizado." << endl;
            }
        } else if (opcion == 5) {
            int id; cout << "ID a inactivar: "; cin >> id; 
            if(inactivarProducto(id)) cout << "Inactivado." << endl;
            else cout << "Error." << endl;
        } else if (opcion == 6) {
            int id; cout << "ID a restaurar: "; cin >> id; 
            if(activarProducto(id)) cout << "Restaurado." << endl;
        }
    } while(opcion != 0);
}

// --- MENU PROVEEDORES ---
void menuProveedores() {
    int opcion;
    do {
        cout << "\n--- GESTION DE PROVEEDORES ---" << endl;
        cout << "1. Registrar Nuevo" << endl;
        cout << "2. Listar" << endl;
        cout << "3. Buscar" << endl;
        cout << "4. Actualizar" << endl;
        cout << "5. Inactivar" << endl;
        cout << "0. Volver" << endl;
        cout << "Opcion: ";
        cin >> opcion;
        cin.ignore(1000, '\n');

        if (opcion == 1) {
            Proveedor p; char buf[100];
            cout << "Nombre: "; cin.getline(buf, 100); p.setNombre(buf);
            cout << "RIF: "; cin.getline(buf, 100); p.setRif(buf);
            cout << "Telefono: "; cin.getline(buf, 100); p.setTelefono(buf);
            registrarProveedor(p);
            cout << "[Exito] ID: " << p.getId() << endl;
        } else if (opcion == 2) {
            Proveedor lista[100];
            int c = obtenerProveedoresActivos(lista, 100);
            for(int i=0; i<c; i++) cout << lista[i].getId() << " | " << lista[i].getNombre() << " | " << lista[i].getRif() << endl;
        } else if (opcion == 5) {
            int id; cout << "ID a inactivar: "; cin >> id; 
            if(inactivarProveedor(id)) cout << "Inactivado." << endl;
        }
    } while(opcion != 0);
}

// --- MENU CLIENTES ---
void menuClientes() {
    int opcion;
    do {
        cout << "\n--- GESTION DE CLIENTES ---" << endl;
        cout << "1. Registrar Nuevo" << endl;
        cout << "2. Listar" << endl;
        cout << "3. Inactivar" << endl;
        cout << "0. Volver" << endl;
        cout << "Opcion: ";
        cin >> opcion;
        cin.ignore(1000, '\n');

        if (opcion == 1) {
            Cliente p; char buf[100];
            cout << "Nombre: "; cin.getline(buf, 100); p.setNombre(buf);
            cout << "Cedula/RIF: "; cin.getline(buf, 100); p.setCedula(buf);
            registrarCliente(p);
            cout << "[Exito] ID: " << p.getId() << endl;
        } else if (opcion == 2) {
            Cliente lista[100];
            int c = obtenerClientesActivos(lista, 100);
            for(int i=0; i<c; i++) cout << lista[i].getId() << " | " << lista[i].getNombre() << " | " << lista[i].getCedula() << endl;
        } else if (opcion == 3) {
            int id; cout << "ID a inactivar: "; cin >> id; 
            if(inactivarCliente(id)) cout << "Inactivado." << endl;
        }
    } while(opcion != 0);
}

// --- MENU TRANSACCIONES Y TIENDA ---
void verTienda() {
    Tienda t;
    if(obtenerDatosTienda(t)) {
        cout << "\n--- DATOS DE LA TIENDA ---" << endl;
        cout << "Nombre: " << t.getNombre() << " (RIF: " << t.getRif() << ")" << endl;
        cout << "CAJA ACTUAL: $" << t.getCaja() << endl;
        cout << "Ingresos: $" << t.getIngresosTotales() << " | Egresos: $" << t.getEgresosTotales() << endl;
        cout << "Ventas emitidas: " << t.getContadorVentas() << " | Compras emitidas: " << t.getContadorCompras() << endl;
    }
}

void menuTransacciones() {
    int opcion;
    do {
        cout << "\n--- TRANSACCIONES ---" << endl;
        cout << "1. Emitir Venta a Cliente" << endl;
        cout << "2. Emitir Compra a Proveedor" << endl;
        cout << "3. Listar Transacciones Activas" << endl;
        cout << "4. Ver Arqueo de Tienda" << endl;
        cout << "0. Volver" << endl;
        cout << "Opcion: ";
        cin >> opcion;
        cin.ignore(1000, '\n');

        if (opcion == 1 || opcion == 2) {
            Transaccion t;
            char tipo = (opcion == 1) ? 'V' : 'C';
            t.setTipo(tipo);

            int idRel;
            if(tipo == 'V') cout << "ID del Cliente: ";
            else cout << "ID del Proveedor: ";
            cin >> idRel; t.setIdRelacionado(idRel);

            char resp;
            do {
                DetalleVenta dv;
                int idP; float precio; int cant;
                cout << "ID del Producto: "; cin >> idP;
                cout << "Cantidad: "; cin >> cant;
                cout << "Precio Unitario: "; cin >> precio;
                
                dv.setIdProducto(idP); dv.setCantidad(cant); dv.setPrecioUnitario(precio);
                t.agregarDetalle(dv);

                cout << "Desea agregar otro producto? (s/n): ";
                cin >> resp;
            } while (resp == 's' || resp == 'S');

            t.calcularTotal();
            if(registrarTransaccion(t)) {
                if(tipo == 'V') procesarRegistroVentaTienda(t.getTotal());
                else procesarRegistroCompraTienda(t.getTotal());
                cout << "Transaccion guardada. Monto total: $" << t.getTotal() << endl;
            }
        } else if (opcion == 3) {
            Transaccion lista[100];
            int c = obtenerTransaccionesActivas(lista, 100);
            for(int i=0; i<c; i++) {
                cout << "Trx #" << lista[i].getId() 
                     << " | TIPO: " << lista[i].getTipo() 
                     << " | Entidad: " << lista[i].getIdRelacionado() 
                     << " | Total: $" << lista[i].getTotal() << endl;
            }
        } else if (opcion == 4) {
            verTienda();
        }
    } while(opcion != 0);
}

int main() {
    // Inicializar persistencia
    GestorArchivos::inicializarSistemaArchivos();
    inicializarTiendaPorDefecto();

    int opcion;
    do {
        cout << "\n=============================================" << endl;
        cout << "        SISTEMA DE INVENTARIO Y VENTAS       " << endl;
        cout << "            (VERSION OOP & CAPAS)            " << endl;
        cout << "=============================================" << endl;
        cout << "1. Modulo de Productos" << endl;
        cout << "2. Modulo de Proveedores" << endl;
        cout << "3. Modulo de Clientes" << endl;
        cout << "4. Modulo de Transacciones y Caja" << endl;
        cout << "0. Salir" << endl;
        cout << "Opcion: ";
        cin >> opcion;

        if (cin.fail()) {
            cin.clear(); cin.ignore(1000, '\n'); break;
        }

        switch(opcion) {
            case 1: menuProductos(); break;
            case 2: menuProveedores(); break;
            case 3: menuClientes(); break;
            case 4: menuTransacciones(); break;
            case 0: cout << "Saliendo del sistema..." << endl; break;
            default: cout << "Opcion no valida." << endl;
        }
    } while(opcion != 0);

    return 0;
}