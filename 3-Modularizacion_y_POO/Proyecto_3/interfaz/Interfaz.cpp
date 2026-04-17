#include "Interfaz.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <cctype>
#include <windows.h>
#include "../utilidades/VALIDACIONES.HPP"
#include "../utilidades/FORMATOS.HPP"
#include "../Tienda/OPERACIONESTIENDA.HPP"
#include "../Productos/OPERACIONESPRODUCTOS.HPP"
#include "../Proveedores/OPERACIONESPROVEEDORES.HPP"
#include "../Clientes/OPERACIONESCLIENTES.HPP"
#include "../Transacciones/OPERACIONESTRANSACCIONES.HPP"
#include "../persistencia/GestorArchivos.hpp"
#include "../persistencia/GestorArchivos.hpp"

using namespace std;

void menuProductos();
void menuProveedores();
void menuClientes();
void menuTransacciones();
void verTienda();

static void limpiarPantallaConPausa() {
    Formatos::pausarYlimpiarpantalla();
}

static void configurarConsolaUTF8() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

static bool esEnteroPositivo(const string& texto) {
    if (texto.empty()) {
        return false;
    }

    for (char caracter : texto) {
        if (!isdigit(static_cast<unsigned char>(caracter))) {
            return false;
        }
    }

    return true;
}

static void imprimirLineaProducto(const Producto& producto) {
    cout << left << setw(6) << producto.getId()
         << setw(28) << producto.getNombre()
         << setw(16) << Formatos::formatearMoneda(producto.getPrecio())
         << setw(10) << producto.getExistencia()
         << setw(10) << producto.getIdProveedor()
         << endl;
}

static void imprimirLineaProveedor(const Proveedor& proveedor) {
    cout << left << setw(6) << proveedor.getId()
         << setw(30) << proveedor.getNombre()
         << setw(18) << proveedor.getRif()
         << setw(16) << proveedor.getTelefono()
         << endl;
}

static void imprimirLineaCliente(const Cliente& cliente) {
    cout << left << setw(6) << cliente.getId()
         << setw(30) << cliente.getNombre()
         << setw(18) << cliente.getCedula()
         << endl;
}

static void imprimirLineaTransaccion(const Transaccion& transaccion) {
    cout << left << setw(6) << transaccion.getId()
         << setw(10) << transaccion.getTipo()
         << setw(12) << transaccion.getIdRelacionado()
         << setw(18) << Formatos::formatearMoneda(transaccion.getTotal())
         << endl;
}

static void imprimirEncabezadoProductos(const char* titulo) {
    cout << "\n================================================================================" << endl;
    cout << titulo << endl;
    cout << "================================================================================" << endl;
    cout << left << setw(6) << "ID"
         << setw(28) << "NOMBRE"
         << setw(16) << "PRECIO"
         << setw(10) << "STOCK"
         << setw(10) << "PROV" << endl;
    cout << "--------------------------------------------------------------------------------" << endl;
}

static void imprimirEncabezadoProveedores(const char* titulo) {
    cout << "\n============================================================" << endl;
    cout << titulo << endl;
    cout << "============================================================" << endl;
    cout << left << setw(6) << "ID"
         << setw(30) << "NOMBRE"
         << setw(18) << "RIF"
         << setw(16) << "TELEFONO" << endl;
    cout << "------------------------------------------------------------" << endl;
}

static void imprimirEncabezadoClientes(const char* titulo) {
    cout << "\n============================================================" << endl;
    cout << titulo << endl;
    cout << "============================================================" << endl;
    cout << left << setw(6) << "ID"
         << setw(30) << "NOMBRE"
         << setw(18) << "CEDULA/RIF" << endl;
    cout << "------------------------------------------------------------" << endl;
}

static void imprimirEncabezadoTransacciones(const char* titulo) {
    cout << "\n============================================================" << endl;
    cout << titulo << endl;
    cout << "============================================================" << endl;
    cout << left << setw(6) << "ID"
         << setw(10) << "TIPO"
         << setw(12) << "RELACION"
         << setw(18) << "TOTAL" << endl;
    cout << "------------------------------------------------------------" << endl;
}

static void mostrarMenuPrincipal(const Tienda& miTienda) {
    // Evita limpiar la pantalla aquí para que el menú siempre quede visible en terminales que repintan lento.
    cout << "╔═══════════════════════════════════════════╗" << endl;
    cout << "║      SISTEMA DE GESTIÓN DE INVENTARIO     ║" << endl;
    cout << "║ Tienda: " << left << setw(33) << miTienda.getNombre() << " ║" << endl;
    cout << "║ RIF:    " << left << setw(33) << miTienda.getRif() << " ║" << endl;
    cout << "╚═══════════════════════════════════════════╝" << endl;
    cout << "1. Gestión de Productos" << endl;
    cout << "2. Gestión de Proveedores" << endl;
    cout << "3. Gestión de Clientes" << endl;
    cout << "4. Gestión de Transacciones" << endl;
    cout << "5. Verificar Integridad Referencial" << endl;
    cout << "6. Reportes Analíticos" << endl;
    cout << "7. Crear Backup Manual" << endl;
    cout << "8. Salir" << endl;
}

int submenu_producto() {
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
    opcion = Validaciones::validarEntradaEntera("Seleccione una opción: ", 0, 6);
    return opcion;
}

int submenu_proveedor() {
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
    opcion = Validaciones::validarEntradaEntera("Seleccione una opción: ", 0, 6);
    return opcion;
}

int submenu_cliente() {
    int opcion;
    cout << "╔═══════════════════════════════════════════╗" << endl;
    cout << "║         GESTIÓN DE CLIENTES               ║" << endl;
    cout << "╚═══════════════════════════════════════════╝" << endl;
    cout << "1. Registrar cliente" << endl;
    cout << "2. Buscar cliente" << endl;
    cout << "3. Actualizar cliente" << endl;
    cout << "4. Listar clientes" << endl;
    cout << "5. Eliminar cliente" << endl;
    cout << "6. Restaurar cliente eliminado" << endl;
    cout << "0. Volver al menú principal" << endl;
    opcion = Validaciones::validarEntradaEntera("Seleccione una opción: ", 0, 6);
    return opcion;
}

int submenu_transaccion() {
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
    opcion = Validaciones::validarEntradaEntera("Seleccione una opción: ", 0, 6);
    return opcion;
}

static void imprimirProductosInactivos() {
    Producto lista[100];
    int cantidad = obtenerProductosInactivos(lista, 100);

    if (cantidad == 0) {
        cout << "[!] No hay productos eliminados." << endl;
        return;
    }

    imprimirEncabezadoProductos("PRODUCTOS ELIMINADOS");
    for (int i = 0; i < cantidad; i++) {
        imprimirLineaProducto(lista[i]);
    }
}

static void imprimirProveedoresInactivos() {
    Proveedor lista[100];
    int cantidad = obtenerProveedoresInactivos(lista, 100);

    if (cantidad == 0) {
        cout << "[!] No hay proveedores eliminados." << endl;
        return;
    }

    imprimirEncabezadoProveedores("PROVEEDORES ELIMINADOS");
    for (int i = 0; i < cantidad; i++) {
        imprimirLineaProveedor(lista[i]);
    }
}

static void imprimirClientesInactivos() {
    Cliente lista[100];
    int cantidad = obtenerClientesInactivos(lista, 100);

    if (cantidad == 0) {
        cout << "[!] No hay clientes eliminados." << endl;
        return;
    }

    imprimirEncabezadoClientes("CLIENTES ELIMINADOS");
    for (int i = 0; i < cantidad; i++) {
        imprimirLineaCliente(lista[i]);
    }
}

static void imprimirTransaccionesInactivas() {
    Transaccion lista[100];
    int cantidad = obtenerTransaccionesInactivas(lista, 100);

    if (cantidad == 0) {
        cout << "[!] No hay transacciones anuladas." << endl;
        return;
    }

    imprimirEncabezadoTransacciones("TRANSACCIONES ANULADAS");
    for (int i = 0; i < cantidad; i++) {
        imprimirLineaTransaccion(lista[i]);
    }
}

static void mostrarTransaccion(const Transaccion& transaccion) {
    cout << "Trx #" << transaccion.getId()
         << " | TIPO: " << transaccion.getTipo()
         << " | Entidad: " << transaccion.getIdRelacionado()
         << " | Total: " << Formatos::formatearMoneda(transaccion.getTotal()) << endl;
}

void menuProductos() {
    int opcion;
    do {
        opcion = submenu_producto();

        switch (opcion) {
            case 1: {
                Producto p;
                char buf[100];
                float f;
                int i;

                cout << "Nombre: "; cin.getline(buf, 100); Formatos::convertirAMayusculas(buf); p.setNombre(buf);
                cout << "Codigo: "; cin.getline(buf, 100); Formatos::convertirAMayusculas(buf); p.setCodigo(buf);
                cout << "Desc: "; cin.getline(buf, 100); p.setDescripcion(buf);
                f = Validaciones::validarEntradaFlotante("Precio: ", 0, 1000000); p.setPrecio(f);
                i = Validaciones::validarEntradaEntera("Stock Inicial: ", 0, 100000); p.setStockInicial(i); p.setExistencia(i);
                i = Validaciones::validarEntradaEntera("Stock Minimo: ", 0, 100000); p.setStockminimo(i);
                registrarProducto(p);
                cout << "[Exito] ID: " << p.getId() << endl;
                break;
            }
            case 2: {
                char consulta[100];
                Producto lista[100];
                cout << "ID o nombre a buscar: ";
                cin.getline(consulta, 100);

                if (esEnteroPositivo(consulta)) {
                    int id = atoi(consulta);
                    Producto p;
                    if (obtenerProductoPorId(id, p)) {
                        imprimirEncabezadoProductos("RESULTADO DE PRODUCTO");
                        imprimirLineaProducto(p);
                    } else {
                        cout << "No encontrado." << endl;
                    }
                } else {
                    int cantidad = buscarProductosPorNombre(consulta, lista, 100);
                    if (cantidad == 0) {
                        cout << "No encontrado." << endl;
                    } else {
                        imprimirEncabezadoProductos("RESULTADOS DE PRODUCTOS");
                        for (int i = 0; i < cantidad; i++) {
                            imprimirLineaProducto(lista[i]);
                        }
                    }
                }
                break;
            }
            case 3: {
                int id = Validaciones::validarEntradaEntera("ID a actualizar: ", 1, 10000);
                Producto p;
                if (obtenerProductoPorId(id, p)) {
                    char buf[100];
                    cout << "Nuevo Nombre (actual: " << p.getNombre() << "): "; cin.getline(buf, 100);
                    if (buf[0] != '\0') {
                        Formatos::convertirAMayusculas(buf);
                        p.setNombre(buf);
                    }
                    modificarProducto(id, p);
                    cout << "Actualizado." << endl;
                }
                break;
            }
            case 4: {
                Producto lista[100];
                int c = obtenerProductosActivos(lista, 100);
                imprimirEncabezadoProductos("CATALOGO DE PRODUCTOS ACTIVOS");
                for (int i = 0; i < c; i++) imprimirLineaProducto(lista[i]);
                break;
            }
            case 5: {
                int id = Validaciones::validarEntradaEntera("ID a eliminar (inactivar): ", 1, 10000);
                if (inactivarProducto(id)) cout << "Inactivado." << endl;
                else cout << "Error." << endl;
                break;
            }
            case 6: {
                imprimirProductosInactivos();
                int id = Validaciones::validarEntradaEntera("ID a restaurar: ", 1, 10000);
                if (activarProducto(id)) cout << "Restaurado." << endl;
                else cout << "Error." << endl;
                break;
            }
            default:
                break;
        }

        if (opcion != 0) limpiarPantallaConPausa();
    } while (opcion != 0);
}

void menuProveedores() {
    int opcion;
    do {
        opcion = submenu_proveedor();

        switch (opcion) {
            case 1: {
                Proveedor p;
                char buf[100];

                cout << "Nombre: "; cin.getline(buf, 100); Formatos::convertirAMayusculas(buf); p.setNombre(buf);

                do {
                    cout << "RIF (Ej. J-12345678): "; cin.getline(buf, 100);
                    Formatos::convertirAMayusculas(buf);
                    if (Validaciones::validarRIF(buf)) break;
                    cout << "Error: RIF invalido. Ingrese nuevamente.\n";
                } while (true);
                p.setRif(buf);

                do {
                    cout << "Telefono: "; cin.getline(buf, 100);
                    if (Validaciones::validarTelefono(buf)) break;
                    cout << "Error: Telefono invalido. Ingrese nuevamente.\n";
                } while (true);
                p.setTelefono(buf);

                registrarProveedor(p);
                cout << "[Exito] ID: " << p.getId() << endl;
                break;
            }
            case 2: {
                char consulta[100];
                Proveedor lista[100];
                cout << "ID o nombre a buscar: ";
                cin.getline(consulta, 100);

                if (esEnteroPositivo(consulta)) {
                    int id = atoi(consulta);
                    Proveedor prov;
                    if (obtenerProveedorPorId(id, prov)) {
                        imprimirEncabezadoProveedores("RESULTADO DE PROVEEDOR");
                        imprimirLineaProveedor(prov);
                    } else {
                        cout << "No encontrado." << endl;
                    }
                } else {
                    int cantidad = buscarProveedoresPorNombre(consulta, lista, 100);
                    if (cantidad == 0) {
                        cout << "No encontrado." << endl;
                    } else {
                        imprimirEncabezadoProveedores("RESULTADOS DE PROVEEDORES");
                        for (int i = 0; i < cantidad; i++) imprimirLineaProveedor(lista[i]);
                    }
                }
                break;
            }
            case 3: {
                int id = Validaciones::validarEntradaEntera("ID a actualizar: ", 1, 10000);
                Proveedor prov;
                if (obtenerProveedorPorId(id, prov)) {
                    char buf[100];
                    cout << "Nuevo Nombre (actual: " << prov.getNombre() << "): "; cin.getline(buf, 100);
                    if (buf[0] != '\0') {
                        Formatos::convertirAMayusculas(buf);
                        prov.setNombre(buf);
                    }
                    cout << "Nuevo RIF (actual: " << prov.getRif() << "): "; cin.getline(buf, 100);
                    if (buf[0] != '\0') {
                        Formatos::convertirAMayusculas(buf);
                        prov.setRif(buf);
                    }
                    cout << "Nuevo Telefono (actual: " << prov.getTelefono() << "): "; cin.getline(buf, 100);
                    if (buf[0] != '\0') {
                        prov.setTelefono(buf);
                    }
                    if (modificarProveedor(id, prov)) cout << "Actualizado." << endl;
                    else cout << "Error." << endl;
                }
                break;
            }
            case 4: {
                Proveedor lista[100];
                int c = obtenerProveedoresActivos(lista, 100);
                imprimirEncabezadoProveedores("LISTADO DE PROVEEDORES ACTIVOS");
                for (int i = 0; i < c; i++) imprimirLineaProveedor(lista[i]);
                break;
            }
            case 5: {
                int id = Validaciones::validarEntradaEntera("ID a eliminar (inactivar): ", 1, 10000);
                if (inactivarProveedor(id)) cout << "Inactivado." << endl;
                else cout << "Error." << endl;
                break;
            }
            case 6: {
                imprimirProveedoresInactivos();
                int id = Validaciones::validarEntradaEntera("ID a restaurar: ", 1, 10000);
                if (activarProveedor(id)) cout << "Restaurado." << endl;
                else cout << "Error." << endl;
                break;
            }
            default:
                break;
        }

        if (opcion != 0) limpiarPantallaConPausa();
    } while (opcion != 0);
}

void menuClientes() {
    int opcion;
    do {
        opcion = submenu_cliente();

        switch (opcion) {
            case 1: {
                Cliente p;
                char buf[100];

                cout << "Nombre: "; cin.getline(buf, 100); Formatos::convertirAMayusculas(buf); p.setNombre(buf);

                do {
                    cout << "Cedula/RIF (Ej. V-12345678): "; cin.getline(buf, 100);
                    Formatos::convertirAMayusculas(buf);
                    if (Validaciones::validarRIF(buf)) break;
                    cout << "Error: Cédula/RIF invalido. Ingrese nuevamente.\n";
                } while (true);
                p.setCedula(buf);

                registrarCliente(p);
                cout << "[Exito] ID: " << p.getId() << endl;
                break;
            }
            case 2: {
                char consulta[100];
                Cliente lista[100];
                cout << "ID o nombre a buscar: ";
                cin.getline(consulta, 100);

                if (esEnteroPositivo(consulta)) {
                    int id = atoi(consulta);
                    Cliente cliente;
                    if (obtenerClientePorId(id, cliente)) {
                        imprimirEncabezadoClientes("RESULTADO DE CLIENTE");
                        imprimirLineaCliente(cliente);
                    } else {
                        cout << "No encontrado." << endl;
                    }
                } else {
                    int cantidad = buscarClientesPorNombre(consulta, lista, 100);
                    if (cantidad == 0) {
                        cout << "No encontrado." << endl;
                    } else {
                        imprimirEncabezadoClientes("RESULTADOS DE CLIENTES");
                        for (int i = 0; i < cantidad; i++) imprimirLineaCliente(lista[i]);
                    }
                }
                break;
            }
            case 3: {
                int id = Validaciones::validarEntradaEntera("ID a actualizar: ", 1, 10000);
                Cliente cliente;
                if (obtenerClientePorId(id, cliente)) {
                    char buf[100];
                    cout << "Nuevo Nombre (actual: " << cliente.getNombre() << "): "; cin.getline(buf, 100);
                    if (buf[0] != '\0') {
                        Formatos::convertirAMayusculas(buf);
                        cliente.setNombre(buf);
                    }
                    cout << "Nueva Cedula/RIF (actual: " << cliente.getCedula() << "): "; cin.getline(buf, 100);
                    if (buf[0] != '\0') {
                        Formatos::convertirAMayusculas(buf);
                        cliente.setCedula(buf);
                    }
                    if (modificarCliente(id, cliente)) cout << "Actualizado." << endl;
                    else cout << "Error." << endl;
                }
                break;
            }
            case 4: {
                Cliente lista[100];
                int c = obtenerClientesActivos(lista, 100);
                imprimirEncabezadoClientes("LISTADO DE CLIENTES ACTIVOS");
                for (int i = 0; i < c; i++) imprimirLineaCliente(lista[i]);
                break;
            }
            case 5: {
                int id = Validaciones::validarEntradaEntera("ID a eliminar (inactivar): ", 1, 10000);
                if (inactivarCliente(id)) cout << "Inactivado." << endl;
                else cout << "Error." << endl;
                break;
            }
            case 6: {
                imprimirClientesInactivos();
                int id = Validaciones::validarEntradaEntera("ID a restaurar: ", 1, 10000);
                if (activarCliente(id)) cout << "Restaurado." << endl;
                else cout << "Error." << endl;
                break;
            }
            default:
                break;
        }

        if (opcion != 0) limpiarPantallaConPausa();
    } while (opcion != 0);
}

void verTienda() {
    Tienda t;
    if (obtenerDatosTienda(t)) {
        cout << "\n--- DATOS DE LA TIENDA ---" << endl;
        cout << "Nombre: " << t.getNombre() << " (RIF: " << t.getRif() << ")" << endl;
        cout << "CAJA ACTUAL: " << Formatos::formatearMoneda(t.getCaja()) << endl;
        cout << "Ingresos: " << Formatos::formatearMoneda(t.getIngresosTotales()) << " | Egresos: " << Formatos::formatearMoneda(t.getEgresosTotales()) << endl;
        cout << "Ventas emitidas: " << t.getContadorVentas() << " | Compras emitidas: " << t.getContadorCompras() << endl;
    }
}

void menuTransacciones() {
    int opcion;
    do {
        opcion = submenu_transaccion();

        switch (opcion) {
            case 1: {
                Transaccion t;
                char tipo = 'C';
                t.setTipo(tipo);

                int idRel = Validaciones::validarEntradaEntera("ID del Proveedor: ", 1, 10000);
                t.setIdRelacionado(idRel);

                char resp;
                do {
                    DetalleVenta dv;
                    int idP = Validaciones::validarEntradaEntera("ID del Producto: ", 1, 10000);
                    int cant = Validaciones::validarEntradaEntera("Cantidad: ", 1, 10000);
                    float precio = Validaciones::validarEntradaFlotante("Precio Unitario: ", 0, 1000000);

                    dv.setIdProducto(idP);
                    dv.setCantidad(cant);
                    dv.setPrecioUnitario(precio);
                    t.agregarDetalle(dv);

                    cout << "Desea agregar otro producto? (s/n): ";
                    cin >> resp;
                    Formatos::limpiarBuffer();
                } while (resp == 's' || resp == 'S');

                t.calcularTotal();
                if (registrarTransaccion(t)) {
                    procesarRegistroCompraTienda(t.getTotal());
                    cout << "Transaccion guardada. Monto total: " << Formatos::formatearMoneda(t.getTotal()) << endl;
                }
                break;
            }
            case 2: {
                Transaccion t;
                char tipo = 'V';
                t.setTipo(tipo);

                int idRel = Validaciones::validarEntradaEntera("ID del Cliente: ", 1, 10000);
                t.setIdRelacionado(idRel);

                char resp;
                do {
                    DetalleVenta dv;
                    int idP = Validaciones::validarEntradaEntera("ID del Producto: ", 1, 10000);
                    int cant = Validaciones::validarEntradaEntera("Cantidad: ", 1, 10000);
                    float precio = Validaciones::validarEntradaFlotante("Precio Unitario: ", 0, 1000000);

                    dv.setIdProducto(idP);
                    dv.setCantidad(cant);
                    dv.setPrecioUnitario(precio);
                    t.agregarDetalle(dv);

                    cout << "Desea agregar otro producto? (s/n): ";
                    cin >> resp;
                    Formatos::limpiarBuffer();
                } while (resp == 's' || resp == 'S');

                t.calcularTotal();
                if (registrarTransaccion(t)) {
                    procesarRegistroVentaTienda(t.getTotal());
                    cout << "Transaccion guardada. Monto total: " << Formatos::formatearMoneda(t.getTotal()) << endl;
                }
                break;
            }
            case 3: {
                int id = Validaciones::validarEntradaEntera("ID de la transaccion a buscar: ", 1, 10000);
                Transaccion transaccion;
                if (obtenerTransaccionPorId(id, transaccion)) {
                    mostrarTransaccion(transaccion);
                } else {
                    cout << "No encontrado." << endl;
                }
                break;
            }
            case 4: {
                Transaccion lista[100];
                int c = obtenerTransaccionesActivas(lista, 100);
                imprimirEncabezadoTransacciones("LISTADO DE TRANSACCIONES ACTIVAS");
                for (int i = 0; i < c; i++) {
                    imprimirLineaTransaccion(lista[i]);
                }
                break;
            }
            case 5: {
                int id = Validaciones::validarEntradaEntera("ID de la transaccion a anular: ", 1, 10000);
                if (anularTransaccion(id)) cout << "Inactivado." << endl;
                else cout << "Error." << endl;
                break;
            }
            case 6: {
                imprimirTransaccionesInactivas();
                int id = Validaciones::validarEntradaEntera("ID de la factura a restaurar: ", 1, 10000);
                if (activarTransaccion(id)) cout << "Restaurado." << endl;
                else cout << "Error." << endl;
                break;
            }
            default:
                break;
        }

        if (opcion != 0) limpiarPantallaConPausa();
    } while (opcion != 0);
}


void Interfaz::ejecutar() {
    configurarConsolaUTF8();
    inicializarTiendaPorDefecto();

    int opcion;
    do {
        Tienda miTienda;
        if (!obtenerDatosTienda(miTienda) || miTienda.getNombre()[0] == '\0' || miTienda.getRif()[0] == '\0') {
            inicializarTiendaPorDefecto();
            obtenerDatosTienda(miTienda);
        }
        mostrarMenuPrincipal(miTienda);
        opcion = Validaciones::validarEntradaEntera("Seleccione una opción: ", 1, 8);

        switch (opcion) {
            case 1: menuProductos(); break;
            case 2: menuProveedores(); break;
            case 3: menuClientes(); break;
            case 4: menuTransacciones(); break;
            case 5: verificarIntegridadReferencial(); limpiarPantallaConPausa(); break;
            case 6: generarReportes(); limpiarPantallaConPausa(); break;
            case 7: crearBackup(); limpiarPantallaConPausa(); break;
            case 8: 
                cout << "Cerrando sistema de " << miTienda.getNombre() << "..." << endl; 
                crearBackup(); // Backup automático al salir
                break;
        }
    } while (opcion != 8);
}