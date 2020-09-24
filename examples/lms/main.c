#include <stdio.h>
#include "board.h"
#include "shell.h"
#include "lpsxxx.h"
#include "lpsxxx_params.h"

static lpsxxx_t dev;

static int Temperature(int argc, char **argv){
    int16_t valueTemperature;
    if (argc != 1) {
        printf("usage: %s\n", argv[0]);
        return 1;
    }
    if (lpsxxx_read_temp(&dev,&valueTemperature)!=LPSXXX_OK){
        puts("Cannot initialize hts221 sensor");
        return 1;
    }
    printf("temperature %d,%d%d,",valueTemperature/100,valueTemperature/10%10,valueTemperature%10);
    return 0;
}



static int pressure(int argc,char **argv){
    uint16_t valuePreassure;
    if (argc != 1) {
        printf("usage: %s\n", argv[0]);
        return 1;
    }
    if (lpsxxx_read_pres(&dev,&valuePreassure)!=LPSXXX_OK){
        puts("cannot Read preassure");
        return -1;
    }
    printf("pressure %d",valuePreassure);
    return 0;

}
static const shell_command_t shell_commands[] = {
    { "Temperature", "Temperature", Temperature },
    {"preassure","preassure", pressure},
    { NULL, NULL, NULL }
};

int main(void)

{   
    
    
    if (lpsxxx_init(&dev, &lpsxxx_params[0]) != LPSXXX_OK) {
        puts("Cannot initialize hts221 sensor");
        return -1;
    }
    if (lpsxxx_enable(&dev) != LPSXXX_OK) {
        puts("Cannot initialize hts221 sensor");
        return -2;
    }

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
}