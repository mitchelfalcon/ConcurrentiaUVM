# Proyecto Integrador: Programación Concurrente con MPI
## Universidad del Valle de México

### 📚 Etapa 1: Elementos y funcionalidades de la programación concurrente

---

## 🎯 Objetivo

Aplicar modelos y técnicas de programación concurrente utilizando **MPI (Message Passing Interface)** con MPICH para la solución de problemas clásicos mediante arquitecturas cliente-servidor y maestro-esclavo.

> **⚠️ Nota Importante:** Este proyecto utiliza MPI en lugar de PVM, ya que PVM no está disponible en Kali Linux. MPI es el estándar moderno de la industria para computación paralela distribuida y cumple los mismos objetivos educativos.

---

## 📋 Contenido del Repositorio


ConcurrentiaUVM/
├── README.md                    # Este archivo
├── etapa1/
│   ├── programaA/              # Comunicación Punto a Punto
│   │   ├── mpi_hello.c         # Código de comunicación cliente-servidor
│   │   ├── Makefile            # Para compilación automatizada
│   │   └── capturas/           # Screenshots de ejecución
│   │       ├── 01_instalacion.png
│   │       ├── 02_config_mpi.png
│   │       ├── 03_compilacion.png
│   │       └── 04_ejecucion.png
│   ├── programaB/              # Modelo Maestro/Trabajadores
│   │   ├── mpi_sum.c           # Código de suma paralela de rangos
│   │   ├── Makefile
│   │   └── capturas/
│   │       ├── 05_compilacion.png
│   │       ├── 06_ejecucion_pequeña.png
│   │       └── 07_ejecucion_grande.png
│   └── documentacion/
│       ├── Proyecto_Integrador_Etapa1.pdf
│       └── Referencias_APA.txt
├── .gitignore
└── INSTALL.md                  # Guía de instalación detallada

---

## 💻 Código Fuente del Proyecto

A continuación se presentan los códigos fuente necesarios para la compilación manual.

### 1. Código: `mpi_hello.c` (Programa A)
Este código implementa la lógica Cliente-Servidor donde el Rank 0 actúa como servidor.

**************************************************************************************
#include <mpi.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
    int rank, size;
    char message[100];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        // --- CÓDIGO DEL SERVIDOR ---
        printf("=== SERVIDOR (Rank 0) ===\n");
        printf("Esperando mensajes de %d clientes...\n", size - 1);
        
        for (int i = 1; i < size; i++) {
            MPI_Recv(message, 100, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
            printf("Mensaje recibido del cliente %d: %s\n", i, message);
        }
        printf("Servidor finalizado.\n");
    } else {
        // --- CÓDIGO DEL CLIENTE ---
        printf("[Cliente %d] Enviando mensaje al servidor...\n", rank);
        sprintf(message, "Hola desde el cliente %d!", rank);
        MPI_Send(message, strlen(message) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
****************************************************************************


****************************************************************************
2. Código: mpi_sum.c (Programa B)
Este código implementa el patrón Maestro-Trabajadores para sumar rangos de números.
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int rank, size;
    long long start, end, local_sum = 0, total_sum = 0;
    long long global_start, global_end;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 3) {
        if (rank == 0) printf("Uso: mpirun -np <N> %s <inicio> <fin>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    global_start = atoll(argv[1]);
    global_end = atoll(argv[2]);

    // Calcular sub-rangos
    long long range_size = (global_end - global_start + 1) / (size - 1);
    
    if (rank == 0) {
        printf("=== MAESTRO (Rank 0) INICIADO ===\n");
        printf("Rango total: [%lld, %lld]\n", global_start, global_end);
        printf("Número de trabajadores: %d\n", size - 1);

        for (int i = 1; i < size; i++) {
            long long r_start = global_start + (i - 1) * range_size;
            long long r_end = (i == size - 1) ? global_end : (r_start + range_size - 1);
            
            MPI_Send(&r_start, 1, MPI_LONG_LONG, i, 0, MPI_COMM_WORLD);
            MPI_Send(&r_end, 1, MPI_LONG_LONG, i, 0, MPI_COMM_WORLD);
            printf("Enviado a Trabajador %d: rango [%lld, %lld]\n", i, r_start, r_end);
        }

        printf("\n=== RECIBIENDO RESULTADOS ===\n");
        for (int i = 1; i < size; i++) {
            long long worker_sum;
            MPI_Recv(&worker_sum, 1, MPI_LONG_LONG, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Resultado Trabajador %d: %lld\n", i, worker_sum);
            total_sum += worker_sum;
        }
        printf("\n=== RESULTADO FINAL ===\n");
        printf("Suma total del rango [%lld, %lld] = %lld\n", global_start, global_end, total_sum);

    } else {
        MPI_Recv(&start, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&end, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("[Trabajador %d] Procesando rango [%lld, %lld]\n", rank, start, end);
        for (long long i = start; i <= end; i++) {
            local_sum += i;
        }
        printf("[Trabajador %d] Suma calculada: %lld\n", rank, local_sum);
        MPI_Send(&local_sum, 1, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}

**************************************************************************************

🔧 Requisitos del Sistema
Software Necesario
 * Sistema Operativo: Kali Linux 2024+ / Ubuntu 22.04+ / Debian 12+
 * MPI: MPICH 4.0+ o OpenMPI 4.1+
 * Compilador: GCC 11.0+
 * Git: Para control de versiones
Instalación Rápida en Kali Linux
# Actualizar sistema
sudo apt update && sudo apt upgrade -y

# Instalar MPICH
sudo apt install mpich -y

# Instalar herramientas de desarrollo
sudo apt install build-essential gcc git -y

# Verificar instalación
which mpicc
mpicc --version
mpirun --version

Alternativa: Usar OpenMPI
# Si prefieres OpenMPI en lugar de MPICH
sudo apt install openmpi-bin openmpi-common libopenmpi-dev -y

🛠️ Guía de Construcción desde Cero (Terminal)
Si deseas crear los archivos, compilar y generar el entregable manualmente, sigue estos pasos:
1. Crear Estructura de Carpetas
mkdir -p ~/ConcurrentiaUVM/etapa1/programaA
mkdir -p ~/ConcurrentiaUVM/etapa1/programaB
mkdir -p ~/ConcurrentiaUVM/etapa1/capturas
mkdir -p ~/ConcurrentiaUVM/documentacion
cd ~/ConcurrentiaUVM

2. Crear Archivos con Nano
Copia el código fuente provisto arriba y pégalo en el editor:
Para Programa A:
nano etapa1/programaA/mpi_hello.c
# Pega el código de mpi_hello.c
# Presiona Ctrl+O (Guardar), Enter, Ctrl+X (Salir)

Para Programa B:
nano etapa1/programaB/mpi_sum.c
# Pega el código de mpi_sum.c
# Presiona Ctrl+O (Guardar), Enter, Ctrl+X (Salir)

3. Generar el ZIP Entregable
Para comprimir todo el proyecto desde la terminal:
# Instalar utilidad zip
sudo apt install zip -y

# Crear el archivo comprimido
cd ~/ConcurrentiaUVM
zip -r Nailea_Falcon_Etapa1.zip .

⚙️ Automatización y Scripts (Opcional)
Este repositorio incluye herramientas para automatizar la compilación y entrega.
Opción A: Script Automático compilar_y_guardar.sh
Este script compila ambos programas, verifica errores y mueve los ejecutables a la carpeta Downloads.
chmod +x compilar_y_guardar.sh
./compilar_y_guardar.sh

Opción B: Uso de Makefile
Si prefieres usar make, los siguientes comandos están disponibles:
make downloads      # Compila y copia todo a ~/Downloads/mpi_proyecto
make downloads-bin  # Solo ejecutables
make zip            # Crea ZIP en Downloads

🚀 Compilación y Ejecución
Programa A: Comunicación Punto a Punto
cd etapa1/programaA/

# Compilar
mpicc mpi_hello.c -o mpi_hello

# Ejecutar con 4 procesos (1 servidor + 3 clientes)
mpirun -np 4 ./mpi_hello

Programa B: Maestro/Trabajadores (Suma de Rangos)
cd etapa1/programaB/

# Compilar
mpicc mpi_sum.c -o mpi_sum

# Ejecutar suma del 1 al 1000 con 4 procesos
mpirun -np 4 ./mpi_sum 1 1000

# Ejemplo con 8 procesos para mejor paralelismo
mpirun -np 8 ./mpi_sum 1 10000

📊 Resultados Esperados
Programa A (Cliente/Servidor)
Comando: mpirun -np 4 ./mpi_hello
Salida Esperada:
=== SERVIDOR (Rank 0) ===
Esperando mensajes de 3 clientes...
[Cliente 1] Enviando mensaje al servidor...
Mensaje recibido del cliente 1: Hola desde el cliente 1!
[Cliente 2] Enviando mensaje al servidor...
Mensaje recibido del cliente 2: Hola desde el cliente 2!
[Cliente 3] Enviando mensaje al servidor...
Mensaje recibido del cliente 3: Hola desde el cliente 3!
Servidor finalizado.

Programa B (Maestro/Trabajadores)
Comando: mpirun -np 4 ./mpi_sum 1 1000
Salida Esperada:
=== MAESTRO (Rank 0) INICIADO ===
Rango total: [1, 1000]
Número de trabajadores: 3
Enviado a Trabajador 1: rango [1, 333]
Enviado a Trabajador 2: rango [334, 666]
Enviado a Trabajador 3: rango [667, 1000]

[Trabajador 1] Procesando rango [1, 333]
[Trabajador 1] Suma calculada: 55611
[Trabajador 2] Procesando rango [334, 666]
[Trabajador 2] Suma calculada: 166500
[Trabajador 3] Procesando rango [667, 1000]
[Trabajador 3] Suma calculada: 278389

=== RECIBIENDO RESULTADOS ===
Resultado Trabajador 1: 55611
Resultado Trabajador 2: 166500
Resultado Trabajador 3: 278389

=== RESULTADO FINAL ===
Suma total del rango [1, 1000] = 500500

🧪 Casos de Prueba
| Comando | Resultado Esperado | Fórmula | Verificación |
|---|---|---|---|
| mpirun -np 3 ./mpi_sum 1 10 | 55 | n(n+1)/2 | ✅ |
| mpirun -np 4 ./mpi_sum 1 100 | 5,050 | 100(101)/2 | ✅ |
| mpirun -np 5 ./mpi_sum 1 1000 | 500,500 | 1000(1001)/2 | ✅ |
| mpirun -np 4 ./mpi_sum 50 150 | 10,100 | Σ(50→150) | ✅ |
📸 Capturas de Pantalla Requeridas
Instalación y Configuración
 * Instalación MPI: Salida de mpicc --version y mpirun --version
 * Configuración: Ejecución del programa de prueba hello_mpi con 4 procesos
Programa A
 * Compilación: Comando mpicc compilando mpi_hello.c sin errores
 * Ejecución: Salida completa mostrando servidor y clientes comunicándose
Programa B
 * Compilación: Comando mpicc compilando mpi_sum.c sin errores
 * Ejecución Pequeña: Suma del 1 al 100
 * Ejecución Grande: Suma del 1 al 1000 mostrando distribución de trabajo
🔍 Solución de Problemas
Error: "mpicc: command not found"
# Reinstalar MPICH
sudo apt install --reinstall mpich

Error al ejecutar: "mpirun not found"
# Verificar PATH
echo $PATH
# Añadir al PATH si es necesario
export PATH=$PATH:/usr/bin

Error: "Segmentation fault"
# Asegúrate de:
# 1. Llamar MPI_Init antes que cualquier función MPI
# 2. Llamar MPI_Finalize al final
# 3. Verificar que los buffers tengan tamaño suficiente

Problemas de rendimiento
# Ejecutar con más procesos
mpirun -np 8 ./mpi_sum 1 10000

# Ver uso de CPU
top
# Presiona '1' para ver todos los cores

🆚 Comparación: PVM vs MPI
| Característica | PVM (Original) | MPI (Implementado) |
|---|---|---|
| Disponibilidad | ❌ No en Kali | ✅ Nativo |
| Estándar | Obsoleto | ✅ Estándar IEEE |
| Rendimiento | Moderado | ✅ Alto |
| Sintaxis | Propietaria | Estándar POSIX |
| Soporte | Limitado | ✅ Activo |
Conclusión: MPI es superior en todos los aspectos y es el estándar de la industria actual.


📚 Referencias Técnicas
 * Gropp, W., Lusk, E., & Skjellum, A. (2014). Using MPI: Portable Parallel Programming with the Message-Passing Interface (3rd ed.). MIT Press.
 * Pacheco, P. (2011). An Introduction to Parallel Programming. Morgan Kaufmann.
 * MPI Forum. (2021). MPI: A Message-Passing Interface Standard Version 4.0. Recuperado de https://www.mpi-forum.org/
 * MPICH Documentation. (2024). MPICH User's Guide. Argonne National Laboratory. https://www.mpich.org/documentation/
 * Tanenbaum, A. S. (2015). Modern Operating Systems (4th ed.). Pearson.

   
👥 Equipo de Desarrollo
 * Integrante 1: NAILEA MITCHEL FALCON TRIANA

   
📝 Licencia
Este proyecto es material académico para la asignatura de Programación Concurrente de la Universidad del Valle de México (UVM).
📞 Contacto
Para dudas o consultas sobre el proyecto, contactar a través de la plataforma educativa UVM o el repositorio de GitHub:
Repositorio: https://github.com/mitchelfalcon/ConcurrentiaUVM.git


Última actualización: Febrero 2025
Versión: 2.0.0 - Migrado de PVM a MPI (MPICH)
Estado: ✅ Etapa 1 Completada

<div align="center">
<p>Creado con 💓 para ayudar a estudiantes a aprender programación</p>
</div> 
