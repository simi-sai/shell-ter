/**
 * @file manager.h
 * @brief This file contains the declaration of the functions that manage the commands.
 */
#include "commands.h"
#include "monitor.h"
#include "prompt.h"
#include <fcntl.h>

/**
 * @brief Maximum number of arguments.
 */
#define MAX_ARGS 64

/**
 * @brief This function gets the command from the user.
 */
void get_command(void);

/**
 * @brief This function executes the command.
 * @param command the command to be executed.
 */
void execute_command(char* command);

/**
 * @brief This function tokenizes the command.
 * @param command the command to be tokenized.
 * @param args the array of arguments.
 * @return the number of arguments.
 */
int tokenizer(char* command, char* args[]);

/**
 * @brief This function checks some flags in the command
 * @param command the command to be checked.
 * @param background pointer to boolean for background execution.
 * @param pipes pointer to boolean for pipes.
 * @param input pointer to boolean for input redirection.
 * @param output pointer to boolean for output redirection.
 */
void check_flags(char* command, bool* background, bool* pipes, bool* input, bool* output);

/**
 * @brief This function handles the pipes.
 * @param command the command to be executed.
 * @param args the array of arguments.
 */
void run_pipelines(char* command, char* args[]);

/**
 * @brief This function handles the I/O redirection.
 * @param command the command to be executed.
 * @param args the array of arguments.
 * @param input pointer to boolean for input redirection.
 * @param output pointer to boolean for output redirection.
 */
void io_redirection(char* command, char* args[], bool* input, bool* output);
