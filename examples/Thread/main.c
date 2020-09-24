#include <stdio.h>
#include "shell.h"
#include "thread.h"
#include <xtimer.h>
#include "mutex.h"
#define UNUSED(x) (void)(x)
static char stack_read[THREAD_STACKSIZE_MAIN];
static char stack_write[THREAD_STACKSIZE_MAIN];
char buf[200];
mutex_t lock=MUTEX_INIT;

void *write_handler(void *arg)
{       
       
        while (1)
        {   
            mutex_lock(&lock);
            //size_t p= sprintf(buf, "start: %"PRIu32"", xtimer_now().ticks32);
            //xtimer_usleep(200 * US_PER_MS);
            //p += sprintf(&buf[p], " - end: %"PRIu32"", xtimer_now().ticks32);
            //mutex_unlock(&lock);
            printf("thread1 sleep\n\r");
            xtimer_usleep(4000 * US_PER_MS);
            mutex_unlock(&lock);
        }
        arg=NULL;
        /* thread code */
        return arg;
}
void *read_handler(void *arg)
{       
       
        while (1)
        {
            buf[0]='q';
            printf("Thread2 wakeup\n\r");
            mutex_lock(&lock);
            printf("Thread2 sleep\n\r");
            //mutex_unlock(&lock);
            xtimer_usleep(100* US_PER_MS);
            
            
            /* code */
        }
        
        arg=NULL;
        /* thread code */
        return arg;
}

int main(void)
{
                    
                
    puts("First RIOT application with a thread");
    //char line_buf[SHELL_DEFAULT_BUFSIZE];
    //shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);                
    thread_create(stack_write,
                sizeof(stack_write),
                THREAD_PRIORITY_MAIN +1,
                0,
                write_handler,
                NULL,
                "write_buffer");
    thread_create(stack_read,
                sizeof(stack_read),
                THREAD_PRIORITY_MAIN +1,
                0,
                read_handler,
                NULL,
                "read_bufferr");

    
   
    
    return 0;
}