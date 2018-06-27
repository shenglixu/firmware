#include "fsl_pit.h"
#include "timer.h"

static volatile uint32_t currentTime, delayLength;

void PIT_TIMER_HANDLER(void)
{
    currentTime++;
    if (delayLength) {
        --delayLength;
    }
    PIT_ClearStatusFlags(PIT, PIT_TIMER_CHANNEL, PIT_TFLG_TIF_MASK);
}

void Timer_Init(void)
{
    pit_config_t pitConfig;
    PIT_GetDefaultConfig(&pitConfig);
    PIT_Init(PIT, &pitConfig);

    // TODO: Why the interval needs to be multiplied by two to arrive to the correct timing?
    // Figure it out and clean this up.
    PIT_SetTimerPeriod(PIT, PIT_TIMER_CHANNEL, MSEC_TO_COUNT(TIMER_INTERVAL_MSEC*2, PIT_SOURCE_CLOCK));

    PIT_EnableInterrupts(PIT, PIT_TIMER_CHANNEL, kPIT_TimerInterruptEnable);
    EnableIRQ(PIT_TIMER_IRQ_ID);
    PIT_StartTimer(PIT, PIT_TIMER_CHANNEL);
}

uint32_t Timer_GetCurrentTime() {
    return currentTime;
}

void Timer_SetCurrentTime(uint32_t *time)
{
    *time = currentTime;
}

uint32_t Timer_GetElapsedTime(uint32_t *time)
{
    uint32_t elapsedTime = currentTime - *time;
    return elapsedTime;
}

uint32_t Timer_GetElapsedTimeAndSetCurrent(uint32_t *time)
{
    uint32_t elapsedTime = Timer_GetElapsedTime(time);
    *time = currentTime;
    return elapsedTime;
}

void Timer_Delay(uint32_t length)
{
    delayLength = length;
    while (delayLength) {
        ;
    }
}
