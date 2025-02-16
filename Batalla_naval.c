//batalla naval
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define TAMANO_TABLERO 10
#define AGUA '~'
#define BARCO 'B'
#define IMPACTO 'X'
#define FALLO 'O'

// Estructura para representar un barco
typedef struct {
    int tamaño;
    char nombre[20];
    int cantidad;
    char **posiciones;
    int vidas;
} Barco;

// Estructura para almacenar los datos del jugador
typedef struct {
    char nombre[50];
    char **tablero;
    int barcos_restantes;
    Barco *barcos;
    int cantidad_barcos;
} Jugador;

// Prototipos de funciones
void solicitar_datos(Jugador *jugador, Jugador *jugador2);
void asignar_memoria_tableros(Jugador *jugador1, Jugador *jugador2);
void inicializar_tablero(char **tablero);
void mostrar_tablero_barcos(char **tablero, bool mostrar_barcos, char nombre[50]);
void mostrar_tablero_ataques(char **tablero, bool mostrar_barcos, char nombre[50]);
void inicializar_barcos(Jugador*jugador);
void seleccionar_barcos(Jugador*jugador);
int colocar_barcos(Jugador *jugador, int tipo_barco,int barcos_colocados);
bool es_posicion_valida(char **tablero, int fila, int columna, int tamaño, char orientacion);
char orientar_barcos(char orientacion_num,char orientacion);
void colocar_barco_en_tablero(char **tablero, int fila, int columna, int tamaño, char orientacion,char**posiciones);
void partida(Jugador *turno_actual, Jugador *oponente, bool *juego_terminado);
bool realizar_ataque(Jugador *defensor, int fila, int columna);
void mostrar_vidas_barcos(Jugador *vidas);
bool verificar_ganador(const Jugador *jugador);
void guardar_configuracion_inicial(Jugador *jugador1,Jugador *jugador2);
void guardar_resultado_turno(const char *archivo, const char *jugador, int fila, int columna, bool impacto,Jugador *oponente);
void guardar_resultado_final(const char *archivo, Jugador *ganador,Jugador *defensor);
void liberar_memoria(const Jugador *jugador);

int main() {
    // Inicialización de los jugadores
    Jugador jugador1, jugador2;
    solicitar_datos(&jugador1, &jugador2);

    // Asignar memoria para los tableros
    asignar_memoria_tableros(&jugador1, &jugador2);

    // Inicializar los tableros
    inicializar_tablero(jugador1.tablero);
    inicializar_tablero(jugador2.tablero);
    // Inicializar barcos
    inicializar_barcos(&jugador1);
    inicializar_barcos(&jugador2);
    // Colocar los barcos
    printf("\nBarcos para %s:\n", jugador1.nombre);
    seleccionar_barcos(&jugador1);

    printf("\nBarcos para %s:\n", jugador2.nombre);
    seleccionar_barcos(&jugador2);

    // Guardar configuración inicial
    guardar_configuracion_inicial(&jugador1,&jugador2);

    // Juego por turnos
    bool juego_terminado = false;
    Jugador *turno_actual = &jugador1;
    Jugador *oponente = &jugador2;
    partida(turno_actual, oponente, &juego_terminado);

    // Liberar memoria
    liberar_memoria(&jugador1);
    liberar_memoria(&jugador2);

    return 0;
}

void solicitar_datos(Jugador *jugador, Jugador *jugador2) {
    printf("Ingrese el nombre del Jugador 1, (sin espacios):");
    scanf(" %49s", jugador->nombre);
    while ((getchar()) != '\n');
    printf("Ingrese el nombre del Jugador 2, (sin espacios):");
    scanf(" %49s", jugador2->nombre);
    while ((getchar()) != '\n');
}
void inicializar_barcos(Jugador*jugador) {
    // Definir los barcos para cada jugador
    jugador->cantidad_barcos = 5;
    jugador->barcos = (Barco *)malloc(jugador->cantidad_barcos * sizeof(Barco));
    jugador->barcos[0] = (Barco){5, "Portaaviones", 1,NULL}; // 1 disponible
    jugador->barcos[1] = (Barco){4, "Acorazado", 1,NULL};    // 1 disponible
    jugador->barcos[2] = (Barco){3, "Crucero", 1,NULL};// 1 disponibles
    jugador->barcos[3] = (Barco){3, "Crucero", 1,NULL};// 1 disponibles
    jugador->barcos[4] = (Barco){2, "Destructor", 1,NULL};   // 1 disponible
    // Inicializar barcos_restantes para cada jugador
    jugador->barcos_restantes = 0;
    for (int i = 0; i < jugador->cantidad_barcos; i++) {
        jugador->barcos_restantes += jugador->barcos[i].tamaño * jugador->barcos[i].cantidad;
    }
    for (int i = 0; i < jugador->cantidad_barcos; i++) {
        jugador->barcos[i].posiciones = (char **)malloc(jugador->barcos[i].tamaño * sizeof(char *));
    }
}

void asignar_memoria_tableros(Jugador *jugador1, Jugador *jugador2) {
    jugador1->tablero = (char **)malloc(TAMANO_TABLERO * sizeof(char *));
    jugador2->tablero = (char **)malloc(TAMANO_TABLERO * sizeof(char *));

    for (int i = 0; i < TAMANO_TABLERO; i++) {
        jugador1->tablero[i] = (char *)malloc(TAMANO_TABLERO * sizeof(char));
        jugador2->tablero[i] = (char *)malloc(TAMANO_TABLERO * sizeof(char));
    }
}

void inicializar_tablero(char **tablero) {
    for (int i = 0; i < TAMANO_TABLERO; i++) {
        for (int j = 0; j < TAMANO_TABLERO; j++) {
            tablero[i][j] = AGUA;
        }
    }
}

void mostrar_tablero_barcos(char **tablero, bool mostrar_barcos, char nombre[50]) {
    printf("BARCOS DE: %s\n", nombre);
    printf("  ");
    for (int i = 0; i < TAMANO_TABLERO; i++) {
        printf("%d ", i);
    }
    printf("\n");

    for (int i = 0; i < TAMANO_TABLERO; i++) {
        printf("%d ", i);
        for (int j = 0; j < TAMANO_TABLERO; j++) {
            if (tablero[i][j] == BARCO && !mostrar_barcos) {
                printf("%c ", AGUA);
            } else {
                printf("%c ", tablero[i][j]);
            }
        }
        printf("\n");
    }
}

void mostrar_tablero_ataques(char **tablero, bool mostrar_barcos, char nombre[50]) {
    printf("TABLERO DE ATAQUES DE: %s\n", nombre);
    printf("  ");
    for (int i = 0; i < TAMANO_TABLERO; i++) {
        printf("%d ", i);
    }
    printf("\n");

    for (int i = 0; i < TAMANO_TABLERO; i++) {
        printf("%d ", i);
        for (int j = 0; j < TAMANO_TABLERO; j++) {
            if (tablero[i][j] == BARCO && !mostrar_barcos) {
                printf("%c ", AGUA);
            } else {
                printf("%c ", tablero[i][j]);
            }
        }
        printf("\n");
    }
}

bool es_posicion_valida(char **tablero, int fila, int columna, int tamaño, char orientacion) {
    if (orientacion == 'H') {
        if (columna + tamaño > TAMANO_TABLERO) return false;
        for (int i = 0; i < tamaño; i++) {
            if (tablero[fila][columna + i] != AGUA) return false;
        }
    } else if (orientacion == 'V') {
        if (fila + tamaño > TAMANO_TABLERO) return false;
        for (int i = 0; i < tamaño; i++) {
            if (tablero[fila + i][columna] != AGUA) return false;
        }
    }
    else {

    }
    return true;
}

void colocar_barco_en_tablero(char **tablero, int fila, int columna, int tamaño, char orientacion,char**posiciones) {
    if (orientacion == 'H') {
        for (int i = 0; i < tamaño; i++) {
            tablero[fila][columna + i] = BARCO;
            posiciones[i] = &tablero[fila][columna + i];
        }
    } else if (orientacion == 'V') {
        for (int i = 0; i < tamaño; i++) {
            tablero[fila + i][columna] = BARCO;
            posiciones[i]=&tablero[fila + i][columna];
        }
    }
}
void seleccionar_barcos(Jugador*jugador) {
    int tipo_barco=0;
    int barcos_colocados = 0;
    int total_barcos = 0;
    // Calcular el total de barcos disponibles
    for (int i = 0; i < jugador->cantidad_barcos; i++) {
        total_barcos += jugador->barcos[i].cantidad;
    }
    // Colocar los barcos hasta que todos estén colocados
    while (barcos_colocados < total_barcos) {
        char desicion_tipo_barco;
        printf("\nTipos de barcos disponibles:\n");
        for (int i = 0; i < jugador->cantidad_barcos; i++) {
            printf("%d. %s (Tama%co: %d) - Disponibles: %d\n", i + 1, jugador->barcos[i].nombre,164 ,jugador->barcos[i].tamaño, jugador->barcos[i].cantidad);
        }
        printf("Seleccione el tipo de barco que desea colocar (1-%d):", jugador->cantidad_barcos);
        scanf(" %c", &desicion_tipo_barco);
        while ((getchar()) != '\n');
        if(isdigit(desicion_tipo_barco)) {
            tipo_barco = desicion_tipo_barco - '0';
            tipo_barco--;
            if (tipo_barco < 0 || tipo_barco >= jugador->cantidad_barcos) {
                printf("Seleccion invalida.Intente nuevamente.\n");
                continue;
            }
            else if(jugador->barcos[tipo_barco].cantidad == 0) {
                printf("No hay mas barcos de este tipo disponibles\n");
                continue;
            }
        }
        else {
            printf("Entrada invalida. Intente nuevamente.\n");
            continue;
        }
        barcos_colocados=colocar_barcos(jugador,tipo_barco,barcos_colocados);
    }
}
char orientar_barcos(char orientacion_num,char orientacion) {
    int desicion_orientacion=0;
    while(orientacion!='H'&& orientacion!='V') {
        printf("Ingrese orientacion (1 para horizontal, 2 para vertical): ");
        scanf("%1c", &orientacion_num);
        while ((getchar()) != '\n');
        if(isdigit(orientacion_num)) {
            desicion_orientacion= orientacion_num - '0';
            switch (desicion_orientacion) {
                case 1:
                    orientacion='H';
                break;
                case 2:
                    orientacion='V';
                break;
                default:
                    printf("opcion invalida.Por favor, ingrese 1 para horizontal o 2 para vertical.\n");
            }
        }
        else {
            printf("Unicamente se reciben numeros, intente de nuevo\n");
        }
    }
    return orientacion;
}
int colocar_barcos(Jugador *jugador,int tipo_barco,int barcos_colocados) {
    char desicion_fila='v';
    char desicion_columna='v';
    int fila=0;
    int columna=0;
    char orientacion_num='0';
    char orientacion='a';
    while(desicion_fila=='v' && desicion_columna=='v' ) {
        printf("Colocando %s (tama%co %d)\n", jugador->barcos[tipo_barco].nombre,164 ,jugador->barcos[tipo_barco].tamaño);
        printf("Ingrese la fila para el barco,solo se tomara el primer numero insertado (0-9):");
        scanf(" %1c", &desicion_fila);
        while ((getchar()) != '\n');
        if(isdigit(desicion_fila)) {
            fila = desicion_fila - '0';
        }
        else {
            printf("Solo se reciben numeros.\n");
            desicion_fila = 'v';
            continue;
        }
        printf("\n");
        printf("Ingrese la columna para el barco, solo se tomara el primer numero insertado (0-9):");
        scanf(" %1c", &desicion_columna);
        while ((getchar()) != '\n');
        if(isdigit(desicion_columna)) {
            columna = desicion_columna - '0';
        }
        else {
            printf("Solo se reciben numeros.\n");
            desicion_columna ='v';
            continue;
        }
        printf("\n");
    }
    orientacion=orientar_barcos(orientacion_num,orientacion);
        if (es_posicion_valida(jugador->tablero, fila, columna, jugador->barcos[tipo_barco].tamaño, orientacion)) {
            colocar_barco_en_tablero(jugador->tablero, fila, columna, jugador->barcos[tipo_barco].tamaño, orientacion,jugador->barcos[tipo_barco].posiciones);
            jugador->barcos[tipo_barco].cantidad--; // Reducir la cantidad disponible
            barcos_colocados++;
            mostrar_tablero_barcos(jugador->tablero, true, jugador->nombre);
        }
        else {
            printf("Posicion invalida o superposicion con otro barco. Intente nuevamente.\n");
        }
    return barcos_colocados;
}
void partida(Jugador *turno_actual, Jugador *oponente, bool *juego_terminado) {
    while (!*juego_terminado) {
        char desicion_fila, desicion_columna;
        mostrar_tablero_ataques(oponente->tablero, false, turno_actual->nombre);
        mostrar_tablero_barcos(turno_actual->tablero, true, turno_actual->nombre);
        printf("%s, ingrese las coordenadas para atacar (fila columna): ", turno_actual->nombre);
        scanf(" %c %c", &desicion_fila, &desicion_columna);
        while ((getchar()) != '\n');
        if (isdigit(desicion_fila) && isdigit(desicion_columna)) {
            int fila = desicion_fila - '0';
            int columna = desicion_columna - '0';

            bool impacto = realizar_ataque(oponente, fila, columna);
            mostrar_vidas_barcos(oponente);
            guardar_resultado_turno("log_partida.txt", turno_actual->nombre, fila, columna, impacto,oponente);

            if (verificar_ganador(oponente)) {
                *juego_terminado = true;
                guardar_resultado_final("resultado_final.txt", turno_actual,oponente);
                printf("\n%s ha ganado el juego!\n", turno_actual->nombre);
            } else {
                // Cambiar de turno
                Jugador *temp = turno_actual;
                turno_actual = oponente;
                oponente = temp;
            }
        } else {
            printf("Entrada invalida. Intente nuevamente.\n");
        }
    }
}

bool realizar_ataque(Jugador *defensor, int fila, int columna) {
    if (fila < 0 || fila >= TAMANO_TABLERO || columna < 0 || columna >= TAMANO_TABLERO) {
        printf("Coordenadas fuera de rango.\n");
        return false;
    }

    if (defensor->tablero[fila][columna] == BARCO) {
        defensor->tablero[fila][columna] = IMPACTO;
        defensor->barcos_restantes--;
        printf("Impacto!\n");
        return true;
    } else if (defensor->tablero[fila][columna] == AGUA) {
        defensor->tablero[fila][columna] = FALLO;
        printf("Fallo.\n");
        return false;
    } else {
        printf("Esta posicion ya fue atacada.\n");
        return false;
    }
}
void mostrar_vidas_barcos(Jugador *vidas) {
    printf("Estado de los barcos de:%s\n",vidas->nombre);
    for (int i = 0; i < 5; i++) {
        const int tamaño=vidas->barcos[i].tamaño;
        int impactos=0;
        for (int j = 0; j <tamaño ; j++) {
            if(*vidas->barcos[i].posiciones[j]==IMPACTO) {
                impactos++;
            }
        }
        vidas->barcos[i].vidas=tamaño-impactos;
        printf("Vidas de %s:%d\n",vidas->barcos[i].nombre,vidas->barcos[i].vidas);
    }
}
bool verificar_ganador(const Jugador *jugador) {
    return jugador->barcos_restantes == 0;
}

void guardar_configuracion_inicial(Jugador *jugador1,Jugador *jugador2) {
    FILE *archivo = fopen("configuracion.txt", "w");
    if (!archivo) {
        perror("Error al abrir el archivo de configuración");
        return;
    }
    fprintf(archivo, "Jugador: %s\n", jugador1->nombre);
        for (int j = 0; j < TAMANO_TABLERO; j++) {
            for (int k = 0; k < TAMANO_TABLERO; k++) {
                fprintf(archivo, "%c ", jugador1->tablero[j][k]);
            }
            fprintf(archivo, "\n");
        }
    fprintf(archivo, "\n");
    fprintf(archivo, "Jugador: %s\n", jugador2->nombre);
    for (int j = 0; j < TAMANO_TABLERO; j++) {
        for (int k = 0; k < TAMANO_TABLERO; k++) {
            fprintf(archivo, "%c ", jugador2->tablero[j][k]);
        }
        fprintf(archivo, "\n");
    }
    fprintf(archivo, "\n");
    fclose(archivo);
}

void guardar_resultado_turno(const char *archivo, const char *jugador, int fila, int columna, bool impacto,Jugador *oponente) {
    FILE *log = fopen(archivo, "a");
    if (!log) {
        perror("Error al abrir el archivo de registro");
        return;
    }
    fprintf(log, "Turno: %s\n", jugador);
    fprintf(log, "Ataque: (%d, %d)\n", fila, columna);
    fprintf(log, "Resultado: %s\n\n", impacto ? "Impacto" : "Fallo");
    fprintf(log, "Barcos de %s:\n", oponente->nombre);
    for (int i = 0; i < 5; i++) {
        fprintf(log,"vidas de %s:%d\n",oponente->barcos[i].nombre,oponente->barcos[i].vidas);
    }
    fclose(log);
}

void guardar_resultado_final(const char *archivo, Jugador *ganador,Jugador *defensor) {
    FILE *resultado = fopen(archivo, "w");
    if (!resultado) {
        perror("Error al abrir el archivo de resultado final");
        return;
    }

    fprintf(resultado, "Ganador: %s\n", ganador->nombre);
    fprintf(resultado, "Estado final del tablero:\n");
    for (int i = 0; i < TAMANO_TABLERO; i++) {
        for (int j = 0; j < TAMANO_TABLERO; j++) {
            fprintf(resultado, "%c ", ganador->tablero[i][j]);
        }
        fprintf(resultado, "\n");
    }
    for (int i = 0; i < 5; i++) {
        fprintf(resultado,"vidas de %s:%d\n",ganador->barcos[i].nombre,ganador->barcos[i].vidas);
    }
    fprintf(resultado,"\n");
    fprintf(resultado, "Perdedor: %s\n", defensor->nombre);
    fprintf(resultado, "Estado final del tablero:\n");
    for (int i = 0; i < TAMANO_TABLERO; i++) {
        for (int j = 0; j < TAMANO_TABLERO; j++) {
            fprintf(resultado, "%c ", defensor->tablero[i][j]);
        }
        fprintf(resultado, "\n");
    }
    for (int i = 0; i < 5; i++) {
        fprintf(resultado,"vidas de %s:%d\n",defensor->barcos[i].nombre,defensor->barcos[i].vidas);
    }
    fclose(resultado);
}
void liberar_memoria(const Jugador *jugador) {
    for (int i = 0; i < TAMANO_TABLERO; i++) {
        free(jugador->tablero[i]);
    }
    free(jugador->tablero);
    for (int i = 0; i < 5; i++) {
        int tamaño=jugador->barcos[i].tamaño;
        for (int j = 0; j <tamaño ; j++) {
            free(jugador->barcos[i].posiciones[j]);
        }
    }
    free(jugador->barcos);
}