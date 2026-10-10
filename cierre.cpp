#include <stdio.h>

struct Comanda{
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

int main() {
    int numeroSemana, mes;
    char fecha[11]; //dd-mm-aaaa
    char nombreArchivo[50];
    char nombreArchivoSalida[50];

    FILE *archivo;
    FILE *archivoSalida;

    int cantidadComandas = 0;
    int i, j;

    printf("Ingrese el numero de semana: ");
    scanf("%d", &numeroSemana);

    printf("Ingrese el mes de la semana (1-12): ");
    scanf("%d", &mes);

    if(numeroSemana < 1 ||mes < 1 || mes > 12) {
        printf("Semana o mes invalido.\n");
        return 1;
    }

    // Primera pasada: contar cuantas comandas hay.
    for (i = 0; i < 7; i++) {
        printf("Ingrese la fecha del dia (DD-MM-AAAA): ", i + 1);
        scanf("%10s", fecha); //01-02-2026

        sprintf(nombreArchivo, "comandas_%s.dat", fecha);

        archivo = fopen(nombreArchivo, "rb");

        if (archivo != NULL) {
            Comanda comanda;

            while(fread(&comanda, sizeof(Comanda), 1, archivo) == 1) {
                cantidadComandas++;
            }

            fclose(archivo);
        }
    }

    // Reservar memoria para todas las comandas.
    Comanda *comandas = NULL;

    if (cantidadComandas > 0) {
        comandas = new Comanda[cantidadComandas];

        if (comandas == NULL) {
            printf("Error al reservar memoria.\n");
            return 1;
        }
    }

    // Segunda pasada: leer y juntar todas las comandas.
    int posicion = 0;

    for (i = 0; i < 7; i++) {
        char fechaDia[11];

        printf("Ingrese nuevamente la fecha del dia %d (DD-MM-AAAA): ",i + 1);
        scanf("%10s", fechaDia);

        sprintf(nombreArchivo, "comandas_%s.dat", fechaDia);

        archivo = fopen(nombreArchivo, "rb");

        if(archivo != NULL) { // && AND, || OR, == comparacion, != distinto
            while(fread(&comandas[posicion],sizeof(Comanda), 1, archivo) == 1) {
                posicion++;
            }

            fclose(archivo);
        }
    }

    // Ordenar las comandas por numero de mozo.
    for (i = 0; i < cantidadComandas - 1; i++){
        for (j = 0; j < cantidadComandas - i - 1; j++) {
            if (comandas[j].idMozo > comandas[j + 1].idMozo) {
                Comanda auxiliar = comandas[j];
                comandas[j] = comandas[j + 1];
                comandas[j + 1] = auxiliar;
            }
        }
    }

    // Crear el archivo semanal.
    sprintf(nombreArchivoSalida,"comandas_semana_s%d-%02d.dat",numeroSemana, mes);

    archivoSalida = fopen(nombreArchivoSalida, "wb");

    if (archivoSalida == NULL) {
        printf("No se pudo crear el archivo semanal.\n");
        delete[] comandas;
        return 1;
    }

    // Escribir las comandas ordenadas.
    if(cantidadComandas > 0) {
        fwrite(comandas, sizeof(Comanda),cantidadComandas, archivoSalida);
    }

    fclose(archivoSalida);

    printf("\nCierre semanal generado correctamente.\n");
    printf("Archivo: %s\n", nombreArchivoSalida);
    printf("Total de comandas: %d\n", cantidadComandas);

    return 0;
}
