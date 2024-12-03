#include "metrics.h"

double get_memory_usage()
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long total_mem = 0, free_mem = 0;

    // Abrir el archivo /proc/meminfo
    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/meminfo");
        return -1.0;
    }

    // Leer los valores de memoria total y disponible
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (sscanf(buffer, "MemTotal: %llu kB", &total_mem) == 1)
        {
            continue; // MemTotal encontrado
        }
        if (sscanf(buffer, "MemAvailable: %llu kB", &free_mem) == 1)
        {
            break; // MemAvailable encontrado, podemos dejar de leer
        }
    }

    fclose(fp);

    // Verificar si se encontraron ambos valores
    if (total_mem == 0 || free_mem == 0)
    {
        fprintf(stderr, "Error al leer la información de memoria desde /proc/meminfo\n");
        return -1.0;
    }

    // Calcular el porcentaje de uso de memoria
    unsigned long long used_mem = total_mem - free_mem;
    unsigned long long mem_usage_percent = used_mem / total_mem * (unsigned long long)100.0;

    return (double)mem_usage_percent;
}

double get_cpu_usage()
{
    static unsigned long long prev_user = 0, prev_nice = 0, prev_system = 0, prev_idle = 0, prev_iowait = 0,
                              prev_irq = 0, prev_softirq = 0, prev_steal = 0;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    unsigned long long totald, idled;
    double cpu_usage_percent;

    // Abrir el archivo /proc/stat
    FILE* fp = fopen("/proc/stat", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/stat");
        return -1.0;
    }

    char buffer[BUFFER_SIZE * 4];
    if (fgets(buffer, sizeof(buffer), fp) == NULL)
    {
        perror("Error al leer /proc/stat");
        fclose(fp);
        return -1.0;
    }
    fclose(fp);

    // Analizar los valores de tiempo de CPU
    int ret = sscanf(buffer, "cpu  %llu %llu %llu %llu %llu %llu %llu %llu", &user, &nice, &system, &idle, &iowait,
                     &irq, &softirq, &steal);
    if (ret < 8)
    {
        fprintf(stderr, "Error al parsear /proc/stat\n");
        return -1.0;
    }

    // Calcular las diferencias entre las lecturas actuales y anteriores
    unsigned long long prev_idle_total = prev_idle + prev_iowait;
    unsigned long long idle_total = idle + iowait;

    unsigned long long prev_non_idle = prev_user + prev_nice + prev_system + prev_irq + prev_softirq + prev_steal;
    unsigned long long non_idle = user + nice + system + irq + softirq + steal;

    unsigned long long prev_total = prev_idle_total + prev_non_idle;
    unsigned long long total = idle_total + non_idle;

    totald = total - prev_total;
    idled = idle_total - prev_idle_total;

    if (totald == 0)
    {
        fprintf(stderr, "Totald es cero, no se puede calcular el uso de CPU!\n");
        return -1.0;
    }

    // Calcular el porcentaje de uso de CPU
    unsigned long long resta = totald - idled;
    double division = (double)resta / (double)totald;
    cpu_usage_percent = division * 100.0;

    // Actualizar los valores anteriores para la siguiente lectura
    prev_user = user;
    prev_nice = nice;
    prev_system = system;
    prev_idle = idle;
    prev_iowait = iowait;
    prev_irq = irq;
    prev_softirq = softirq;
    prev_steal = steal;

    return (double)cpu_usage_percent;
}

// Leer estadísticas de I/O de disco
struct DiskStats get_disk_io()
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long read_sectors = 0, write_sectors = 0, read_time = 0, write_time = 0;
    struct DiskStats disk = {0, 0, 0, 0};

    // Abrir el archivo /proc/diskstats
    fp = fopen("/proc/diskstats", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/diskstats");
        // exit(EXIT_FAILURE);
    }

    // Leer los valores de lecturas y escrituras
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (sscanf(buffer, "%*u %*u nvme0n1p5 %*u %*u %llu %llu %*u %*u %llu %llu", &read_sectors, &read_time,
                   &write_sectors, &write_time) == 4)
        {
            break;
        }
    }

    disk.reads = read_sectors;
    disk.writes = write_sectors;
    disk.read_time = read_time * (unsigned long long)(1e-3);
    disk.write_time = write_time * (unsigned long long)(1e-3);

    fclose(fp);

    return disk;
}

// Leer estadísticas de red
struct NetStats get_network_stats()
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long rx_bytes = 0, tx_bytes = 0, rx_packets = 0, tx_packets = 0;
    struct NetStats net = {0, 0, 0, 0};

    // Abrir el archivo /proc/net/dev
    fp = fopen("/proc/net/dev", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/net/dev");
        // exit(EXIT_FAILURE);
    }

    // Saltar las dos primeras líneas del archivo (encabezados)
    fgets(buffer, sizeof(buffer), fp);
    fgets(buffer, sizeof(buffer), fp);

    // Leer las estadísticas de la interfaz de red
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (sscanf(buffer, "wlp3s0: %llu %llu %*u %*u %*u %*u %*u %*u %llu %llu", &rx_bytes, &rx_packets, &tx_bytes,
                   &tx_packets) == 4)
        {
            break;
        }
    }

    fclose(fp);

    net.bytes_received = rx_bytes;
    net.bytes_transmitted = tx_bytes;
    net.packets_received = rx_packets;
    net.packets_transmitted = tx_packets;

    return net;
}

// Leer el conteo de procesos
int get_process_count()
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long process_count = 0;

    // Abrir el archivo /proc/stat
    fp = fopen("/proc/stat", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/stat");
        return -1;
    }

    // Leer las estadísticas de procesos
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (sscanf(buffer, "procs_running %llu", &process_count) == 1)
        {
            break; // Encontramos el conteo de procesos
        }
    }

    fclose(fp);

    return (int)process_count;
}

// Leer cambios de contexto
unsigned long long get_context_switches()
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long ctxt = 0;

    // Abrir el archivo /proc/stat
    fp = fopen("/proc/stat", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/stat");
        return (unsigned long long)(-1.0);
    }

    // Leer las líneas hasta encontrar la línea que contiene el número de cambios
    // de contexto
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (sscanf(buffer, "ctxt %llu", &ctxt) == 1)
        {
            break; // Encontramos los cambios de contexto
        }
    }

    fclose(fp);

    return ctxt;
}
