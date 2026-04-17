CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11

# Nombre del ejecutable
TARGET = sistema.exe

# Lista de todos los archivos fuente (.cpp y .CPP)
SRCS = main_final.cpp \
       interfaz/Interfaz.cpp \
       utilidades/VALIDACIONES.CPP \
       utilidades/FORMATOS.CPP \
       persistencia/GestorArchivos.cpp \
       Productos/PRODUCTOS.CPP \
       Productos/OPERACIONESPRODCUTOS.CPP \
       Proveedores/PROVEEDORES.CPP \
       Proveedores/OPERACIONESPROVEEDORES.CPP \
       Clientes/CLIENTES.CPP \
       Clientes/OPERACIONESCLIENTES.CPP \
       Tienda/TIENDA.CPP \
       Tienda/OPERACIONESTIENDA.CPP \
       Transacciones/TRANSACCIONES.CPP \
       Transacciones/OPERACIONESTRANSACCIONES.CPP

# Transformar la lista de fuentes a objetos (.o)
OBJS = $(SRCS:.cpp=.o)
OBJS := $(OBJS:.CPP=.o)

# Default: Compilar el programa
all: $(TARGET)

# Enlazar los objetos para crear el ejecutable final
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Reglas genéricas para compilar de source a objeto
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.CPP
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Comando para limpiar archivos compilados
clean:
	del /S /Q *.o
	del /Q $(TARGET)
