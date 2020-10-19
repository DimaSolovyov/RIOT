#include <stdio.h>
#include <string.h>
#include <time.h>

#include "xtimer.h"
#include "thread.h"
#include "msg.h"
#include "periph/rtc.h"
#include "board.h"

kernel_pid_t pid;
static char stack[THREAD_STACKSIZE_MAIN];
struct tm time_alarm={
        .tm_year = 2020 - 1900,   /* years are counted from 1900 */
        .tm_mon  =  9,                      /* 0 = January, 11 = December */
        .tm_mday = 25,
        .tm_hour = 18,
        .tm_min  = 6,
        .tm_sec  = 1
    
};
static void print_time(const char *label, const struct tm *time)
{
    printf("%s  %04d-%02d-%02d %02d:%02d:%02d\n", label,
            time->tm_year +1900,
            time->tm_mon + 1,
            time->tm_mday,
            time->tm_hour,
            time->tm_min,
            time->tm_sec);
}
static void alarm_callback(void *arg){
    (void) arg;
    msg_t mes;
    mes.content.value=1;
    msg_send(&mes,pid);

}

static void *blink_thread(void *arg)
{
    
    (void) arg;
    msg_t mes;

    while (1)
    {
        msg_receive(&mes);
        LED0_ON;
        xtimer_usleep(1*US_PER_SEC);
        LED0_OFF;
        rtc_get_time(&time_alarm);
        print_time("time: ",&time_alarm);
        time_alarm.tm_sec+=4;
        rtc_set_alarm(&time_alarm,alarm_callback,NULL);
        mktime(&time_alarm);

    }
    
    

    return NULL;
}


int main(void)
{
    puts("RTC alarm RIOT application");
    rtc_init();
    rtc_set_time(&time_alarm);
    rtc_get_time(&time_alarm);
    print_time("time: ",&time_alarm);
    time_alarm.tm_sec=10;
    rtc_set_alarm(&time_alarm,alarm_callback,NULL);

    pid = thread_create(stack, sizeof(stack),
                        THREAD_PRIORITY_MAIN - 1,
                        0,
                        blink_thread,
                        NULL, "thread");

    return 0;
}