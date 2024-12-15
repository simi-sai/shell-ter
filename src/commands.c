/**
 * @file commands.c
 * @brief This file contains the implementation of the functions that handle the commands.
 */
#include "commands.h"

/**
 * @brief This function executes the internal commands.
 * @note This function also handles the external commands.
 */
void internal_commands(int argc, char* args[], bool background)
{
    if (argc > 0)
    {
        if (strcmp(args[0], "cd") == 0)
        {
            change_directory(args[1]);
        }
        else if (strcmp(args[0], "quit") == 0)
        {
            if (!background)
                exit(EXIT_SUCCESS);
            else
                perror("No se puede salir en segundo plano");
        }
        else if (strcmp(args[0], "clr") == 0)
        {
            clear_screen();
        }
        else if (strcmp(args[0], "echo") == 0)
        {
            echo(args);
        }
        else if (strcmp(args[0], "start_monitor") == 0)
        {
            start_monitor();
        }
        else if (strcmp(args[0], "stop_monitor") == 0)
        {
            stop_monitor();
        }
        else if (strcmp(args[0], "status_monitor") == 0)
        {
            status_monitor();
        }
        else if (strcmp(args[0], "list_config") == 0)
        {
            list_configuration_files(args[1]);
        }
        else if (strcmp(args[0], "search_config") == 0)
        {
            search_config_files_recursively(args[1]);
        }
        else if (strcmp(args[0], "read_file") == 0)
        {
            read_file_content(args[1]);
        }
        else
        {
            external_command(args);
        }
    }
}

/**
 * @brief This function executes the command as an external program.
 */
void external_command(char* args[])
{
    // El comando exit genera error en execvp
    if (strcmp(args[0], "exit") == 0)
    {
        printf("¿Quisiste decir \"quit\"?\n");
        return;
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        // Error en la creación del proceso
        perror("fork error");
    }
    else if (pid == 0)
    {
        // Proceso hijo: ejecutar el comando
        execvp(args[0], args);

        // Si execvp falla, mostrar el error y terminar el proceso hijo
        perror("execvp error");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Proceso padre: esperar a que termine el hijo
        int status;
        waitpid(pid, &status, 0);
        return;
    }
}

/**
 * @brief This function changes the current directory.
 * @note If the path is NULL, it prints the current directory.
 * @note If the path is "-", it changes to the last directory.
 */
void change_directory(char* path)
{
    char* old_pwd = getenv("PWD");
    char* new_pwd;

    if (path == NULL)
    {
        // Si el comando es "cd" sin argumentos, mostramos el directorio actual
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            fprintf(stdout, "%s\n", cwd);
        }
        else
        {
            perror("cd: cwd");
        }
    }
    else if (strcmp(path, "-") == 0)
    {
        // Si el comando es "cd -", volvemos al último directorio
        new_pwd = getenv("OLDPWD");
    }
    else
    {
        // Intentamos cambiar al directorio indicado
        new_pwd = path;
    }

    if (chdir(new_pwd) == 0)
    {
        // Actualizamos las variables de entorno
        setenv("OLDPWD", old_pwd, 1);
        setenv("PWD", new_pwd, 1);
    }
    else
    {
        perror("cd");
    }
}

/**
 * @brief This function clears the screen.
 */
void clear_screen(void)
{
    printf("\033[H\033[J");
}

/**
 * @brief This function echoes the message.
 * @note Supports environment variables.
 */
void echo(char* message[])
{
    int i = 1;
    while (message[i] != NULL)
    {
        if (strchr(message[i], '$') != NULL)
        {
            char* var = strchr(message[i], '$') + 1;
            if (getenv(var) != NULL)
            {
                fprintf(stdout, "%s ", getenv(var));
            }
            else
            {
                fprintf(stdout, "Error con la variable de entorno ");
            }
        }
        else
        {
            fprintf(stdout, "%s ", message[i]);
        }
        i++;
    }
    fprintf(stdout, "\n");
}

/** --- TP3: Functions Added --- */

void list_configuration_files(char* directory)
{
    struct dirent* entry;
    DIR* dir = opendir(directory);

    if (!dir)
    {
        printf("Error: No se puede abrir el directorio %s\n", directory);
        return;
    }

    printf("Explorando el directorio: %s en busca de archivos '.config' o '.json'\n", directory);

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        { // Archivos regulares
            const char* ext = strrchr(entry->d_name, '.');

            if (ext && (strcmp(ext, ".config") == 0 || strcmp(ext, ".json") == 0))
            {
                printf("Archivo de configuración encontrado: %s/%s\n", directory, entry->d_name);
            }
        }
    }

    closedir(dir);
}

void search_config_files_recursively(char* directory)
{
    struct dirent* entry;
    DIR* dir = opendir(directory);

    if (!dir)
    {
        printf("Error: No se puede abrir el directorio %s\n", directory);
        return;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_DIR)
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                char subpath[1024];
                snprintf(subpath, sizeof(subpath), "%s/%s", directory, entry->d_name);
                search_configs_recursively(subpath);
            }
        }
        else if (entry->d_type == DT_REG)
        {
            const char* ext = strrchr(entry->d_name, '.');
            if (ext && (strcmp(ext, ".config") == 0 || strcmp(ext, ".json") == 0))
            {
                printf("Archivo de configuración encontrado: %s/%s\n", directory, entry->d_name);
            }
        }
    }

    closedir(dir);
}

void read_file_content(char* filepath)
{
    FILE* file = fopen(filepath, "r");
    if (!file)
    {
        printf("Error: No se puede abrir el archivo %s\n", filepath);
        return;
    }

    printf("Contenido de %s:\n", filepath);
    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        printf("%s", line);
    }
    fclose(file);
    printf("\n");
}
