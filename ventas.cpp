#include <iostream>
#include <cstdio>

using namespace std;

struct Mozo {
    int idMozo;
    char nombre[50];
    char password[20];
    float totalComision;
};

struct Producto {
    int codigo;
    char descripcion[50];
    float precio;
    int stockActual;
};

struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

const float TASA_COMISION = 0.10f;
const int K_SECRETO = 7; 

bool textosIguales(const char t1[], const char t2[]) {
    int i = 0;
    while (t1[i] != '\0' && t2[i] != '\0') {
        if (t1[i] != t2[i]) return false;
        i++;
    }
    return t1[i] == t2[i];
}

void encriptarClave(char clave[]) {
    int i = 0;
    while (clave[i] != '\0') {
        clave[i] = clave[i] + K_SECRETO;
        i++;
    }
}

bool validarLogin(int idIngresado, char claveIngresada[]) {
    encriptarClave(claveIngresada);

    FILE* arch = fopen("mozos.dat", "rb"); 
    if (!arch) return false;

    Mozo m;
    bool exito = false;
    
    while (fread(&m, sizeof(Mozo), 1, arch) == 1) {
        if (m.idMozo == idIngresado && textosIguales(m.password, claveIngresada)) {
            exito = true;
            break;
        }
    }
    fclose(arch);
    return exito;
}

float buscarPrecioYActualizarStock(int cod, int cant) {
    FILE* arch = fopen("inventario.dat", "r+b"); 
    if (!arch) return 0.0f;

    Producto p;
    float precioEncontrado = 0.0f;
    
    while (fread(&p, sizeof(Producto), 1, arch) == 1) {
        if (p.codigo == cod) {
            if (p.stockActual >= cant) {
                precioEncontrado = p.precio;
                p.stockActual -= cant; 
                
                fseek(arch, -sizeof(Producto), SEEK_CUR);
                fwrite(&p, sizeof(Producto), 1, arch);
            }
            break;
        }
    }
    fclose(arch);
    return precioEncontrado;
}

void ordenarArchivoPorMozo(char nombreArchivo[]) {
    FILE* arch = fopen(nombreArchivo, "rb");
    if (!arch) return;

    Comanda ventasDia[500]; 
    int len = 0;
    
    while (fread(&ventasDia[len], sizeof(Comanda), 1, arch) == 1) {
        len++;
    }
    fclose(arch);

    for (int i = 0; i < len - 1; i++) {
        for (int j = 0; j < len - i - 1; j++) {
            if (ventasDia[j].idMozo > ventasDia[j+1].idMozo) {
                Comanda aux = ventasDia[j];
                ventasDia[j] = ventasDia[j+1];
                ventasDia[j+1] = aux;
            }
        }
    }

    arch = fopen(nombreArchivo, "wb");
    if (arch) {
        fwrite(ventasDia, sizeof(Comanda), len, arch);
        fclose(arch);
    }
}

int main() {
    char fecha[11];
    char nombreArchivoDia[50];
    
    cout << "Ingrese la fecha de hoy (DD-MM-AAAA): ";
    cin >> fecha;
    
    sprintf(nombreArchivoDia, "comandas_%s.dat", fecha);
    
    int opcion = 1;
    
    while (opcion != 0) {
        int idMozo;
        char claveIngresada[20];
        
        cout << "\nID del Mozo: ";
        cin >> idMozo;
        cout << "Clave: ";
        cin >> claveIngresada;
        
        if (validarLogin(idMozo, claveIngresada)) {
            Comanda nuevaVenta;
            nuevaVenta.idMozo = idMozo;
            
            cout << "Codigo del Producto: ";
            cin >> nuevaVenta.codigoProducto;
            cout << "Cantidad a vender: ";
            cin >> nuevaVenta.cantidad;
            
            float precio = buscarPrecioYActualizarStock(nuevaVenta.codigoProducto, nuevaVenta.cantidad);
            
            if (precio > 0) {
                nuevaVenta.comision = (precio * nuevaVenta.cantidad) * TASA_COMISION;
                
                FILE* archDia = fopen(nombreArchivoDia, "ab");
                if (archDia) {
                    fwrite(&nuevaVenta, sizeof(Comanda), 1, archDia);
                    fclose(archDia);
                    cout << "Venta registrada. Comision: $" << nuevaVenta.comision << endl;
                }
            } else {
                cout << "Error: Producto inexistente o stock insuficiente." << endl;
            }
        } else {
            cout << "Acceso denegado: ID o clave incorrectos." << endl;
        }
        
        cout << "\nCargar otra venta? (1 = Si / 0 = Terminar dia): ";
        cin >> opcion;
    }
    
    cout << "\nOrdenando las ventas por mozo..." << endl;
    ordenarArchivoPorMozo(nombreArchivoDia);
    cout << "Jornada finalizada." << endl;

    return 0;
}