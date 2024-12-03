#include "expose_metrics.h"

/** Mutex para sincronización de hilos */
pthread_mutex_t lock;

/** Métrica de Prometheus para el uso de CPU */
static prom_gauge_t* cpu_usage_metric;

/** Métrica de Prometheus para el uso de memoria */
static prom_gauge_t* memory_usage_metric;

/** Nuevas métricas de Prometheus */
static prom_gauge_t* disk_reads_metric;
static prom_gauge_t* disk_writes_metric;
static prom_gauge_t* disk_read_time_metric;
static prom_gauge_t* disk_write_time_metric;

static prom_gauge_t* network_transfers_metric;
static prom_gauge_t* network_reception_metric;
static prom_gauge_t* network_packet_tx_metric;
static prom_gauge_t* network_packet_rx_metric;

static prom_gauge_t* process_count_metric;
static prom_gauge_t* context_switches_metric;

void update_cpu_gauge()
{
    double usage = get_cpu_usage();
    if (usage >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(cpu_usage_metric, usage, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener el uso de CPU\n");
    }
}

void update_memory_gauge()
{
    double usage = get_memory_usage();
    if (usage >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(memory_usage_metric, usage, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener el uso de memoria\n");
    }
}

// Nueva función para actualizar la métrica de I/O de disco
void update_disk_io_gauge()
{
    struct DiskStats io = get_disk_io();

    pthread_mutex_lock(&lock);
    prom_gauge_set(disk_reads_metric, (double)io.reads, NULL);
    prom_gauge_set(disk_writes_metric, (double)io.writes, NULL);
    prom_gauge_set(disk_read_time_metric, (double)io.read_time, NULL);
    prom_gauge_set(disk_write_time_metric, (double)io.write_time, NULL);
    pthread_mutex_unlock(&lock);
}

// Nueva función para actualizar la métrica de uso de red
void update_network_usage_gauge()
{
    struct NetStats net = get_network_stats();

    pthread_mutex_lock(&lock);
    prom_gauge_set(network_transfers_metric, (double)net.bytes_transmitted, NULL);
    prom_gauge_set(network_reception_metric, (double)net.bytes_received, NULL);
    prom_gauge_set(network_packet_tx_metric, (double)net.packets_transmitted, NULL);
    prom_gauge_set(network_packet_rx_metric, (double)net.packets_received, NULL);
    pthread_mutex_unlock(&lock);
}

// Nueva función para actualizar el conteo de procesos
void update_process_count_gauge()
{
    int process_count = get_process_count();
    if (process_count >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(process_count_metric, (double)process_count, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener el número de procesos\n");
    }
}

// Nueva función para actualizar la métrica de cambios de contexto
void update_context_switches_gauge()
{
    int ctxt_switches = (int)get_context_switches();
    if (ctxt_switches >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(context_switches_metric, ctxt_switches, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener cambios de contexto\n");
    }
}

void* expose_metrics(void* arg)
{
    (void)arg; // Argumento no utilizado

    // Aseguramos que el manejador HTTP esté adjunto al registro por defecto
    promhttp_set_active_collector_registry(NULL);

    // Iniciamos el servidor HTTP en el puerto 8000
    struct MHD_Daemon* daemon = promhttp_start_daemon(MHD_USE_SELECT_INTERNALLY, 8000, NULL, NULL);
    if (daemon == NULL)
    {
        fprintf(stderr, "Error al iniciar el servidor HTTP\n");
        return NULL;
    }

    // Mantenemos el servidor en ejecución
    while (1)
    {
        sleep(1);
    }

    // Nunca debería llegar aquí
    MHD_stop_daemon(daemon);
    return NULL;
}

void init_metrics()
{
    // Inicializamos el mutex
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        fprintf(stderr, "Error al inicializar el mutex\n");
        return;
    }

    // Inicializamos el registro de coleccionistas de Prometheus
    if (prom_collector_registry_default_init() != 0)
    {
        fprintf(stderr, "Error al inicializar el registro de Prometheus\n");
        return;
    }

    memory_usage_metric = prom_collector_registry_must_register_metric(
        prom_gauge_new("memory_usage_percentage", "Porcentaje de uso de memoria", 0, NULL));
    cpu_usage_metric = prom_collector_registry_must_register_metric(
        prom_gauge_new("cpu_usage_percentage", "Porcentaje de uso de CPU", 0, NULL));

    disk_read_time_metric = prom_collector_registry_must_register_metric(
        prom_gauge_new("disk_time_read", "Estadisticas de tiempo de Lectura de Disco", 0, NULL));
    disk_write_time_metric = prom_collector_registry_must_register_metric(
        prom_gauge_new("disk_time_write", "Estadisticas de tiempo de Escritura de Disco", 0, NULL));
    disk_reads_metric = prom_collector_registry_must_register_metric(
        prom_gauge_new("disk_read", "Estadisticas de Lectura de Disco", 0, NULL));
    disk_writes_metric = prom_collector_registry_must_register_metric(
        prom_gauge_new("disk_write", "Estadisticas de Escritura de Disco", 0, NULL));
    network_transfers_metric = prom_collector_registry_must_register_metric(
        prom_gauge_new("network_usage_transmission", "Estadisticas de Envio de Red [en GB]", 0, NULL));
    network_reception_metric = prom_collector_registry_must_register_metric(
        prom_gauge_new("network_usage_reception", "Estadisticas de Recepcion de Red [en GB]", 0, NULL));
    network_packet_tx_metric = prom_collector_registry_must_register_metric(
        prom_gauge_new("network_usage_packages_tx", "Estadisticas de Paquetes Enviados de Red", 0, NULL));
    network_packet_rx_metric = prom_collector_registry_must_register_metric(
        prom_gauge_new("network_usage_packages_rx", "Estadisticas de Paquetes Recibidos de Red", 0, NULL));

    process_count_metric = prom_collector_registry_must_register_metric(
        prom_gauge_new("process_count", "Numero de procesos corriendo", 0, NULL));
    context_switches_metric = prom_collector_registry_must_register_metric(
        prom_gauge_new("context_switches", "Cambios de contexto", 0, NULL));
}

void destroy_mutex()
{
    pthread_mutex_destroy(&lock);
}
