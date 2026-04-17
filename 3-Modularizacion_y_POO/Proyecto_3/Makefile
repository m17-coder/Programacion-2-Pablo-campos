CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11

TARGET = sistema.exe

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

OBJS = $(SRCS:.cpp=.o)
OBJS := $(OBJS:.CPP=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.CPP
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del /Q /F $(TARGET) *.o interfaz\*.o utilidades\*.o persistencia\*.o Productos\*.o Proveedores\*.o Clientes\*.o Tienda\*.o Transacciones\*.o 2>NUL