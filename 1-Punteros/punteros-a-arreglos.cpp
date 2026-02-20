#include <iostream>
#include <winbase.h>

using namespace std;
 
void obtenerFechaActual(char* buffer){
    SYSTEMTIME st;
    GetLocalTime(&st);
    sprintf(buffer, "%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay);
}
int main () {
   char buffer[11];
   obtenerFechaActual(buffer);
   cout<< "fecha actual: "<<buffer<<endl;
    return 0;
}