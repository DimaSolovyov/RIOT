#include <stdio.h>
#include "board.h"
#include "shell.h"
#include "hts221.h"
#include "hts221_params.h"
static hts221_t dev;

static int Temperature(int argc, char **argv){
    int16_t valueTemperature;
    if (argc != 1) {
        printf("usage: %s\n", argv[0]);
        return 1;
    }
    if (hts221_read_temperature(&dev,&valueTemperature)!=HTS221_OK){
        puts("Cannot initialize hts221 sensor");
        return 1;
    }
    printf("temperature %d,%d",valueTemperature/10,valueTemperature%10);
    return 0;
}
printf("temperature %d,%d",valueTemperature/10,valueTemperature%10);
static const shell_command_t shell_commands[] = {
    { "Temperature", "Temperature", Temperature },
    { NULL, NULL, NULL }
};



int main(void)

{   
    
    
    if (hts221_init(&dev, &hts221_params[0]) != HTS221_OK) {
    puts("Cannot initialize hts221 sensor");
    return -1;
    }
    if (hts221_power_on(&dev) != HTS221_OK) {
        puts("Failed to set hts221 power on");
        return -2;
    }
     if (hts221_set_rate(&dev,dev.p.rate) != HTS221_OK) {
        puts("Failed to set one_shot power on");
        return -2;
    }
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    
    
    
    
   
    
}