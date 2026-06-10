#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// Función para medir el tiempo con alta precisión
double GetTime() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

double f(double x) {
    return 4.0 / (1.0 + x * x);
}

// Estructura para pasar los argumentos a cada hilo
typedef struct {
    int thread_id;
    int n;
    int num_threads;
    double fH;
} thread_data_t;

// Función que ejecutará cada hilo trabajador
void* CalcPiPartial(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    int id = data->thread_id;
    int n = data->n;
    int T = data->num_threads;
    double fH = data->fH;

    double fSum = 0.0;
    double fX;

    // Particionamiento del rango total (Data Parallelism)
    int chunk_size = n / T;
    int start_idx = id * chunk_size;
    // El último hilo se encarga del residuo en caso de divisiones inexactas
    int end_idx = (id == T - 1) ? n : (id + 1) * chunk_size;

    for (int i = start_idx; i < end_idx; i++) {
        fX = fH * ((double)i + 0.5);
        fSum += f(fX);
    }

    // Se reserva memoria para retornar de manera segura el valor local sin usar mutex
    double* result = malloc(sizeof(double));
    if (result == NULL) {
        pthread_exit(NULL);
    }
    *result = fSum;
    
    return (void*)result;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <num_rectangulos> <num_hilos>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int T = atoi(argv[2]);

    pthread_t* threads = malloc(T * sizeof(pthread_t));
    thread_data_t* thread_data = malloc(T * sizeof(thread_data_t));
    double fH = 1.0 / (double)n;

    // Medición exclusiva de la creación, ejecución y recolección de los hilos
    double start = GetTime();

    // Creación de los T hilos trabajadores
    for (int i = 0; i < T; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].n = n;
        thread_data[i].num_threads = T;
        thread_data[i].fH = fH;
        pthread_create(&threads[i], NULL, CalcPiPartial, &thread_data[i]);
    }

    double total_sum = 0.0;
    
    // Sincronización y recolección de sumas parciales
    for (int i = 0; i < T; i++) {
        double* partial_sum;
        pthread_join(threads[i], (void**)&partial_sum);
        if (partial_sum != NULL) {
            total_sum += *partial_sum;
            free(partial_sum); // Liberación de la memoria asignada en el hilo
        }
    }

    // Reducción final por el hilo principal
    double pi = fH * total_sum;
    double end = GetTime();

    printf("Pi calculado (Paralelo): %.15f\n", pi);
    printf("Tiempo de ejecucion (%d hilos): %f segundos\n", T, end - start);

    free(threads);
    free(thread_data);
    return 0;
}