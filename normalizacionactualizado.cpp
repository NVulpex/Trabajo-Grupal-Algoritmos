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

int main()
{
    FILE* comanda = fopen("comandas_historicas.dat", "rb");
    FILE* inventario = fopen("inventario.dat", "rb+");
    FILE* Mozos = fopen("mozos.dat", "wb");
    if(comanda == NULL || inventario == NULL || Mozos == NULL)
{
    cout << "Error al abrir los archivos." << endl;
    return 1;
}
    Mozo mozos[100];
    int Cantmozos = 0;

    ComandaHistorica ch;

    // Crear los mozos y acumular sus comisiones
    while(fread(&ch, sizeof(ComandaHistorica), 1, comanda) == 1)
    {
        int pos = buscarMozos(mozos, Cantmozos, ch.nombreMozo);

        if(pos == -1)                                        // si no encuentra el mozo, lo agrega al array de mozos
        {
            mozos[Cantmozos].idMozo = Cantmozos + 1;

            strcpy(mozos[Cantmozos].nombre, ch.nombreMozo);

            mozos[Cantmozos].totalComision = ch.comision;

            sprintf(mozos[Cantmozos].password, "%d",
                    mozos[Cantmozos].idMozo);

            encriptar(mozos[Cantmozos].password,
                      strlen(mozos[Cantmozos].password),
                      7);

            Cantmozos++;
        }
        else
        {
            mozos[pos].totalComision += ch.comision;
        }
    }

    // Guardar los mozos
    for(int i = 0; i < Cantmozos; i++)
    {
        fwrite(&mozos[i], sizeof(Mozo), 1, Mozos);
    }

    fclose(Mozos);

    // Obtener las fechas diferentes
    Comanda comandas[100];
    int cantComandas = 0;

    char fechas[100][11];
    int cantFechas = 0;

    fseek(comanda, 0, SEEK_SET);

    while(fread(&ch, sizeof(ComandaHistorica), 1, comanda) == 1)
    {
        int pos = buscarFecha(fechas, cantFechas, ch.fecha);

        if(pos == -1)
        {
            strcpy(fechas[cantFechas], ch.fecha);
            cantFechas++;
        }
    }

    // Crear un archivo diario por cada fecha
    for(int f = 0; f < cantFechas; f++)
    {
        cantComandas = 0;

        fseek(comanda, 0, SEEK_SET);

        while(fread(&ch, sizeof(ComandaHistorica), 1, comanda) == 1)
        {
            if(strcmp(ch.fecha, fechas[f]) == 0)
            {
                int posMozo = buscarMozos(mozos, Cantmozos, ch.nombreMozo);

                comandas[cantComandas].idMozo = mozos[posMozo].idMozo;
                comandas[cantComandas].codigoProducto = ch.codigoProducto;
                comandas[cantComandas].cantidad = ch.cantidad;
                comandas[cantComandas].comision = ch.comision;

                cantComandas++;
            }
        }

        // Ordenar las comandas por id de mozo
        ordenarComandas(comandas, cantComandas);

        char nombreArchivo[30];

        sprintf(nombreArchivo, "comandas_%s.dat", fechas[f]); // Crear el nombre del archivo diario para la fecha actual

        FILE* diario = fopen(nombreArchivo, "wb");
      
        if(diario == NULL)
{
    cout << "Error al crear el archivo diario." << endl;
    return 1;
}

        for(int i = 0; i < cantComandas; i++)
        {
            fwrite(&comandas[i], sizeof(Comanda), 1, diario);
        }

        fclose(diario);
    }

    // Actualizar el stock del inventario
    fseek(comanda, 0, SEEK_SET);

    while(fread(&ch, sizeof(ComandaHistorica), 1, comanda) == 1)
    {
        int pos = buscarProducto(inventario, ch.codigoProducto);

        if(pos != -1)
        {
            fseek(inventario, pos * sizeof(Producto), SEEK_SET);

            Producto p;

            fread(&p, sizeof(Producto), 1, inventario);

            p.stockActual -= ch.cantidad;

            fseek(inventario, -(long)sizeof(Producto), SEEK_CUR);

            fwrite(&p, sizeof(Producto), 1, inventario);
        }
    }

    fclose(comanda);
    fclose(inventario);

    return 0;
}


int buscarMozos(Mozo m[], int len, char nombre[])
{
    int i = 0;

    while(i < len && strcmp(m[i].nombre, nombre) != 0)
    {
        i++;
    }

    if(i == len)
    {
        return -1;
    }
    else
    {
        return i;
    }
}

void encriptar(char clave[], int l, int k)
{
    for(int x = 0; x < l; x++)
    {
        clave[x] = clave[x] + k;
    }
}

void ordenarComandas(Comanda c[], int len) // ordenamiento burbuja por id de mozo
{
    for(int i = 0; i < len - 1; i++)
    {
        for(int j = 0; j < len - 1 - i; j++)
        {
            if(c[j].idMozo > c[j + 1].idMozo)
            {
                Comanda aux = c[j];
                c[j] = c[j + 1];
                c[j + 1] = aux;
            }
        }
    }
}



int buscarFecha(char fechas[][11], int len, char fecha[])
{
    int i = 0;

    while(i < len && strcmp(fechas[i], fecha) != 0)
    {
        i++;
    }

    if(i == len)
        return -1;
    else
        return i;
}


int buscarProducto(FILE* inventario, int codigo)   //busqueda binaria de un producto en el inventario por su codigo
{
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

        if(p.codigo == codigo)
        {
            return med;
        }
        else if(codigo > p.codigo)
        {
            pri = med + 1;
        }
        else
        {
            ult = med - 1;
        }
    }

    return -1;
}
