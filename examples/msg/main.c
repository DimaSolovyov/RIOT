#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "msg.h"
#include "shell.h"

static kernel_pid_t pid;
static char stack[THREAD_STACKSIZE_MAIN];

static void *thread_handler(void *arg)
{
    (void) arg;
    msg_t messege;
    while (1)
    {   
        
        msg_receive(&messege);
        printf("%s",(char*)(messege.content.ptr));

        
    }
    
    /* wait for incoming messages and print their content */

    return NULL;
}

static int send_command(int argc, char **argv)
{
    static char str_buff[100];
    msg_t messege;
    if (argc < 2) {
        printf("Usage: %s <message>\n", argv[0]);
        return 1;
    }
    strcpy(str_buff, argv[1]);
    messege.content.ptr=str_buff;
    msg_send(&messege,pid);


    /* send the message to the thread */

    return 0;
}

static const shell_command_t commands[] = {
    { "send", "send a message", send_command },
    { NULL, NULL, NULL}
};

int main(void)
{
    puts("RIOT application with thread IPC");

    pid = thread_create(stack, sizeof(stack),
                        THREAD_PRIORITY_MAIN - 1,
                        0,
                        thread_handler,
                        NULL, "my thread");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}