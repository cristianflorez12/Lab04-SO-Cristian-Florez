#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Función para medir el tiempo con alta precisión en segundos
double GetTime() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

// Función matemática a integrar
double f(double x) {
    return 4.0 / (1.0 + x * x);
}

// Núcleo computacional serial
double CalcPi(int n) {
    const double fH = 1.0 / (double)n;
    double fSum = 0.0;
    double fX;
    int i;
    
    for (i = 0; i < n; i += 1) {
        fX = fH * ((double)i + 0.5);
        fSum += f(fX);
    }
    return fH * fSum;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <num_rectangulos>\n", argv[0]);
        return 1;
    }
    
    int n = atoi(argv[1]);
    
    // Instrumentación para medir exclusivamente la función CalcPi
    double start = GetTime();
    double pi = CalcPi(n);
    double end = GetTime();
    
    printf("Pi calculado (Serial): %.15f\n", pi);
    printf("Tiempo de ejecucion: %f segundos\n", end - start);
    
    return 0;
}