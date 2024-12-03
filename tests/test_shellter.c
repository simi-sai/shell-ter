/**
 * @file test_shellter.c
 * @brief Pruebas unitarias para el shell
 *
 * Para compilar y ejecutar las pruebas:
 * gcc tests/test_shell.c shell.c commands.c monitor.c signal_handlers.c -o test_shell -lcmocka
 * ./test_shell
 */
#include "commands.h"
#include "manager.h"
#include "monitor.h"
#include "prompt.h"
#include "signals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <unity/unity.h>

/**
 * @brief Variable global para indicar si el shell está en ejecución
 */
extern pid_t monitor_pid;

#define MONITOR_OFF -1
// Prototipos de funciones

/**
 * @brief Prueba la función echo
 * Esta función prueba la función echo.
 */
void test_echo(void);

/**
 * @brief Prueba las funciones start_monitor y stop_monitor
 *
 * Esta función prueba las funciones start_monitor y stop_monitor.
 */
void test_monitor_functions(void);

/**
 * @brief Prueba la función tokenizer
 * Esta función prueba la función tokenizer.
 */
void test_tokenizer(void);

/**
 * @brief Prueba la función show_prompt
 * Esta función prueba la función show_prompt.
 */
void test_show_prompt(void);

// Funciones de configuración y limpieza
void setUp(void)
{
    // Configuración antes de cada prueba
    monitor_pid = MONITOR_OFF; // Restablecer el PID del monitor
}
// Funciones de configuración y limpieza
void tearDown(void)
{
    // Limpieza después de cada prueba
}

/**
 * @brief Función principal de las pruebas
 *
 * Esta función llama a todas las pruebas unitarias.
 */
int main(void)
{
    UNITY_BEGIN(); // Inicia Unity

    // Llama a tus funciones de prueba
    RUN_TEST(test_echo);
    RUN_TEST(test_monitor_functions);
    RUN_TEST(test_tokenizer);
    RUN_TEST(test_show_prompt);

    return UNITY_END(); // Finaliza las pruebas y devuelve el resultado
}

// Prueba la funcion echo
void test_echo(void)
{
    // Configurar un entorno de prueba
    setenv("TEST_VAR", "test_value", 1); // Establece una variable de entorno

    // Preparar entradas y salida esperada
    char* input1[] = {"echo", "Hello,", "$TEST_VAR", "World!", NULL};
    char expected_output1[] = "Hello, test_value World! \n";

    char* input2[] = {"echo", "No", "env", "$NOT_SET", NULL};
    char expected_output2[] = "No env Error con la variable de entorno \n";

    // Redirigir stdout para capturar la salida de la función
    char buffer[256];
    FILE* fp = fmemopen(buffer, sizeof(buffer), "w");
    if (fp == NULL)
    {
        TEST_FAIL_MESSAGE("Error al abrir flujo de memoria");
    }

    // Redirige stdout a nuestro flujo de memoria
    FILE* original_stdout = stdout;
    stdout = fp;

    // Ejecutar la función con el primer caso
    echo(input1);

    // Restaurar y cerrar stdout
    fflush(stdout);
    stdout = original_stdout;
    fclose(fp);

    // Verificar la salida capturada
    TEST_ASSERT_EQUAL_STRING(expected_output1, buffer);

    // Repetir para el segundo caso
    fp = fmemopen(buffer, sizeof(buffer), "w");
    if (fp == NULL)
    {
        TEST_FAIL_MESSAGE("Error al abrir flujo de memoria");
    }

    stdout = fp;
    echo(input2);

    fflush(stdout);
    stdout = original_stdout;
    fclose(fp);

    TEST_ASSERT_EQUAL_STRING(expected_output2, buffer);
}

// Prueba de las funciones start_monitor y stop_monitor
void test_monitor_functions(void)
{
    // Caso 1: Se inicia monitor
    printf("Ejecutando prueba 1: Iniciar monitor...\n");
    start_monitor(); // Llamar a la función para iniciar el monitor

    // Verificar que el archivo contiene los datos esperados
    TEST_ASSERT_GREATER_THAN(0, monitor_pid); // Verificar que el monitor está en ejecución

    // Caso 2: Detener el monitor
    printf("Ejecutando prueba 2: Detener monitor...\n");
    stop_monitor(); // Detener el monitor

    // Verifica que el monitor se detuvo
    TEST_ASSERT_EQUAL_INT(MONITOR_OFF, monitor_pid);
}

// Prueba de la función tokenizer de comandos
void test_tokenizer(void)
{
    // **Caso 1: Tokenizar un comando con múltiples argumentos**
    char command1[] = "ls -l /home/user";
    char* args1[10]; // Espacio suficiente para los argumentos
    int argc1 = tokenizer(command1, args1);

    // Verificar el número de argumentos
    TEST_ASSERT_EQUAL_INT_MESSAGE(3, argc1, "Error: Número incorrecto de argumentos");

    // Verificar los valores de los argumentos
    TEST_ASSERT_EQUAL_STRING_MESSAGE("ls", args1[0], "Error: Argumento 1 incorrecto");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("-l", args1[1], "Error: Argumento 2 incorrecto");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("/home/user", args1[2], "Error: Argumento 3 incorrecto");

    // Verificar que el último argumento es NULL
    TEST_ASSERT_NULL_MESSAGE(args1[argc1], "Error: El último elemento no es NULL");

    // **Caso 2: Tokenizar un comando con un solo argumento**
    char command2[] = "date";
    char* args2[10];
    int argc2 = tokenizer(command2, args2);

    TEST_ASSERT_EQUAL_INT_MESSAGE(1, argc2, "Error: Número incorrecto de argumentos");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("date", args2[0], "Error: Argumento 1 incorrecto");
    TEST_ASSERT_NULL_MESSAGE(args2[argc2], "Error: El último elemento no es NULL");

    // **Caso 3: Tokenizar un comando vacío**
    char command3[] = "";
    char* args3[10];
    int argc3 = tokenizer(command3, args3);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, argc3, "Error: Número incorrecto de argumentos para comando vacío");
    TEST_ASSERT_NULL_MESSAGE(args3[0], "Error: El primer elemento no es NULL para comando vacío");

    // **Caso 4: Tokenizar un comando con espacios iniciales, intermedios y finales**
    char command4[] = "   echo   hello   world   ";
    char* args4[10];
    int argc4 = tokenizer(command4, args4);

    TEST_ASSERT_EQUAL_INT_MESSAGE(3, argc4, "Error: Número incorrecto de argumentos para comando con espacios");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("echo", args4[0], "Error: Argumento 1 incorrecto para comando con espacios");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("hello", args4[1], "Error: Argumento 2 incorrecto para comando con espacios");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("world", args4[2], "Error: Argumento 3 incorrecto para comando con espacios");
    TEST_ASSERT_NULL_MESSAGE(args4[argc4], "Error: El último elemento no es NULL para comando con espacios");
}

// Prueba de la funcion show_prompt
void test_show_prompt(void)
{
    // Redirigir stdout a un archivo temporal
    FILE* stdout_backup = stdout;
    FILE* temp_output = tmpfile();
    TEST_ASSERT_NOT_NULL_MESSAGE(temp_output, "Error: No se pudo crear el archivo temporal para redirigir stdout");
    stdout = temp_output;

    // Llamar a la función show_prompt
    show_prompt();

    // Restaurar stdout
    fflush(temp_output);
    stdout = stdout_backup;

    // Leer la salida redirigida
    fseek(temp_output, 0, SEEK_SET);
    char output[256];
    fgets(output, sizeof(output), temp_output);
    fclose(temp_output);

    // Obtener valores esperados
    char hostname[100];
    char directory[100];
    char* user = getenv("USER");

    gethostname(hostname, sizeof(hostname));
    getcwd(directory, sizeof(directory));

    char expected_output[256];
    snprintf(expected_output, sizeof(expected_output), ANSI_GREEN "%s@%s" ANSI_BLUE ":~%s" ANSI_RESET "$ ", user,
             hostname, directory);

    // Verificar la salida
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_output, output,
                                     "Error: El mensaje mostrado por show_prompt no coincide con lo esperado");
}
