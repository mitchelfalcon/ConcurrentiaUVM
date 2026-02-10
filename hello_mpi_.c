#include <mpi.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    if (world_rank == 0) {
        // Proceso 0 actúa como "servidor"
        printf("=== SERVIDOR (Rank 0) ===\n");
        printf("Esperando mensajes de %d clientes...\n", world_size - 1);
        
        for (int i = 1; i < world_size; i++) {
            char message[100];
            MPI_Recv(message, 100, MPI_CHAR, i, 0, 
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Mensaje recibido del cliente %d: %s\n", i, message);
        }
        
        printf("Servidor finalizado.\n");
    } else {
        // Procesos 1-N actúan como "clientes"
        char message[100];
        sprintf(message, "Hola desde el cliente %d!", world_rank);
        
        printf("[Cliente %d] Enviando mensaje al servidor...\n", world_rank);
        MPI_Send(message, strlen(message) + 1, MPI_CHAR, 0, 0, 
                 MPI_COMM_WORLD);
        printf("[Cliente %d] Mensaje enviado.\n", world_rank);
    }
    
    MPI_Finalize();
    return 0;
}
