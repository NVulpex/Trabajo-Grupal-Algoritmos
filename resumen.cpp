#include <iostream>
#include <cstdio>
#include <string>
#include <iomanip>

using namespace std;

struct Mozo {
    int idMozo;
    char nombre[50];
    char password[20];
    float totalComision;
};

struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

string obtenerNombreMozo(int idBuscado) {
    FILE* fMozos = fopen("mozos.dat", "rb");
    if (!fMozos) {
        return "Desconocido";
    }

    Mozo m;
    while (fread(&m, sizeof(Mozo), 1, fMozos) == 1) {
        if (m.idMozo == idBuscado) {
            fclose(fMozos);
            return string(m.nombre);
        }
    }

    fclose(fMozos);
    return "Desconocido";
}

int main() {
    string nombreArchivoSemanal;

    cout << "==========================================" << endl;
    cout << "    BUFFET ALBERT EINSTEIN - RESUMEN      " << endl;
    cout << "==========================================" << endl;
    cout << "Ingrese el nombre del archivo semanal (ej: comandas_semana_s1-06.dat): ";
    cin >> nombreArchivoSemanal;

    FILE* fSemanal = fopen(nombreArchivoSemanal.c_str(), "rb");
    if (!fSemanal) {
        cout << "\nError: No se pudo abrir el archivo '" << nombreArchivoSemanal << "'." << endl;
        return 1;
    }

    cout << "\n----------------------------------------------------------------------" << endl;
    cout << left << setw(10) << "ID MOZO" 
         << setw(30) << "NOMBRE DEL MOZO" 
         << setw(18) << "CANT. PRODUCTOS" 
         << setw(15) << "COMISION ($)" << endl;
    cout << "----------------------------------------------------------------------" << endl;

    Comanda reg;
    int totalProductosBuffet = 0;
    
    if (fread(&reg, sizeof(Comanda), 1, fSemanal) == 1) {
        
        while (!feof(fSemanal) && !ferror(fSemanal)) {
            int mozoActual = reg.idMozo;
            int productosPorMozo = 0;
            float comisionPorMozo = 0.0f;
            while (!feof(fSemanal) && reg.idMozo == mozoActual) {
                productosPorMozo += reg.cantidad;
                comisionPorMozo += reg.comision;
                if (fread(&reg, sizeof(Comanda), 1, fSemanal) != 1) {
                    break;
                }
            }
            
            string nombreMozo = obtenerNombreMozo(mozoActual);
            cout << left << setw(10) << mozoActual 
                 << setw(30) << nombreMozo 
                 << right << setw(15) << productosPorMozo 
                 << setw(15) << fixed << setprecision(2) << comisionPorMozo << endl;
            totalProductosBuffet += productosPorMozo;
        }
    } else {
        cout << "El archivo semanal no contiene registros." << endl;
    }

    fclose(fSemanal);

    cout << "----------------------------------------------------------------------" << endl;
    cout << "TOTAL DE PRODUCTOS VENDIDOS POR EL BUFFET EN LA SEMANA: " 
         << totalProductosBuffet << " unidades." << endl;
    cout << "================================================================------" << endl;

    return 0;
}