#include <stdio.h>
#include <string.h>
#include "thread.h"
#include "msg.h"
#include "periph/gpio.h"
#include "board.h"
#include "xtimer.h"

kernel_pid_t pid;
static char stack[THREAD_STACKSIZE_MAIN];

static void *thread_handler(void *arg)
{
    (void) arg;
    msg_t mes;
    while(1){
        msg_receive(&mes);
        gpio_toggle(LED0_PIN);

    }
    
    return NULL;
}
static void gp_cb(void *arg){
    (void) arg;
    msg_t mes;
    msg_send(&mes,pid);
    LED1_TOGGLE;

}
int main(void)
{
    puts("Push button RIOT application");
    
    
    pid = thread_create(stack, sizeof(stack),
                        THREAD_PRIORITY_MAIN - 1,
                        0,
                        thread_handler,
                        NULL, "thread");
    
    /* initialize on-board LED1 */
    gpio_init(LED0_PIN,GPIO_OUT);
    
    /* initialize on-board user button */
    gpio_init_int(BTN0_PIN,GPIO_IN,GPIO_RISING,gp_cb,NULL);
    return 0;
}