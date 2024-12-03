/**
 * @brief Envía las métricas recolectadas como un objeto JSON a través de un
 * pipe.
 */
#include "cjson_metrics.h"

void send_metrics_as_json()
{
    cJSON* root = cJSON_CreateObject();

    // Recolectar las métricas de CPU, memoria, disco, red, procesos y cambios de
    // contexto
    double cpu_usage = get_cpu_usage();
    double memory_usage = get_memory_usage();
    struct DiskStats disk_stats = get_disk_io();
    struct NetStats network_stats = get_network_stats();
    int running_processes = get_process_count();
    unsigned long long context_switches = get_context_switches();

    // Añadir métricas al objeto JSON
    cJSON_AddNumberToObject(root, "cpu_usage_percentage", (double)cpu_usage);
    cJSON_AddNumberToObject(root, "memory_usage_percentage", (double)memory_usage);
    cJSON_AddNumberToObject(root, "disk_reads", (double)disk_stats.reads);
    cJSON_AddNumberToObject(root, "disk_writes", (double)disk_stats.writes);
    cJSON_AddNumberToObject(root, "disk_read_time_seconds", (double)disk_stats.read_time);
    cJSON_AddNumberToObject(root, "disk_write_time_seconds", (double)disk_stats.write_time);
    cJSON_AddNumberToObject(root, "network_bandwidth_rx", (double)network_stats.bytes_received);
    cJSON_AddNumberToObject(root, "network_bandwidth_tx", (double)network_stats.bytes_transmitted);
    cJSON_AddNumberToObject(root, "network_packet_ratio",
                            (double)(network_stats.packets_received > 0)
                                ? (double)network_stats.packets_transmitted / (double)network_stats.packets_received
                                : 0.0);
    cJSON_AddNumberToObject(root, "running_processes_count", (double)running_processes);
    cJSON_AddNumberToObject(root, "context_switches_total", (double)context_switches);

    // Convertir el objeto JSON a string y enviarlo al pipe
    char* json_data = cJSON_Print(root);

    if (json_data != NULL)
    {
        FILE* pipe = fopen("/tmp/monitor_pipe",
                           "a"); // "a" mantiene el pipe abierto para múltiples escrituras
        if (pipe)
        {
            fprintf(pipe, "%s\n", json_data);
            fflush(pipe); // Asegura que los datos se escriban inmediatamente
            fclose(pipe);
        }
        else
        {
            perror("Error al abrir el pipe para enviar métricas");
        }

        free(json_data);
    }

    // Limpiar el objeto JSON
    cJSON_Delete(root);
}