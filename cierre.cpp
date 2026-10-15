#include <stdio.h>
#include <iostream>
using namespace std;

struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

int main() {
    int numeroSemana, mes, numDias;
    char fecha[7][11];
    char nombreArchivo[50];
    char nombreArchivoSalida[50];

    FILE *archivos[7];
    FILE *archivoSalida;

    // Guarda la comanda actual de cada archivo.
    Comanda actual[7];

    // Indica si cada archivo tiene una comanda disponible.
    bool disponible[7];

    int i;
    int totalComandas = 0;

    // Inicializar los archivos.
    for (i = 0; i < 7; i++) {
        archivos[i] = NULL;
        disponible[i] = false;
    }

    printf("Ingrese el numero de semana (1-4): ");
    scanf("%d", &numeroSemana);

    cout << "Cuantos dias de la semana pasaron (1-7): ";
    cin >> numDias;

    while (numDias < 1 || numDias > 7) {
        cout << "ERROR: ingrese un numero entre 1 y 7: ";
        cin >> numDias;
    }

    printf("Ingrese el mes de la semana (1-12): ");
    scanf("%d", &mes);

    if (numeroSemana < 1 || numeroSemana > 4 ||
        mes < 1 || mes > 12) {
        printf("Semana o mes invalido.\n");
        return 1;
    }

    // Ingresar las fechas una sola vez.
    for (i = 0; i < numDias; i++) {
        printf("Ingrese la fecha del dia (DD-MM-AAAA): ");
        scanf("%10s", fecha[i]);
    }

    // Crear el archivo semanal.
    sprintf(nombreArchivoSalida,
            "comandas_semana_s%d-%02d.dat",
            numeroSemana, mes);

    archivoSalida = fopen(nombreArchivoSalida, "wb");

    if (archivoSalida == NULL) {
        printf("No se pudo crear el archivo semanal.\n");
        return 1;
    }

    // Abrir las planillas diarias y leer la primera comanda.
    for (i = 0; i < numDias; i++) {
        sprintf(nombreArchivo, "comandas_%s.dat", fecha[i]);

        archivos[i] = fopen(nombreArchivo, "rb");

        if (archivos[i] == NULL) {
            printf("Aviso: no se pudo abrir %s\n", nombreArchivo);
        } else {
            if (fread(&actual[i], sizeof(Comanda), 1,
                      archivos[i]) == 1) {
                disponible[i] = true;
            }
        }
    }

    // APAREO: elegir la menor ID entre las comandas disponibles.
    while (true) {
        int menor = -1;

        // Buscar qué archivo tiene la menor ID de mozo.
        for (i = 0; i < numDias; i++) {
            if (disponible[i]) {
                if (menor == -1 ||
                    actual[i].idMozo < actual[menor].idMozo) {
                    menor = i;
                }
            }
        }

        // Si no quedan comandas, termina el apareo.
        if (menor == -1) {
            break;
        }

        // Escribir la comanda con menor ID en el archivo semanal.
        if (fwrite(&actual[menor], sizeof(Comanda), 1,
                   archivoSalida) != 1) {
            printf("Error al escribir el archivo semanal.\n");
            break;
        }

        totalComandas++;

        // Leer la siguiente comanda del archivo elegido.
        if (fread(&actual[menor], sizeof(Comanda), 1,
                  archivos[menor]) != 1) {
            disponible[menor] = false;
        }
    }

    // Cerrar los archivos diarios.
    for (i = 0; i < numDias; i++) {
        if (archivos[i] != NULL) {
            fclose(archivos[i]);
        }
    }

    fclose(archivoSalida);

    printf("\nCierre semanal generado correctamente.\n");
    printf("Archivo: %s\n", nombreArchivoSalida);
    printf("Total de comandas: %d\n", totalComandas);

    return 0;
}
