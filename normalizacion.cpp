#include <iostream>
#include <cstdio>
#include <cstring>
using namespace std;

struct Mozo {
    int idMozo;
    char nombre[50];
    char password[20];
    float totalComision;
};

struct ComandaHistorica {
    char fecha[11];
    char nombreMozo[50];
    int codigoProducto;
    int cantidad;
    float comision;
};

struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

struct Producto {
    int codigo;
    char descripcion[50];
    float precio;
    int stockActual;
};

void encriptar(char clave[], int l, int k);
int buscarMozos(Mozo m[], int len, char nombre[]);
void ordenarComandas(Comanda c[], int len);
int buscarFecha(char fechas[][11], int len, char fecha[]);
int buscarProducto(FILE* inventario, int codigo);
void obtenerFecha(char fecha[], int &dia, int &mes, int &anio);
void agregarComandaArchivo(ComandaHistorica ch,Mozo mozos[],int cantMozos);
void ordenarArchivoComandas(char nombreArchivo[]);

// Hace todo el proceso: crea los mozos, genera las planillas por día, las ordena y actualiza el stock
int main()
{
    FILE* comanda = fopen("comandas_historicas.dat", "rb");
    FILE* inventario = fopen("inventario.dat", "rb+");
    FILE* Mozos = fopen("mozos.dat", "wb");
    if(comanda == NULL || inventario == NULL || Mozos == NULL){
      cout << "Error al abrir los archivos." << endl;
      return 1;
    }
    Mozo mozos[100];
    int Cantmozos = 0;

    ComandaHistorica ch;

    //Crear los mozos y acumular sus comisiones
    while(fread(&ch, sizeof(ComandaHistorica), 1, comanda) == 1){
        int pos = buscarMozos(mozos, Cantmozos, ch.nombreMozo);

        if(pos == -1){                                        // si no encuentra el mozo, lo agrega al array de mozos
            mozos[Cantmozos].idMozo = Cantmozos + 1;
            strcpy(mozos[Cantmozos].nombre, ch.nombreMozo);
            mozos[Cantmozos].totalComision = ch.comision;
            sprintf(mozos[Cantmozos].password, "%d", mozos[Cantmozos].idMozo);
            encriptar(mozos[Cantmozos].password, strlen(mozos[Cantmozos].password), 7);
            Cantmozos++;
        }
        else{
            mozos[pos].totalComision += ch.comision;
        }
    }

    //Guardar los mozos
    for(int i = 0; i < Cantmozos; i++){
        fwrite(&mozos[i], sizeof(Mozo), 1, Mozos);
    }

    fclose(Mozos);

    //Generar una planilla por día
    char fechas[400][11];
    int cantFechas = 0;

    fseek(comanda, 0, SEEK_SET);

    while(fread(&ch, sizeof(ComandaHistorica), 1, comanda) == 1){
      agregarComandaArchivo(ch, mozos, Cantmozos);

      if(buscarFecha(fechas, cantFechas, ch.fecha) == -1){
          strcpy(fechas[cantFechas], ch.fecha);
          cantFechas++;
      }
    }

    //Ordenar cada planilla una sola vez
    for(int i = 0; i < cantFechas; i++){
        int dd, mm, aaaa;
        char nombreArchivo[50];

        obtenerFecha(fechas[i], dd, mm, aaaa);
        sprintf(nombreArchivo, "comandas_%02d-%02d-%04d.dat", dd, mm, aaaa);

        ordenarArchivoComandas(nombreArchivo);
    }

    //Actualizar el stock del inventario
    fseek(comanda, 0, SEEK_SET);

    while(fread(&ch, sizeof(ComandaHistorica), 1, comanda) == 1){
        int pos = buscarProducto(inventario, ch.codigoProducto);

        if(pos != -1){
            fseek(inventario, pos * sizeof(Producto), SEEK_SET);
            Producto p;
            fread(&p, sizeof(Producto), 1, inventario);

            if(p.stockActual-ch.cantidad >= 0)
              p.stockActual -= ch.cantidad;
            else{
              cout << "Error -> stock insuficiente"<<endl;
              cout <<"Cantidad de stock actual : "<<p.stockActual<<" Cantidad de stock pedido: "<<ch.cantidad;
            }

            fseek(inventario, -(long)sizeof(Producto), SEEK_CUR);

            fwrite(&p, sizeof(Producto), 1, inventario);
        }
    }

    fclose(comanda);
    fclose(inventario);

    return 0;
}

// Busca un mozo por nombre y devuelve su posición, o -1 si no está
int buscarMozos(Mozo m[], int len, char nombre[]){
    int i = 0;

    while(i < len && strcmp(m[i].nombre, nombre) != 0){
        i++;
    }
    if(i == len){
        return -1;
    }
    else{
        return i;
    }
}

// Le suma k a cada carácter de la clave para que no se lea tal cual
void encriptar(char clave[], int l, int k){
    for(int x = 0; x < l; x++){
        clave[x] = clave[x] + k;
    }
}

// Ordena un vector de comandas por idMozo (burbuja)
void ordenarComandas(Comanda c[], int len){
    for(int i = 0; i < len - 1; i++){
        for(int j = 0; j < len - 1 - i; j++){
            if(c[j].idMozo > c[j + 1].idMozo){
                Comanda aux = c[j];
                c[j] = c[j + 1];
                c[j + 1] = aux;
            }
        }
    }
}

// Busca una fecha en la lista y devuelve su posición, o -1 si no está
int buscarFecha(char fechas[][11], int len, char fecha[]){
    int i = 0;

    while(i < len && strcmp(fechas[i], fecha) != 0){
        i++;
    }
    if(i == len)
        return -1;
    else
        return i;
}

// Busca un producto por código en el inventario (búsqueda binaria) y devuelve su posición, o -1 si no existe
int buscarProducto(FILE* inventario, int codigo){
    fseek(inventario, 0, SEEK_END);

    int n = ftell(inventario) / sizeof(Producto);
    int pri = 0;
    int ult = n - 1;

    while(pri <= ult)
    {
        int med = (pri + ult) / 2;
        fseek(inventario, med * sizeof(Producto), SEEK_SET);
        Producto p;
        fread(&p, sizeof(Producto), 1, inventario);

        if(p.codigo == codigo){
            return med;
        }
        else if(codigo > p.codigo){
            pri = med + 1;
        }
        else{
            ult = med - 1;
        }
    }
    return -1;
}

// Separa una fecha "DD-MM-AAAA" en día, mes y año
void obtenerFecha(char fecha[], int &dd, int &mm, int &aaaa)
{
    sscanf(fecha, "%d-%d-%d", &dd, &mm, &aaaa);
}

// Guarda la venta (con el ID del mozo) en el archivo de su día, creándolo si no existe
void agregarComandaArchivo(ComandaHistorica ch,Mozo mozos[],int cantMozos){
    int dd, mm, aaaa;
    obtenerFecha(ch.fecha,dd,mm,aaaa);

    char nombreArchivo[50];

    sprintf(nombreArchivo, "comandas_%02d-%02d-%04d.dat",dd,mm,aaaa);

    int posMozo = buscarMozos(mozos, cantMozos, ch.nombreMozo);

    if(posMozo == -1){
        cout <<"No se encontro el mozo: "<<ch.nombreMozo<< endl;
        return;
    }

    Comanda c;
    c.idMozo = mozos[posMozo].idMozo;
    c.codigoProducto = ch.codigoProducto;
    c.cantidad = ch.cantidad;
    c.comision = ch.comision;

    FILE* archivo = fopen(nombreArchivo, "ab");
    if(archivo == NULL){
        cout << "Error al abrir " << nombreArchivo << endl;
        return;
    }

    fwrite(&c, sizeof(Comanda), 1, archivo);
    fclose(archivo);
}

// Lee un archivo de día a memoria, lo ordena por mozo y lo reescribe
void ordenarArchivoComandas(char nombreArchivo[]){
    FILE* f = fopen(nombreArchivo, "rb");
    if(f == NULL) return;

    fseek(f, 0, SEEK_END);
    int n = ftell(f) / sizeof(Comanda);
    fseek(f, 0, SEEK_SET);

    Comanda* c = new Comanda[n];
    int i = 0;
    while(fread(&c[i], sizeof(Comanda), 1, f) == 1){
        i++;
    }
    fclose(f);

    ordenarComandas(c, n);

    f = fopen(nombreArchivo, "wb");
    for(int j = 0; j < n; j++){
        fwrite(&c[j], sizeof(Comanda), 1, f);
    }
    fclose(f);

    delete[] c;
}
