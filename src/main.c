/**
 * @file main.c
 * @brief This file contains the main function.
 */
#include "manager.h"
#include "signals.h"

/**
 * @brief Main function.
 * @param argc the number of arguments.
 * @param argv the array of arguments.
 * @note Supports batch txt.
 */
int main(int argc, char* argv[])
{

    if (argc >= 2)
    {
        FILE* file = fopen(argv[1], "r"); // Read a file line by line and execute it as command until EOF

        if (file != NULL)
        {
            char* line = (char*)malloc(sizeof(char) * 1024);

            while (fgets(line, 1024, file) != NULL)
            {
                execute_command(line);
            }

            fclose(file);
            free(line);
            exit(EXIT_SUCCESS);
        }
        else
        {
            perror("Error opening file\n");
            exit(EXIT_FAILURE);
        }
    }

    while (1)
    {
        manage_signals();
        get_command();
    }

    return EXIT_SUCCESS;
}
