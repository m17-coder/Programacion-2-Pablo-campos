#include <string>
#include <ctime>
#include <iomanip>
#include <cmath>
        std::string obtenerFechaActual() {
            time_t ahora = time(0);
            tm *ltm = localtime(&ahora);
            std::string fecha = "";
            int dia = ltm->tm_mday;
            int mes = 1 + ltm->tm_mon;
            int anio = 1900 + ltm->tm_year;
            if (dia < 10) fecha += "0";
            fecha += std::to_string(dia) + "/";
            if (mes < 10) fecha += "0";
            fecha += std::to_string(mes) + "/";
            fecha += std::to_string(anio);
            return fecha;
}
        std::string obtenerHoraActual() {
            time_t ahora = time(0);
            tm *ltm = localtime(&ahora);
            std::string hora = "";
            if (ltm->tm_hour < 10) hora += "0";
            hora += std::to_string(ltm->tm_hour) + ":";
            if (ltm->tm_min < 10) hora += "0";
            hora += std::to_string(ltm->tm_min) + ":";
            if (ltm->tm_sec < 10) hora += "0";
            hora += std::to_string(ltm->tm_sec);
            return hora;
}
        std::string aMayusculas(std::string texto) {
            std::string resultado = "";
            for (int i = 0; i < (int)texto.length(); i++) {
            char c = texto[i];
            if (c >= 'a' && c <= 'z') {
            resultado += (char)(c - 32);
            } else {
            resultado += c;
            }
        }
            return resultado;
}

        std::string aMinusculas(std::string texto) {
            std::string resultado = "";
            for (int i = 0; i < (int)texto.length(); i++) {
            char c = texto[i];
            if (c >= 'A' && c <= 'Z') {
            resultado += (char)(c + 32);
            } else {
            resultado += c;
            }
        }
            return resultado;
}
        std::string recortarEspacios(std::string texto) {
            int inicio = 0, fin = texto.length() - 1;
            while (inicio <= fin && texto[inicio] == ' ') inicio++;
            while (fin >= inicio && texto[fin] == ' ') fin--;
            std::string resultado = "";
            for (int i = inicio; i <= fin; i++) {
            resultado += texto[i];
            }
            return resultado;
}

        std::string rellenarDerecha(std::string texto, int largo, char relleno) {
            std::string resultado = texto;
            while ((int)resultado.length() < largo) {
            resultado += relleno;
            }
            return resultado;
}

        std::string rellenarIzquierda(std::string texto, int largo, char relleno) {
            std::string resultado = texto;
            while ((int)resultado.length() < largo) {
            resultado = relleno + resultado;
            }
            return resultado;
}

        std::string lineaSeparadora(int largo, char caracter) {
            std::string linea = "";
            for (int i = 0; i < largo; i++) linea += caracter;
            return linea;
}

        double redondear(double valor, int decimales) {
            double factor = pow(10, decimales);
            return round(valor * factor) / factor;
}

        bool compararIgnorandoCase(std::string a, std::string b) {
            if (a.length() != b.length()) return false;
            for (int i = 0; i < (int)a.length(); i++) {
            char ca = a[i], cb = b[i];
            if (ca >= 'A' && ca <= 'Z') ca += 32;
            if (cb >= 'A' && cb <= 'Z') cb += 32;
            if (ca != cb) return false;
    }
            return true;
}

        bool contieneTexto(std::string texto, std::string busqueda) {
            std::string textoMin = aMinusculas(texto);
            std::string busqMin = aMinusculas(busqueda);
            for (int i = 0; i <= (int)textoMin.length() - (int)busqMin.length(); i++) {
            bool encontrado = true;
            for (int j = 0; j < (int)busqMin.length(); j++) {
            if (textoMin[i + j] != busqMin[j]) {
                encontrado = false;
                break;
                }
            }
            if (encontrado) return true;
    }
            return false;
}

        // formatea un numero con decimales fijos
        std::string formatoNota(double nota) {
            std::string resultado = std::to_string(redondear(nota, 2));
            // recortar a 2 decimales
            int puntoPos = -1;
            for (int i = 0; i < (int)resultado.length(); i++) {
            if (resultado[i] == '.') { puntoPos = i; break; }
        }
            if (puntoPos >= 0 && (int)resultado.length() > puntoPos + 3) {
            resultado = resultado.substr(0, puntoPos + 3);
        }
            return resultado;
}

        // genera una barra visual de progreso con caracteres
        std::string barraProgreso(double porcentaje, int largo) {
            int llenos = (int)(porcentaje / 100.0 * largo);
            if (llenos > largo) llenos = largo;
            if (llenos < 0) llenos = 0;
            std::string barra = "[";
            for (int i = 0; i < largo; i++) {
            if (i < llenos) barra += "#";
            else barra += ".";
        }
            barra += "] " + formatoNota(porcentaje) + "%";
            return barra;
}
