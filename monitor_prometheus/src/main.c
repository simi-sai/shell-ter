/**
 * @def SLEEP_TIME
 * @brief Tiempo de espera entre actualizaciones de métricas, en segundos.
 */

#include "expose_metrics.h"
#include <signal.h>
#include <stdbool.h>

#define SLEEP_TIME 1
volatile sig_atomic_t keep_running = 1;

/**
 * @brief Función principal que ejecuta el programa.
 *
 * @param argc Número de argumentos de la línea de comandos.
 * @param argv Argumentos de la línea de comandos.
 * @return int Código de retorno del programa.
 */
int main(void)
{
    // Creamos un hilo para exponer las métricas vía HTTP
    pthread_t tid;
    if (pthread_create(&tid, NULL, expose_metrics, NULL) != 0)
    {
        fprintf(stderr, "Error al crear el hilo del servidor HTTP\n");
        return EXIT_FAILURE;
    }

    init_metrics(); // Inicializa las métricas

    // Bucle principal para actualizar las métricas cada segundo
    while (keep_running)
    {
        // update_cpu_gauge();
        // update_memory_gauge();
        // update_disk_io_gauge();
        // update_network_usage_gauge();
        // update_process_count_gauge();
        // update_context_switches_gauge();
        send_metrics_as_json();

        sleep(SLEEP_TIME);
    }

    return EXIT_SUCCESS;
}
