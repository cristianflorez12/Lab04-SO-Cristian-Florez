#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Estructura para la transferencia de argumentos al hilo trabajador
typedef struct {
    unsigned long long* array;
    int n;
} fib_data_t;

// Función ejecutada por el hilo trabajador
void* generate_fibonacci(void* arg) {
    fib_data_t* data = (fib_data_t*)arg;
    
    // Casos base de la secuencia
    if (data->n > 0) data->array[0] = 0;
    if (data->n > 1) data->array[1] = 1;
    
    // Cálculo iterativo secuencial dentro del arreglo compartido
    for (int i = 2; i < data->n; i++) {
        data->array[i] = data->array[i - 1] + data->array[i - 2];
    }
    
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <N_elementos>\n", argv[0]);
        return 1;
    }
    
    int N = atoi(argv[1]);
    if (N <= 0) {
        fprintf(stderr, "Error: El numero de elementos debe ser un entero positivo.\n");
        return 1;
    }

    // Asignación de memoria dinámica para el arreglo compartido en el Heap
    unsigned long long* shared_array = malloc(N * sizeof(unsigned long long));
    if (shared_array == NULL) {
        fprintf(stderr, "Error: No se pudo asignar memoria.\n");
        return 1;
    }

    // Preparación del paquete de datos para transferir al hilo trabajador
    fib_data_t thread_data;
    thread_data.array = shared_array;
    thread_data.n = N;

    pthread_t worker_thread;
    
    // Instanciación del hilo trabajador
    if (pthread_create(&worker_thread, NULL, generate_fibonacci, &thread_data) != 0) {
        fprintf(stderr, "Error al crear el hilo trabajador.\n");
        free(shared_array);
        return 1;
    }

    // Sincronización: main se bloquea hasta que el trabajador termine
    pthread_join(worker_thread, NULL);

    // Impresión segura de los resultados desde el arreglo compartido
    printf("Secuencia de Fibonacci generada para N = %d:\n", N);
    for (int i = 0; i < N; i++) {
        printf("%llu ", shared_array[i]);
    }
    printf("\n");

    // Liberación de recursos
    free(shared_array);
    return 0;
}