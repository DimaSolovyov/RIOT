
#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "msg.h"
#include "periph/uart.h"
#include "board.h"
#include "xtimer.h"
#include "periph/spi.h"
kernel_pid_t pid;
uart_t uart0=UART_DEV(0);
static char stack[THREAD_STACKSIZE_MAIN];
#define MSG_QUEUE_SIZE 8
static msg_t msg_queue[MSG_QUEUE_SIZE];
static void *printer(void *arg)
{
    (void) arg;
    msg_init_queue(msg_queue, MSG_QUEUE_SIZE);
    msg_t meseg;
    while (1)
    {   
        msg_receive(&meseg);
        uart_write(uart0,(uint8_t*)&meseg.content.value,1);
        xtimer_usleep(1000* US_PER_MS);

    }
    
   

    return NULL;
}
static void uart_callback(void *arg,uint8_t data){
    (void) arg;
    msg_t meseg;
    meseg.content.value=data;
    msg_send(&meseg,pid);
}
int main(void)
{
    puts("UART echo application");

    pid = thread_create(stack, sizeof(stack),
                        THREAD_PRIORITY_MAIN - 1,
                        0,
                        printer,
                        NULL, "thread");
    
    /* initialize UART 0 with a callback */
      
    uart_init(uart0,115200,uart_callback,NULL);
    uart_poweron(uart0);
    spi_init (3);
    
    return 0;
}