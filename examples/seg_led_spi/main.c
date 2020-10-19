#include <stdio.h>
#include <string.h>
#include "thread.h"
#include "msg.h"
#include "periph/uart.h"
#include "board.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "shell.h"
#include "xtimer.h"
#define ST_SP GPIO_PIN(PORT_C,7)
#define DIGITONE GPIO_PIN(PORT_B,8)
#define DIGITTWO GPIO_PIN(PORT_B,9)

static const uint8_t led_numbers[10]={0x3F,0x6,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
static msg_t messeage_que[1];


kernel_pid_t pid;
static char stack[THREAD_STACKSIZE_MAIN+800];
spi_t dev=SPI_DEV(0);

void printer_number( uint8_t number ){
    spi_acquire(dev,SPI_CS_UNDEF,SPI_MODE_0,SPI_CLK_10MHZ );
    spi_transfer_byte(dev, SPI_CS_UNDEF,true, number);
    spi_release(dev);
    gpio_write(ST_SP,1 );
    gpio_write(ST_SP,0 );
}
static void *printer(void *arg)
{
    (void) arg;
    msg_t msg;
    static uint8_t counter=0;
    msg_init_queue(messeage_que,1);
    while (1)
    {       
            if (msg_try_receive(&msg)==1){
                if (counter>99){
                    counter=0;
                }
                else{
                    if (gpio_read(BTN0_PIN)){
                        counter++;
                        xtimer_usleep(100);
                    
                    }
                }
                    
            }
            else {
                printer_number(led_numbers[counter%10]);
                gpio_write(DIGITONE,1);
                xtimer_usleep(100);
                gpio_write(DIGITONE,0);
                printer_number(led_numbers[counter/10]);
                gpio_write(DIGITTWO,1);
                xtimer_usleep(100);
                gpio_write(DIGITTWO,0);
            }
         
    }
    
   

    return NULL;
}
static void gp_cb(void *arg){
    (void) arg;
    gpio_toggle(LED0_PIN);
    msg_t mes;
    msg_send(&mes,pid);
    xtimer_usleep(10000);
}
static const shell_command_t commands[] = {
    
    { NULL, NULL, NULL}
};

int main(void) {
    
    puts("SPI LED");
    spi_init(dev);
    int tmp=spi_init_cs(dev,(spi_cs_t)GPIO_PIN(PORT_A, 7));
    if (tmp==SPI_OK) {
        puts("spi_ok");
    }

    gpio_init(ST_SP ,GPIO_OUT);
    gpio_init(DIGITONE,GPIO_OUT);
    gpio_init(DIGITTWO,GPIO_OUT); 
    gpio_init(LED0_PIN,GPIO_OUT);
    gpio_init_int(BTN0_PIN,GPIO_IN,GPIO_RISING,gp_cb,NULL);   
    pid = thread_create(stack, sizeof(stack),
                        THREAD_PRIORITY_MAIN -1,
                        0,
                        printer,
                        NULL, "thread");
    
   char line_buf[SHELL_DEFAULT_BUFSIZE];
   shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    	   
}