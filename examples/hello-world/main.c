/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Hello World application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */
#include <stdio.h>
#include "board.h"
#include "shell.h"

static int toogle(int argc, char **argv)
{   
    if (argc != 2) {
        printf("Usage: toggle <led num>\n\r");
        return 1;
    }
    switch (*argv[1])
    {
    case '0':{
       
        LED0_TOGGLE;
        break;
    }
    case '1':{

        LED1_TOGGLE;
        break;
    }
    case '2':{
        LED2_TOGGLE;
        break;
    }

    default:{
        printf("do not exist\n\r");
        break;
        }
        
        
    }
    
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "toogle", "my own command", toogle },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("RIOT application with shell");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
}