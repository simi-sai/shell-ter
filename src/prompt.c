/**
 * @file prompt.c
 * @brief This file contains the implementation of the function that shows the prompt.
 */
#include "prompt.h"

/**
 * @brief This function shows the prompt.
 */
void show_prompt(void)
{
    char hostname[HOSTNAME_MAX];
    char directory[DIRECTORY_MAX];
    char* user;

    user = getenv("USER");
    gethostname(hostname, sizeof(hostname));
    getcwd(directory, 100);

    printf(ANSI_GREEN "%s@%s" ANSI_BLUE ":~%s" ANSI_RESET "$ ", user, hostname, directory);

    fflush(stdout);
}
