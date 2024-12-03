/**
 * @file metrics.h
 * @brief Funciones para obtener el uso de CPU y memoria desde el sistema de
 * archivos /proc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief Estructura para almacenar las estadísticas de disco.
 */
struct DiskStats {
    unsigned long long reads;
    unsigned long long writes;
    unsigned long long read_time;
    unsigned long long write_time;
};

/**
 * @brief Estructura para almacenar las estadísticas de red.
 */
struct NetStats {
    unsigned long long bytes_received;
    unsigned long long bytes_transmitted;
    unsigned long long packets_received;
    unsigned long long packets_transmitted;
};

/**
 * @brief Tamaño del buffer utilizado para la recolección de métricas.
 */
#define BUFFER_SIZE 256

/**
 * @brief Obtiene el porcentaje de uso de memoria desde /proc/meminfo.
 *
 * Lee los valores de memoria total y disponible desde /proc/meminfo y calcula
 * el porcentaje de uso de memoria.
 *
 * @return Uso de memoria como porcentaje (0.0 a 100.0), o -1.0 en caso de
 * error.
 */
double get_memory_usage(void);

/**
 * @brief Obtiene las estadísticas de entrada/salida del disco.
 */
struct DiskStats get_disk_io(void);

/**
 * @brief Obtiene las estadísticas de la red.
 */
struct NetStats get_network_stats(void);

/**
 * @brief Obtiene el conteo de procesos en ejecución.
 */
int get_process_count(void);

/**
 * @brief Obtiene los cambios de contexto del sistema.
 */
unsigned long long get_context_switches(void);
/**
 * @brief Obtiene el porcentaje de uso de CPU desde /proc/stat.
 *
 * Lee los tiempos de CPU desde /proc/stat y calcula el porcentaje de uso de CPU
 * en un intervalo de tiempo.
 *
 * @return Uso de CPU como porcentaje (0.0 a 100.0), o -1.0 en caso de error.
 */
double get_cpu_usage(void);
