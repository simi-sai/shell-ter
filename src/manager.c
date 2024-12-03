/**
 * @file manager.c
 * @brief This file contains the implementation of the functions that manage the commands.
 */
#include "manager.h"

/**
 * @brief Global variable for the job id.
 */
static int job_id = 0;

/**
 * @brief This function gets the command from the user.
 */
void get_command(void)
{
    show_prompt();
    char* command = (char*)malloc(sizeof(char) * 256);

    fgets(command, 256, stdin);

    execute_command(command);
}

/**
 * @brief This function executes the command.
 * @note This function also handles the background execution of commands.
 */
void execute_command(char* command)
{
    char* args[MAX_ARGS];
    int argc = 0;
    bool background = false;
    bool pipes = false;
    bool input = false;
    bool output = false;

    command[strcspn(command, "\n")] = '\0';

    check_flags(command, &background, &pipes, &input, &output);

    // Pipeline management
    if (pipes)
    {
        run_pipelines(command, args);
        return;
    }

    // I/O management
    if (input || output)
    {
        io_redirection(command, args, &input, &output);
        return;
    }

    argc = tokenizer(command, args);

    if (background)
    {
        pid_t pid = fork();
        job_id++;

        if (pid < 0)
        {
            perror("fork error");
            background = false;
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            // Proceso hijo, ejecutar el comando
            printf("[%d] %d\n", job_id, getpid());
            internal_commands(argc, args, background);
            fflush(stdout);
            show_prompt();
            exit(0);
        }
        else
        {
            // Proceso padre, retornar al shell
            return;
        }
    }
    else
    {
        internal_commands(argc, args, background);
    }
}

/**
 * @brief This function tokenizes the command.
 * @return the number of arguments.
 * @note This function separates the command at each space and stores it in the args array.
 */
int tokenizer(char* command, char* args[])
{
    char* tokens;
    int argc = 0;

    command[strcspn(command, "\n")] = '\0';

    tokens = strtok(command, " ");

    while (tokens != NULL)
    {
        args[argc] = tokens;
        argc++;
        tokens = strtok(NULL, " ");
    }

    args[argc] = NULL;

    return argc;
}

/**
 * @brief This function checks some flags in the command
 */
void check_flags(char* command, bool* background, bool* pipes, bool* input, bool* output)
{
    if (strchr(command, '&') != NULL)
    {
        *background = true;
        command[strcspn(command, "&")] = '\0';
    }
    else
    {
        *background = false;
    }

    if (strchr(command, '|') != NULL)
    {
        *pipes = true;
    }
    else
    {
        *pipes = false;
    }

    if ((strchr(command, '<') != NULL))
    {
        *input = true;
    }
    else
    {
        *input = false;
    }

    if ((strchr(command, '>') != NULL))
    {
        *output = true;
    }
    else
    {
        *output = false;
    }
}

/**
 * @brief This function runs the pipelines.
 */
void run_pipelines(char* command, char* args[])
{
    char* tokens;
    char* commands[MAX_ARGS];
    int argc = 0;
    int i = 0;
    int pipefd[2];
    pid_t pid;

    tokens = strtok(command, "|");

    while (tokens != NULL)
    {
        commands[argc] = tokens;
        argc++;
        tokens = strtok(NULL, "|");
    }

    commands[argc] = NULL;

    int fd_in = 0;

    for (i = 0; i < argc; i++)
    {
        pipe(pipefd);
        pid = fork();

        if (pid < 0)
        {
            perror("fork error");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            dup2(fd_in, 0);

            if (i < argc - 1)
            {
                dup2(pipefd[1], 1);
            }

            close(pipefd[0]);

            tokenizer(commands[i], args);
            internal_commands(argc, args, false);
            exit(0);
        }
        else
        {
            wait(NULL);
            close(pipefd[1]);
            fd_in = pipefd[0];
        }
    }
}

/**
 * @brief This function handles the input output redirection.
 */
void io_redirection(char* command, char* args[], bool* input, bool* output)
{
    char* input_file = NULL;
    char* output_file = NULL;
    int argc = 0;

    char* token = strtok(command, " ");

    while (token != NULL)
    {
        if (strcmp(token, "<") == 0)
        {
            token = strtok(NULL, " ");
            input_file = token;
        }
        else if (strcmp(token, ">") == 0)
        {
            token = strtok(NULL, " ");
            output_file = token;
        }
        else
        {
            args[argc++] = token;
        }
        token = strtok(NULL, " ");
    }

    args[argc] = NULL;

    int saved_stdout = dup(fileno(stdout));
    int saved_stdin = dup(fileno(stdin));

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork error");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        if (*input)
        {
            int fd_in = open(input_file, O_RDONLY);
            if (fd_in < 0)
            {
                perror("open input file");
                exit(EXIT_FAILURE);
            }
            dup2(fd_in, fileno(stdin));
            close(fd_in);
        }
        if (*output)
        {
            int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out < 0)
            {
                perror("open output file");
                exit(EXIT_FAILURE);
            }
            dup2(fd_out, fileno(stdout));
            close(fd_out);
        }
        execvp(args[0], args);
        internal_commands(argc, args, false);
    }

    if (*input)
    {
        fflush(stdin);
        dup2(saved_stdin, fileno(stdin));
        close(saved_stdin);
    }

    if (*output)
    {
        fflush(stdout);
        dup2(saved_stdout, fileno(stdout));
        close(saved_stdout);
    }
}
