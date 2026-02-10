#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    if (argc != 3) {
        if (world_rank == 0) {
            printf("Uso: mpirun -np <N> %s <inicio> <fin>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }
    
    int inicio_total = atoi(argv[1]);
    int fin_total = atoi(argv[2]);
    
    if (world_rank == 0) {
        // PROCESO MAESTRO
        printf("=== MAESTRO (Rank 0) INICIADO ===\n");
        printf("Rango total: [%d, %d]\n", inicio_total, fin_total);
        printf("Número de trabajadores: %d\n", world_size - 1);
        
        int rango_por_trabajador = (fin_total - inicio_total + 1) / (world_size - 1);
        
        // Enviar trabajo a cada trabajador
        for (int i = 1; i < world_size; i++) {
            int inicio = inicio_total + (i - 1) * rango_por_trabajador;
            int fin = (i == world_size - 1) ? fin_total : inicio + rango_por_trabajador - 1;
            
            int datos[2] = {inicio, fin};
            MPI_Send(datos, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
            printf("Enviado a Trabajador %d: rango [%d, %d]\n", i, inicio, fin);
        }
        
        // Recibir resultados
        printf("\n=== RECIBIENDO RESULTADOS ===\n");
        long long suma_total = 0;
        
        for (int i = 1; i < world_size; i++) {
            long long suma_parcial;
            MPI_Recv(&suma_parcial, 1, MPI_LONG_LONG, i, 0, 
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Resultado Trabajador %d: %lld\n", i, suma_parcial);
            suma_total += suma_parcial;
        }
        
        printf("\n=== RESULTADO FINAL ===\n");
        printf("Suma total del rango [%d, %d] = %lld\n", 
               inicio_total, fin_total, suma_total);
               
    } else {
        // PROCESOS TRABAJADORES
        int datos[2];
        MPI_Recv(datos, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        int inicio = datos[0];
        int fin = datos[1];
        
        printf("[Trabajador %d] Procesando rango [%d, %d]\n", 
               world_rank, inicio, fin);
        
        // Calcular suma
        long long suma = 0;
        for (long long i = inicio; i <= fin; i++) {
            suma += i;
        }
        
        printf("[Trabajador %d] Suma calculada: %lld\n", world_rank, suma);
        
        // Enviar resultado al maestro
        MPI_Send(&suma, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    }
    
    MPI_Finalize();
    return 0;
}
