/**
 * @file prompt.h
 * @brief This file contains the declaration of the function that shows the prompt.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * @brief These are the ANSI escape codes for colors.
 * This code is used to color the prompt.
 */
#define ANSI_BLUE "\x1b[34m"

/**
 * @brief These are the ANSI escape codes for colors.
 * This code is used to color the prompt.
 */
#define ANSI_GREEN "\x1b[32m"

/**
 * @brief These are the ANSI escape codes for colors.
 * This code is used to reset the color.
 */
#define ANSI_RESET "\x1b[0m"

/**
 * @brief This is the maximum length of the hostname.
 */
#define HOSTNAME_MAX 100

/**
 * @brief This is the maximum length of the directory.
 */
#define DIRECTORY_MAX 100

/**
 * @brief This function shows the prompt.
 */
void show_prompt(void);
