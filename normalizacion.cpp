#include <iostream>   // cin, cout, endl
#include <cstdio>     // FILE, fopen, fread, fwrite, fseek, fclose, sprintf
#include <cstring>    // strcpy, strcmp

using namespace std;

struct Mozo {
    int idMozo;
    char nombre[50];
    char password[20];
    float totalComision;
};

struct ComandaHistorica {
char fecha[11]; // "DD-MM-AAAA"
char nombreMozo[50]; // el nombre completo, repetido en cada venta
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
 
int main()
{
    File* comanda = fopen("comandas_historicas.dat","rb");
    File* inventario = fopen("inventario.dat","rb+");
    File* mozos = fopen("mozos.dat","wb");
   char c[20] = {'s','o','l'};
   encriptar(c,3,7);
    return 0;
}

int buscarMozos(Mozo m[], int len, char nombre[]){
    int i = 0;
    while(i<len && m[i].nombre!=nombre){
        i++;
    }
    if(i==len){
        return -1;
    }
    else
        return i;
}

void encriptar(char clave[], int l, int k) {
    for (int x = 0; x < l; x++) {
        clave[x] = clave[x] + k;
    }
}
