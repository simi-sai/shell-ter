/**
 * @file commands.h
 * @brief This file contains the declarations of the functions that handle the commands.
 */
#include "monitor.h"

/**
 * @brief This function changes the directory.
 * @param path The path to change to.
 */
void change_directory(char* path);

/**
 * @brief This function executes the internal commands.
 * @param argc The number of arguments.
 * @param args The arguments of the command.
 * @param background Boolean for background execution.
 */
void internal_commands(int argc, char* args[], bool background);

/**
 * @brief This function clears the screen.
 */
void clear_screen(void);

/**
 * @brief This function echoes the message.
 * @param message The message to be echoed.
 */
void echo(char* message[]);

/**
 * @brief This function executes the command as an external program.
 * @param args The arguments of the command.
 */
void external_command(char* args[]);
