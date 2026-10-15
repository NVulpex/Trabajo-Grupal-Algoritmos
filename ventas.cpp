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

bool buscarMozo(int idIngresado, Mozo &mEncontrado) {
    FILE* arch = fopen("mozos.dat", "rb"); 
    if (!arch) return false;

    Mozo m;
    while (fread(&m, sizeof(Mozo), 1, arch) == 1) {
        if (m.idMozo == idIngresado) {
            mEncontrado = m;
            fclose(arch);
            return true;
        }
    }
    fclose(arch);
    return false;
}

int buscarPrecioYActualizarStock(int cod, int cant, float &precioEncontrado) {
    FILE* arch = fopen("inventario.dat", "r+b"); 
    if (!arch) return -1;

    fseek(arch, 0, SEEK_END);
    int n = ftell(arch) / sizeof(Producto);
    
    int pri = 0;
    int ult = n - 1;

    while (pri <= ult) {
        int med = (pri + ult) / 2;
        
        fseek(arch, med * sizeof(Producto), SEEK_SET);
        Producto p;
        fread(&p, sizeof(Producto), 1, arch);

        if (p.codigo == cod) {
            if (p.stockActual >= cant) {
                precioEncontrado = p.precio;
                p.stockActual -= cant; 
                
                fseek(arch, med * sizeof(Producto), SEEK_SET);
                fwrite(&p, sizeof(Producto), 1, arch);
                fclose(arch);
                return 0; 
            } else {
                fclose(arch);
                return -2; 
            }
        } else if (cod > p.codigo) {
            pri = med + 1;
        } else {
            ult = med - 1;
        }
    }
    fclose(arch);
    return -1; 
}

void ordenarArchivoPorMozo(char nombreArchivo[]) {
    FILE* arch = fopen(nombreArchivo, "rb");
    if (!arch) return;

    fseek(arch, 0, SEEK_END);
    int cantComandas = ftell(arch) / sizeof(Comanda);
    fseek(arch, 0, SEEK_SET);

    if (cantComandas <= 0) {
        fclose(arch);
        return;
    }

    Comanda* ventasDia = new Comanda[cantComandas]; 
    
    fread(ventasDia, sizeof(Comanda), cantComandas, arch);
    fclose(arch);

    for (int i = 0; i < cantComandas - 1; i++) {
        for (int j = 0; j < cantComandas - i - 1; j++) {
            if (ventasDia[j].idMozo > ventasDia[j+1].idMozo) {
                Comanda aux = ventasDia[j];
                ventasDia[j] = ventasDia[j+1];
                ventasDia[j+1] = aux;
            }
        }
    }

    arch = fopen(nombreArchivo, "wb");
    if (arch) {
        fwrite(ventasDia, sizeof(Comanda), cantComandas, arch);
        fclose(arch);
    }
    
    delete[] ventasDia;
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
        
        Mozo mActual;
        
        if (buscarMozo(idMozo, mActual)) {
            cout << "Clave: ";
            cin >> claveIngresada;
            
            encriptarClave(claveIngresada);
            
            if (textosIguales(mActual.password, claveIngresada)) {
                Comanda nuevaVenta;
                nuevaVenta.idMozo = idMozo;
                
                cout << "Codigo del Producto: ";
                cin >> nuevaVenta.codigoProducto;
                cout << "Cantidad a vender: ";
                cin >> nuevaVenta.cantidad;
                
                float precio = 0;
                int estadoStock = buscarPrecioYActualizarStock(nuevaVenta.codigoProducto, nuevaVenta.cantidad, precio);
                
                if (estadoStock == 0) {
                    nuevaVenta.comision = (precio * nuevaVenta.cantidad) * TASA_COMISION;
                    
                    FILE* archDia = fopen(nombreArchivoDia, "ab");
                    if (archDia) {
                        fwrite(&nuevaVenta, sizeof(Comanda), 1, archDia);
                        fclose(archDia);
                        cout << "Venta registrada. Comision: $" << nuevaVenta.comision << endl;
                    }
                } else if (estadoStock == -1) {
                    cout << "Error: Producto inexistente." << endl;
                } else if (estadoStock == -2) {
                    cout << "Error: Stock insuficiente." << endl;
                }
            } else {
                cout << "Error: Clave incorrecta." << endl;
            }
        } else {
            cout << "Error: Mozo inexistente." << endl;
        }
        
        cout << "\nCargar otra venta? (1 = Si / 0 = Terminar dia): ";
        cin >> opcion;
    }
    
    cout << "\nOrdenando las ventas por mozo..." << endl;
    ordenarArchivoPorMozo(nombreArchivoDia);
    cout << "Jornada finalizada." << endl;

    return 0;
}
