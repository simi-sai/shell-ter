/**
 * @file commands.h
 * @brief This file contains the declarations of the functions that handle the commands.
 */
#include "monitor.h"
#include <dirent.h>

/**
 * @brief This function executes the internal commands.
 * @param argc The number of arguments.
 * @param args The arguments of the command.
 * @param background Boolean for background execution.
 */
void internal_commands(int argc, char* args[], bool background);

/**
 * @brief This function changes the directory.
 * @param path The path to change to.
 */
void change_directory(char* path);

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

/** 
 * @brief This function lists the configuration files in the directory.
 * @param directory The directory to list the configuration files.
*/
void list_configuration_files(char* directory);

/**
 * @brief This function searches recursively for the configuration files in the directory.
 * @param directory The directory to search the configuration files.
 */
void search_config_files_recursively(char* directory);

/**
 * @brief This function reads the content of the file.
 * @param filepath The path to the file.
 */
void read_file_content(char* filepath);
